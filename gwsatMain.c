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
#include "DESolver/GWSAT.c"
#include "WCNFParser/wcnf_parser.c"
#include "SettingParser/settingParser.c"


#define CONFIG_FILE "maxsat.cfg"

int is_regular_file(const char *path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void print_description(CNF *cnf, int max_flips, int reps, float WS, int SEED){
    
    printf("c -------------------------------------------\n");
    printf("c GWSAT Solver\n");
    printf("c MaxFlips      = %d\n", max_flips);
    printf("c Repetitions   = %d\n", reps);
    printf("c Noise param   = %.2f\n", WS);
    printf("c Seed          = %d\n", SEED); 
    printf("c Clauses       = %ld \n", cnf->clause_count);
    printf("c Literals      = %ld \n", cnf->variable_count);
    printf("c Total cost    = %ld \n", cnf->max_cost);
    printf("c -------------------------------------------\n");
    printf("\n");

}



int main(int argc,char const *argv[]){

    //Settings
    config_option_t co;
    co = read_config_file(CONFIG_FILE);

    int max_flips = atoi(getval("MAXFLIPS", co));

    int rep = atoi(getval("rep", co));

    int SEED = atoi(getval("SEED", co));

    float WS = atof(getval("WS", co));


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

        strcpy(outpath, path);

        outpath[strlen(outpath)-5] = 0;
        strcat(outpath, "-GWSAT.txt");

        print_description(cnf, max_flips, rep, WS, SEED);       

        gwsat(cnf, max_flips, rep, WS,SEED, outpath);


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

            char filepath[128];
            strcpy(filepath, path);
            strcat(filepath, de->d_name);

            char outpath[128];

            strcpy(outpath, path);
            strcat(outpath, "GWSAT.txt-");
            strcat(outpath, de->d_name);

            outpath[strlen(outpath)-4] = 0;
            strcat(outpath, "txt");


            if ((input = fopen(filepath, "r")) == NULL){
                printf("Error opening file %s\n",filepath);
                exit(1);
            }

            printf("----------------------------------------------------\n");
            printf("Solving %s\n", de->d_name );

            cnf = read_file(input);       
    
            print_description(cnf, max_flips, rep, WS, SEED);       

            gwsat(cnf, max_flips, rep, WS,SEED, outpath);
            
            fclose(input);
            free_CNF(cnf);
            

        }

        closedir(dr);


    }

   
    freeConfig(co);


    return 0;
}
