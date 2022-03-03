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
#include <time.h>
#include <math.h>
#include <signal.h>
#include "bitOperators.c"
#include "individual.c"
#include "sigterm_handler.c"


/**
 * Picks a random unsatisfied clause from a given individual.
 *
 * @param ind Individual to pick from
 * @return Index of a random unsatisfied clause
 */
static int pick_random_unsat(Individual *ind){
    int r = (ind->unsat_size) ? rand() % (ind->unsat_size) : 0;
    return ind->unsat_clauses[r];
}

/**
 * Picks a random literal from a given clause.
 *
 * @param clause Clause to pick a literal from
 * @return Index in the clause of a random literal
 */
static int pick_random_lit(Clause *clause){
    int r = rand() % (clause->size);
    return abs(clause->literals[r]) - 1;
}

/**
 * Evaluates only the clauses where a variable has changed.
 *
 * @param cnf CNF formula
 * @param ind Individual that is currently being evaluated
 * @param flipped_variable Variable that has been flipped.
 */
void reevaluate(CNF *cnf, Individual **ind, int flipped_variable){
    int v, sat_val, n_supports, var, pos;
    for (int i=0; i<cnf->entries[flipped_variable]->size; i++){
        int cl = cnf->entries[flipped_variable]->clauses[i];
        int currentSat = GetBit((*ind)->clValues, cl);
        sat_val = 0;
        n_supports = 0;
        for (int j = 0; j < cnf->clauses[cl]->size; j++){
            v = cnf->clauses[cl]->literals[j];
            if (!v) continue;
            pos = abs(v) - 1;
            var =  (v < 0) ? !GetBit((*ind)->assigment, pos) : GetBit((*ind)->assigment, pos);
            sat_val = sat_val | var;
            if (var) n_supports++;
        }
        ((*ind)->supports[cl]) = n_supports;
        AssignBit((*ind)->clValues, cl, sat_val);
        ((*ind)->score) += (!sat_val - !currentSat) * (int) cnf->clauses[cl]->weight;
        if (cnf->clauses[cl]->is_hard) (*ind)->hard_unsat += (!sat_val - !currentSat);
    }
}

/**
 * Computes the score of each variable and returns the index of the highest scored variable.
 * The score of a variable represent the global improvement that would be achieved by flipping a variable.
 *
 * @param cl_break Number of clauses that would be broken (unsatisfied) if the variable is flipped
 * @param cl_make Number of clauses that would become satisfied if the variable is flipped
 * @param variable_count Total number of variables
 * @param output_score Score of the best variable i.e. the highest score
 * @return Index of the highest scored variable
 */
int select_variable(const int *cl_break, const int *cl_make, size_t variable_count, int *output_score) {
    int best_score = 0, best_var = 0, score = 0;
    for (int i = 0; i < variable_count; i++) {
        score = cl_make[i] - cl_break[i];
        if (score > best_score) {
            best_score = score;
            best_var = i;
        }
    }
    *output_score = best_score;
    return best_var;
}

/**
 * Computes the values 'make' and 'break' for each variable. \n
 * 'make' stands for the number of clauses that would become satisfied if a variable is flipped \n
 * 'break' stands for the number of clauses that would become unsatisfied if a variable is flipped \n
 *
 * @param cnf CNF formula
 * @param ind Individual that is currently being evaluated
 * @param output_score Score of the best variable i.e. the highest score
 * @return Index of the highest scored variable
 */
int score_variables(CNF *cnf, Individual *ind, int *output_score){
    int i, j, v, pos, var;
    int *cl_break = calloc(cnf->variable_count, sizeof(int));
    int *cl_make = calloc(cnf->variable_count, sizeof(int));
    for (i = 0; i < cnf->clause_count; i++) {
        for (j = 0; j < cnf->clauses[i]->size; j++){
            v = cnf->clauses[i]->literals[j];
            if (!v) continue;
            pos = abs(v) - 1;
            var =  (v < 0) ? !GetBit(ind->assigment, pos) : GetBit(ind->assigment, pos);
            if (var && ind->supports[i] == 1){
                cl_break[pos] += (int) cnf->clauses[i]->weight;
                continue;
            }
            if (!var && !GetBit(ind->clValues, i))
                cl_make[pos]+= (int) cnf->clauses[i]->weight;
        }
    }
    int bv = select_variable(cl_break, cl_make, cnf->variable_count, output_score);
    free(cl_break);
    free(cl_make);
    return bv;
}

