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
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include "WCNFParser/CNF_types.h"
#include "DESolver/BDE.c"
#include "WCNFParser/wcnf_parser.c"
#include "SettingParser/settingParser.c"

#define CONFIG_FILE "maxsat.cfg"

const char *output_format = "%s_HS_%s_CR%.2f_F%.2f_LSS%.2f_RW%.2f";

int is_regular_file(const char *path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void print_description(CNF *cnf, int gen_max, int NP, float CR, float F, 
    int reps, float LSS, float RW, int maxLSS, int SEED, const char* hscope){
    
    printf("c -------------------------------------------\n");
    printf("c BINARY DIFFERENTIAL EVOLUTION MAXSAT SOLVER\n");
    printf("c Generations   = %d\n", gen_max);
    printf("c Population    = %d\n", NP);
    printf("c Crossover     = %.2f\n", CR);
    printf("c Mutation      = %.2f\n", F);
    printf("c Repetitions   = %d\n", reps);
    printf("c Noise param   = %.2f\n", RW);
    printf("c Seed          = %d\n", SEED); 
    printf("c LS Step       = %d (%.2f%%) \n", (int) ceil(LSS*cnf->variable_count), LSS);
    printf("c Max LSS       = %d\n", maxLSS);
    printf("c Clauses       = %ld \n", cnf->clause_count);
    printf("c Literals      = %ld \n", cnf->variable_count);
    printf("c Total cost    = %ld \n", cnf->max_cost);
    printf("c h - scope     = %s \n", hscope);
    printf("c -------------------------------------------\n");
    printf("\n");
}

int main(int argc,char const *argv[]){
    //Settings
    config_option_t co;
    co = read_config_file(CONFIG_FILE);

    int gen_max = atoi(getval("GEN", co)); //Numero maximo de generaciones
    int NP = atoi(getval("NP", co));  // Numero individuos
    float CR = atof(getval("CR", co)); // Crossover probability
    float F = atof(getval("F", co));  // Mutation probability
    int rep = atoi(getval("rep", co));
    float LSS = atof(getval("LSS", co));
    int maxLSS = atoi(getval("maxLSS", co));
    int SEED = atoi(getval("SEED", co));
    float RW = atof(getval("RW", co));
    const char* hscope = getval("H-SCOPE", co);

    if (argv[1] == NULL){
        printf("Error: No wcnf file was given!\n");
        exit(0);
    }
    char path[128];
    strcpy(path, argv[1]);
    if (is_regular_file(path)){
        CNF *cnf;
        FILE *input;
        if ((input = fopen(path, "r")) == NULL){
            printf("Error opening file %s\n",path);
            exit(1);
        }
        cnf = read_file(input);
        char outpath[128];
        if (argv[2] == NULL){
            char filename[64];
            strcpy(filename, path);
            filename[strlen(filename)-5] = 0;
            snprintf(outpath, 128, output_format, filename, hscope, CR, F, LSS, RW);
            strcat(outpath, ".txt");
        } else {
            strcpy(outpath, argv[2]);
        }
        print_description(cnf, gen_max, NP, CR, F, rep, LSS, RW, maxLSS, SEED, hscope);
        differential_evolution(cnf, gen_max, NP, CR, F, rep, LSS, RW, maxLSS,SEED, hscope, outpath);
        fclose(input);
        free_CNF(cnf);
    } else {
        struct dirent *de;
        DIR *dr = opendir(path);
        if (dr == NULL){
            printf("Error opening dir %s\n", path);
            exit(0);
        }
        while ((de=readdir(dr)) != NULL){
            if (!strcmp (de->d_name, "."))
                continue;
            if (!strcmp (de->d_name, ".."))    
                continue;
            if (de->d_type == DT_DIR) 
                continue;
            if (strcmp(de->d_name + (strlen(de->d_name)-4), "wcnf") != 0)
                continue;
            CNF *cnf;
            FILE *input;
            char outpath[128];
            char filename[128];
            strcpy(filename, path);
            strcat(filename, de->d_name);
            if ((input = fopen(filename, "r")) == NULL){
                printf("Error opening file %s\n",filename);
                exit(1);
            }
            filename[strlen(filename)-5] = 0;
            snprintf(outpath, 128, output_format, filename, hscope, CR, F, LSS, RW);
            strcat(outpath, ".txt");
            printf("----------------------------------------------------\n");
            printf("Solving %s\n", de->d_name );
            cnf = read_file(input);
            differential_evolution(cnf, gen_max, NP, CR, F, rep, LSS, RW, maxLSS, SEED, hscope, outpath);
            fclose(input);
            free_CNF(cnf);
        }
        closedir(dr);
    }
    freeConfig(co);
    return 0;
}
