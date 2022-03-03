/**
 * MaxSAT solver based on Differential Evolution
 * Copyright (C) 2021  Manuel Framil de Amorin (m.framil.deamorin@udc.es)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#include "WCNFParser/CNF_types.h"
#include "DESolver/BDE.c"
#include "WCNFParser/wcnf_parser.c"
#include "SettingParser/settingParser.c"
#include <argp.h>
#include <libgen.h>

const char *argp_program_version = "DEMaxSAT solver v0.1";
const char *argp_program_bug_address = "m.framil.deamorin@udc.es";
static char doc[] = "DEMaxSAT solver is a MaxSAT solved which combines Differential Evolution "
                    "with ad-hoc maxsat heuristics, such as GSAT and RandomWalk.";
static char args_doc[] = "WCNF \t Path to '.wcnf' file";
static struct argp_option options[] = {
        { "gens", 'g',"GENS",0, "Max generations. No limit=-1. Default=-1"},
        { "pop", 'p',"POP",0, "Population size. Default=100"},
        { "cr", 'c',"CR",0, "Crossover probability. Default=0.4f"},
        { "f", 'f',"F",0, "Mutation probability. Default=0.6f"},
        { "lss", 'l',"LSS",0, "Number of Local Search Steps. "
                              "Is a percentage of the number of variables. Default=0.01"},
        { "maxlss", 'm',"maxLSS",0, "Max number of LSS on each call to local heuristics. No limit=-1."
                                    "Default=100"},
        { "seed", 's',"SEED",0, "Random numbers seed. Randomly chosen = -1. Default=-1"},
        { "rw", 'r',"RW",0, "RandomWalk probability. GSAT prob=(1-RW). Default=0.5f"},
        { "hscope", 'h',"H_SCOPE",0, "Individuals affected by the Local Search "
                                     "heuristics [all|better_than_mean|best]. Default=all"},
        { 0 }
};

static int n_valid = 3;
static char* valid_hscope[] =  {
        "all",
        "better_than_mean",
        "best"
};

struct arguments {
    char *wcnf[1];
    int gens, pop, maxlss, seed;
    float cr, f, rw, lss;
    char *hscope;
};

int validate_hscope(char *arg){
    for (int i = 0; i < n_valid; i++){
        if (strcmp(valid_hscope[i], arg) == 0)
            return 1;
    }
    return 0;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'g':
            arguments->gens = atoi(arg);
            break;
        case 'p':
            arguments->pop = atoi(arg);
            break;
        case 'l':
            arguments->lss = atof(arg);
            break;
        case 'm':
            arguments->maxlss = atoi(arg);
            break;
        case 's':
            arguments->seed = atoi(arg);
            break;
        case 'c':
            arguments->cr = atof(arg);
            break;
        case 'f':
            arguments->f = atof(arg);
            break;
        case 'r':
            arguments->rw = atof(arg);
            break;
        case 'h':
            if (validate_hscope(arg)){
                arguments->hscope = arg;
            }else{
                printf("c WARNING: %s is not a valid HSCOPE. Using hscope=all.\n", arg);
                return 0;
            }
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 1)
                argp_usage (state);
            arguments->wcnf[state->arg_num] = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1)
                argp_usage (state);
            break;
        default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int is_regular_file(const char *path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void print_description(CNF *cnf, struct arguments arg){
    printf("c -------------------------------------------\n");
    printf("c BINARY DIFFERENTIAL EVOLUTION MAXSAT SOLVER\n");
    printf("c Generations   = %d\n", arg.gens);
    printf("c Population    = %d\n", arg.pop);
    printf("c Crossover     = %.2f\n", arg.cr);
    printf("c Mutation      = %.2f\n", arg.f);
    printf("c Noise param   = %.2f\n", arg.rw);
    printf("c Seed          = %d\n", arg.seed);
    printf("c LS Step       = %d (%.2f%%) \n", (int) ceilf(arg.lss*cnf->variable_count), arg.lss);
    printf("c Max LSS       = %d\n", arg.maxlss);
    printf("c Clauses       = %ld \n", cnf->clause_count);
    printf("c Literals      = %ld \n", cnf->variable_count);
    printf("c Total cost    = %ld \n", cnf->max_cost);
    printf("c h - scope     = %s \n", arg.hscope);
    printf("c -------------------------------------------\n");
}

int main(int argc,char **argv){

    struct arguments arguments;
    arguments.gens = -1;
    arguments.pop = 100;
    arguments.cr = 0.4f;
    arguments.f = 0.6f;
    arguments.lss = 0.01f;
    arguments.maxlss = 100;
    arguments.seed = -1;
    arguments.rw = 0.5f;
    arguments.hscope = "all";

    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    //printf("%s\n", arguments.wcnf[0]);
    char path[512];
    strcpy(path, arguments.wcnf[0]);

    if (is_regular_file(path)){
        CNF *cnf;
        FILE *input;
        if ((input = fopen(path, "r")) == NULL){
            printf("Error opening file %s\n",path);
            exit(1);
        }

        char buff[20];
        time_t now = time(NULL);
        strftime(buff, 20, "_%d.%m.%y_%H.%M.%S", localtime(&now));

        char outfile[512];
        strcpy(outfile, path);
        strcat(outfile, buff);
        strcat(outfile,"_gens.log");
        printf("%s\n",outfile);

        cnf = read_file(input);
        print_description(cnf, arguments);
        differential_evolution(cnf, arguments.gens, arguments.pop, arguments.cr, arguments.f,
                               arguments.lss, arguments.rw, arguments.maxlss, arguments.seed,
                               arguments.hscope, outfile);
        fclose(input);
        free_CNF(cnf);
    }
    return 0;
}