/**
 * Calculates the weight of the clauses satisfied by the truth assigment of a given individual.
 *
 * @param cnf CNF formula
 * @param ind Individual that is currently being evaluated
 */
void evaluate(CNF *cnf, Individual **ind){
    int i, j, sat_val, v=0, pos=0, var = 0, n_soportes = 0, newScore = 0;
    (*ind)->score = INT_MAX;
    (*ind)->unsat_size = 0;
    (*ind)->hard_unsat = 0;
    for ( i = 0; i < cnf->clause_count; i++) {
        sat_val = 0;
        n_soportes = 0;
        for (j = 0; j < cnf->clauses[i]->size; j++){
            v = cnf->clauses[i]->literals[j];
            if (!v) continue;
            pos = abs(v) - 1;
            var =  (v < 0) ? !GetBit((*ind)->assigment, pos) : GetBit((*ind)->assigment, pos);
            sat_val |= var;
            if (var) n_soportes++;
        }
        if (!sat_val){
            (*ind)->unsat_clauses[(*ind)->unsat_size] = i;
            (*ind)->unsat_size++;
        }
        (*ind)->supports[i] = n_soportes;
        AssignBit((*ind)->clValues, i, sat_val);
        if (cnf->clauses[i]->is_hard){
            (*ind)->hard_unsat += !sat_val;
        }
        newScore += sat_val ? 0 : (int) cnf->clauses[i]->weight;
    }
    (*ind)->score = newScore;
}

/**
 * Performs the GWSAT over a given individual.
 *
 * @param cnf CNF formula
 * @param ind Individual that is currently being evaluated
 * @param ls_steps Number of Local Search (LS) steps
 * @param RW Probability of perform Random Walk.
 */
void local_search_step(CNF *cnf, Individual *ind, int ls_steps, float RW){
    int best_var;
    for (int k=0; k < ls_steps; k++){
        int sc = -1;
        float r = rand() % 100;
        r /= 100;
        if (r > RW){
            // GSAT
            best_var = score_variables(cnf, ind, &sc);
            if (sc > 0)
                InvertBit(ind->assigment, best_var);
            reevaluate(cnf, &ind, best_var);
        }else{
            // RandomWalk
            int rand_cl = pick_random_unsat(ind);
            int swapping_var = pick_random_lit(cnf->clauses[rand_cl]);
            InvertBit(ind->assigment, swapping_var);
            reevaluate(cnf, &ind, swapping_var);
        }
    }
}

/**
 * Decides whether an individual will be affected by the local search step.
 *
 * @param heuristic_scope Chosen scope
 * @param ind Individual that is currently being evaluated
 * @param population_mean_score Mean score of the population
 * @return True if the ind is affected, False otherwise
 */
int inHeuristicScope(const char* heuristic_scope, Individual *ind, int i, float population_mean_score, int best_individual){
    if (strcmp(heuristic_scope, "all") == 0)
        return 1;
    if ((strcmp(heuristic_scope, "better_than_mean") == 0) && ((float) ind->score < population_mean_score))
        return 1;
    if ((strcmp(heuristic_scope, "best") == 0) && (i == best_individual))
        return 1;
    return 0;
}

/**
 * Implements the main algorithm: Binary Difference Evolution w/ GSAT-RW
 *
 * @param cnf
 * @param gen_max
 * @param pop
 * @param CR
 * @param F
 * @param reps
 * @param LSS
 * @param RW
 * @param maxLSS
 * @param SEED
 * @param hscope
 */
