#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include "bitOperators.c"
#include "individual.c"

static int pick_random_unsat(Individual *ind){

    int r = rand() % (ind->unsatCl_pos);
    return ind->unsatCl[r];

}

static int pick_random_lit(CNF *cnf, int cl){

    int r = rand() % (cnf->clauses[cl]->size);
    return abs(cnf->clauses[cl]->literals[r]) - 1;

}

void reevaluate(CNF *cnf, Individual **ind, int swap){

    int v, sat_val = 0, n_soportes = 0;

    for (int i=0; i<cnf->entries[swap]->size; i++){
        int cl = cnf->entries[swap]->clauses[i];
        // Obtenemos el valor que tiene ahora mismo para determinar si cambia
        int currentSat = GetBit((*ind)->clValues, cl);
        sat_val = 0;
        n_soportes = 0;

        for (int j = 0; j < cnf->clauses[cl]->size; j++){
            v = cnf->clauses[cl]->literals[j];
            if (!v) continue;
            
            int pos = abs(v) - 1;
            int var;

            if (v < 0) var = !GetBit((*ind)->assigment, pos); 
            else var = GetBit((*ind)->assigment, pos);


            sat_val = sat_val | var;
            if (var){
                n_soportes++;
            }
        }

        ((*ind)->supports[cl]) = n_soportes;

        AssignBit((*ind)->clValues, cl, sat_val);

        ((*ind)->solution) += (sat_val - currentSat);
        ((*ind)->score) += (sat_val - currentSat) * cnf->clauses[cl]->weigth;
    }

}


int select_variable(int *clbreak, int *clmake, int variable_count, int *sc){

    int best_score = 0;
    int best_var = 0;
    int score = 0;

    for (int i = 0; i < variable_count; i++){
        score = clmake[i] - clbreak[i];
        if (score > best_score){
            best_score = score;
            best_var = i;
        }
    }
    *sc = best_score;
    return best_var;
}

int score_variables(CNF *cnf, Individual *ind, int *sc){
    // clbreak: Peso de clausulas que una variable haria falsas si se flipea 
    int *clbreak = calloc(cnf->variable_count, sizeof(int));
    // clmake: Peso de las clausulas que una variable haria verdaderas si se flipea
    int *clmake = calloc(cnf->variable_count, sizeof(int));

    int v = 0, pos = 0;
    int i = 0, j = 0;


    for (i = 0; i < cnf->clause_count; i++) { 

        for (j = 0; j < cnf->clauses[i]->size; j++){
            v = cnf->clauses[i]->literals[j];
            if (!v) continue;

            pos = abs(v) - 1;
            int var;

            if (v < 0) var = !GetBit(ind->assigment, pos); 
            else var = GetBit(ind->assigment, pos);

            if (var && ind->supports[i] == 1){
                clbreak[pos] += cnf->clauses[i]->weigth;
                continue;
            }
            if (!var && !GetBit(ind->clValues, i))
                clmake[pos]+= cnf->clauses[i]->weigth;
        }
    }
    //int sc;
    int bv = select_variable(clbreak, clmake, cnf->variable_count, sc);
    free(clbreak);
    free(clmake);
    return bv;
}


void evaluate(CNF *cnf, Individual **ind){
    int i, j, sat_val, v, pos;
    int n_soportes = 0;
    int newScore = 0;
    (*ind)->solution = 0;
    (*ind)->score = 0;
    (*ind)->unsatCl_pos = 0;

    for ( i = 0; i < cnf->clause_count; i++) {
        sat_val = 0;
        n_soportes = 0;
        for (j = 0; j < cnf->clauses[i]->size; j++){

            v = cnf->clauses[i]->literals[j];
            if (!v) continue;
            
            pos = abs(v) - 1;
            int var;

            if (v < 0) var = !GetBit((*ind)->assigment, pos); 
            else var = GetBit((*ind)->assigment, pos);

            sat_val = sat_val | var;
            if (var){
                n_soportes++;
            }
        }
        
        if (!sat_val){
            (*ind)->unsatCl[(*ind)->unsatCl_pos] = i;
            (*ind)->unsatCl_pos++;
        }
        
        (*ind)->supports[i] = n_soportes;

        AssignBit((*ind)->clValues, i, sat_val);

        (*ind)->solution += sat_val;
        newScore += (sat_val * cnf->clauses[i]->weigth);
        
    }

    (*ind)->score = newScore;

}

void gwsat (CNF*cnf, int max_flips, int reps, float WS, int SEED, const char *outfile) {

    int flips = 0;
    FILE *fp = NULL;
    fp = fopen(outfile, "w+");
    int max_score = 0;


    for (int rep=0; rep < reps; rep++){

        flips = 0;

        if (SEED == -1)
            srand(time(0));
        else
            srand(SEED);
    
        Individual *ind = new_individual(cnf->variable_count, cnf->clause_count, cnf->clause_count, cnf->clause_count);

        random_initialize(&ind);

        evaluate(cnf, &ind);

    
        /* Halt after gen_max generations. */
        while (1) {
            
            int sc = -1;
            
            float r = rand() % 100;
            r /= 100;

            if (r > WS){
                // GSAT
                int best_var = score_variables(cnf, ind, &sc);
                if (sc > 0)
                    InvertBit(ind->assigment, best_var);
                reevaluate(cnf, &ind, best_var);
            
            }else{
                // WalkSAT
                int rand_cl = pick_random_unsat(ind);
                int swapping_var = pick_random_lit(cnf, rand_cl);
                InvertBit(ind->assigment, swapping_var);
                reevaluate(cnf, &ind, swapping_var);
            }
            if (flips % 10 == 0)
                fprintf(fp, "%d \n", ind->score);

            if (ind->score > max_score){
                max_score = ind->score;
                printf("o %ld\n",cnf->max_cost - max_score );
            }

            flips++;

        }

        free_individual(&ind);
        

    }


}