void differential_evolution(CNF *cnf, int gen_max, int pop, float CR, float F, float LSS, float RW, int maxLSS, int SEED, const char* hscope, const char*outfile){

    catch_sigterm();

    FILE *f;
    f = fopen(outfile,"w");

    sigtermMsg.assigment_size = (int) cnf->variable_count;
    sigtermMsg.assigment = calloc(ceil(cnf->variable_count/BitsInt)+1, sizeof(int));
    int *mejor_assigment = calloc(ceil(cnf->variable_count/BitsInt)+1, sizeof(int));

    const int D = (int) cnf->variable_count;    // Dimension vectores
    const int NP = pop;

    int *mutant = (int*) calloc (ceil(D/BitsInt)+1, sizeof(int));
    memset(mutant, 0, ceil(D/BitsInt)+1);

    Individual **inds = (Individual**) calloc(NP, sizeof(Individual*));
    int mejor_score_overall = INT_MAX;
    int mejor_score = INT_MAX;
    float media_poblacion;
    int best_satisfie_hards;
    int count = 0;
    int raw_pushes = (int) ceil(cnf->variable_count * LSS);
    int npush = (maxLSS == -1) ? raw_pushes : fmin(raw_pushes, maxLSS);
    int a = 0, b = 0, c = 0, best_ind;
    if (SEED == -1) srand(time(0));
    else srand(SEED);

    /* Initialize individuals */
    int acc = 0;
    for (int i=0; i<NP; i++) {
        inds[i] = new_individual(D, cnf->clause_count, cnf->clause_count, cnf->clause_count);
        random_initialize(&inds[i]);
        evaluate(cnf, &inds[i]);
        acc += inds[i]->score;
        if (inds[i]->score < mejor_score){
            mejor_score = inds[i]->score;
            best_ind = i;
            best_satisfie_hards = satisfie_all_hard(inds[i]);
        }
    }
    
    media_poblacion = acc/NP;
    /* Halt after gen_max generations. */
    while (count < gen_max || gen_max == -1) {
        acc = 0;
        mejor_score = INT_MAX;

        /* Start loop through population. */
        for (int i=0; i<NP; i++) {
            //printf("score = %d\n",inds[i]->score);
            Individual *indTmp = new_individual(D, cnf->clause_count, cnf->clause_count, cnf->clause_count);
            /*
            * Population improvement step
            * Employ GWSAT Local search algorithm to
            * guide the global search performed by DE.
            * The number of individuals affected by this
            * improvement is determined by the heuristic
            * scope (hscope) parameter.
            */
            if (inHeuristicScope(hscope, inds[i], i, media_poblacion, best_ind)){
                local_search_step(cnf, inds[i], npush, RW);
            }
            /********** Mutate/recombine **********/
            /* Randomly pick 3 vectors, all different from i */
            do a = rand() % NP; while (a==i);
            do b = rand() % NP; while (b==i || b==a);
            do c = rand() % NP; while (c==i || c==a || c==b);
            // Define the mutant of this gen
            for (int j=0; j<D; j++){
                float r = (rand() % 100);
                r = r/100;
                // Mutation
                if (GetBit(inds[b]->assigment, j) != GetBit(inds[c]->assigment, j) && (r < F)){
                    AssignBit(mutant, j, !GetBit(inds[a]->assigment, j));
                } else {
                    AssignBit(mutant, j, GetBit(inds[a]->assigment,j));
                }
                r = (rand() % 100);
                r = r/100;
                // Crossover
                if (r < CR) {
                    AssignBit(indTmp->assigment, j, GetBit(mutant, j));
                } else {
                    AssignBit(indTmp->assigment, j, GetBit(inds[i]->assigment, j));
                }
            }
            /********** Evaluate/select **********/
            /* Evaluate indTmp with fitness function. */
            evaluate(cnf, &indTmp);
            /* If indTmp->score improves on inds[i]->score, update inds[i] */
            if (indTmp->score <= inds[i]->score) {
                deep_copy_ind(&indTmp, &inds[i]);
            }
            free_individual(&indTmp);
            acc += inds[i]->score;

            if (inds[i]->score < mejor_score && satisfie_all_hard(inds[i])){
                mejor_score = inds[i]->score;
                best_ind = i;
                copy_assigment(&inds[i]->assigment, &mejor_assigment, inds[i]->assigment_size);
                best_satisfie_hards = 1;
            }
        }

        // Reset mutant
        memset(mutant, 0, (int) ceilf((float) D/BitsInt)+1);

        /********** End of population loop; swap arrays **********/
        media_poblacion = acc/(float) NP;
        if (mejor_score < mejor_score_overall && best_satisfie_hards) {
            mejor_score_overall = mejor_score;
            copy_assigment(&mejor_assigment, &sigtermMsg.assigment, sigtermMsg.assigment_size);
            sigtermMsg.sol = (int) mejor_score;
            sigtermMsg.satisfie_hard = best_satisfie_hards;
            printf("o %d\n", mejor_score);
        }
        fprintf(f,"%d,%d,%f\n",count, mejor_score_overall, media_poblacion);

        count++;
    }

    print_message();

    for (int i=0; i < NP; i++){
        free_individual(&inds[i]);
    }
    /************************* FREES *************************/
    free(inds);
    free(mutant);
    free(mejor_assigment);
    free(sigtermMsg.assigment);
    fclose(f);

}
