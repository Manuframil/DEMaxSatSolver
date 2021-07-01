#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include "bitOperators.c"
#include "individual.c"

struct sigtermMsg {
    int assigment_size;
    int *assigment;
    int sol;
} sigtermMsg;


void assigment_to_string (char *dest ,const int *assigment, int assigment_size){

    char *out = malloc(assigment_size);
    memset(out, 0, assigment_size);

    for (int i=0; i < assigment_size; i++){
        char aux[8];
        sprintf(aux, "%d", GetBit(assigment, i));
        strcat(out, aux);
    }

    strcpy(dest, out);
    free(out);
}

void sigterm_handler(int signum, siginfo_t *info, void *ptr){

    char *final_line = malloc(sizeof(char) * (sigtermMsg.assigment_size + 128) );
    
    char *vline = malloc(sizeof(char) * sigtermMsg.assigment_size);
    assigment_to_string(vline, sigtermMsg.assigment, sigtermMsg.assigment_size);

    size_t msg_size = sizeof(char) * (sigtermMsg.assigment_size + 128);

    memset(final_line, 0x00, msg_size );

    const char *format = "\ns UNKNOWN\no %d\nv %s\n";
    snprintf(final_line, msg_size, format, sigtermMsg.sol, vline );

    write(STDERR_FILENO, final_line, sigtermMsg.assigment_size + sizeof(int) + 17);

    free(vline);
    free(final_line);

    exit(0);
}

void catch_sigterm(){
    static struct sigaction _sigact;

    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = sigterm_handler;
    _sigact.sa_flags = SA_SIGINFO;

    sigaction(SIGTERM, &_sigact, NULL);
}

static int pick_random_unsat(Individual *ind){


    //printf("%d\n", ind->unsatCl_pos);

    int r = (ind->unsatCl_pos) ? rand() % (ind->unsatCl_pos): 0;
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


int select_variable(const int *cl_break, const int *cl_make, int variable_count, int *sc) {

    int best_score = 0;
    int best_var = 0;
    int score = 0;

    for (int i = 0; i < variable_count; i++) {
        score = cl_make[i] - cl_break[i];
        if (score > best_score) {
            best_score = score;
            best_var = i;
        }
    }
    *sc = best_score;
    return best_var;
}

int score_variables(CNF *cnf, Individual *ind, int *sc){
    // cl_break: Peso de clausulas que una variable haria falsas si se flipea
    int *cl_break = calloc(cnf->variable_count, sizeof(int));
    // cl_make: Peso de las clausulas que una variable haria verdaderas si se flipea
    int *cl_make = calloc(cnf->variable_count, sizeof(int));

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
                cl_break[pos] += cnf->clauses[i]->weigth;
                continue;
            }
            if (!var && !GetBit(ind->clValues, i))
                cl_make[pos]+= cnf->clauses[i]->weigth;
        }
    }
    //int sc;
    int bv = select_variable(cl_break, cl_make, cnf->variable_count, sc);
    free(cl_break);
    free(cl_make);
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

            sat_val |= var;
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


void local_search_step(CNF *cnf,Individual **inds, int currentInd, int npush, float RW){

    int best_var;

    for (int k=0; k<npush; k++){

        int sc = -1;

        float r = rand() % 100;
        r /= 100;

        if (r > RW){
            // GSAT
            best_var = score_variables(cnf, inds[currentInd], &sc);
            if (sc > 0)
                InvertBit(inds[currentInd]->assigment, best_var);
            reevaluate(cnf, &inds[currentInd], best_var);

        }else{
            // RandomWalk
            int rand_cl = pick_random_unsat(inds[currentInd]);
            int swapping_var = pick_random_lit(cnf, rand_cl);
            InvertBit(inds[currentInd]->assigment, swapping_var);
            reevaluate(cnf, &inds[currentInd], swapping_var);
        }
    }
}


int inHeuristicScope(const char* hscope,Individual *ind, float mediapoblacion){

    if (strcmp(hscope, "all") == 0)
        return 1;
    if ((strcmp(hscope, "better_than_mean") == 0) && (ind->score > mediapoblacion))
        return 1;
    return 0;
}



void differential_evolution(CNF *cnf, int gen_max, int num_inds, float CR, float F, 
    int reps, float LSS, float RW, int maxLSS, int SEED, const char* hscope, const char *outfile){

    catch_sigterm();

    sigtermMsg.assigment_size = (int) cnf->variable_count;

    const int D = (int) cnf->variable_count;    // Dimension vectores
    const int NP = num_inds;


    float *all_bests = calloc(gen_max, sizeof(float));
    float *all_means = calloc(gen_max, sizeof(float));
    float *all_times = calloc(gen_max, sizeof(float));


    int *mutant = (int*) calloc (ceil(D/BitsInt)+1, sizeof(int));
    memset(mutant, 0, ceil(D/BitsInt)+1);


    Individual **inds = (Individual**) calloc(NP, sizeof(Individual*));
    clock_t genStart, genEnd;
    FILE *fp = NULL;
    float seconds;
    int mejor_individuo;
    int mejor_score_overall;
    int mejor_score = 0;
    float media_poblacion;


    fp = fopen(outfile, "w+");
  
    int count = 0;

    int npush = fmin((int) ceil(cnf->variable_count * LSS), maxLSS);

    int a = 0, b = 0, c = 0;

    if (SEED == -1)
        srand(time(0));
    else
        srand(SEED);

    mejor_score_overall = 0;

    genStart = clock();

    /* Initialize individuals */
    int acc = 0;
    for (int i=0; i<NP; i++) {

        inds[i] = new_individual(D, cnf->clause_count, cnf->clause_count, cnf->clause_count);

        random_initialize(&inds[i]);

        evaluate(cnf, &inds[i]);
        acc += inds[i]->score;
        if (inds[i]->score > mejor_score){
            mejor_score = inds[i]->score;
            mejor_individuo = i;
        }
    }
    
    media_poblacion = acc/NP;
    genEnd = clock();
    seconds = (float) (genEnd - genStart) / CLOCKS_PER_SEC;
    fprintf(fp, "%lu , %lu, %f, %f\n", cnf->max_cost - mejor_score, cnf->max_cost - mejor_score, cnf->max_cost - media_poblacion, seconds);
    /* Halt after gen_max generations. */
    while (count < gen_max) {

        acc = 0;
        mejor_score = 0;
        mejor_individuo = 0;


        /* Start loop through population. */
        for (int i=0; i<NP; i++) {

            /*
            * Population improvement step
            * Employ GWSAT Local search algorithm to
            * guide the global search performed by DE.
            * The number of individuals affected by this
            * improvement is determined by the heuristic
            * scope (hscope) parameter.
            */
            if (inHeuristicScope(hscope, inds[i], media_poblacion)){
                local_search_step(cnf, inds, i, npush, RW);
            }


            /********** Mutate/recombine **********/
            /* Randomly pick 3 vectors, all different from i */
            do a = rand() % NP; while (a==i);
            do b = rand() % NP; while (b==i || b==a);
            do c = rand() % NP; while (c==i || c==a || c==b);


            Individual *indTmp = new_individual(D, cnf->clause_count, cnf->clause_count, cnf->clause_count);
            //indTmp->assigment = trialPos;

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
            if (indTmp->score >= inds[i]->score) {
                deep_copy_ind(&indTmp, &inds[i]);
            }

            free_individual(&indTmp);

            acc += inds[i]->score;

            if (inds[i]->score > mejor_score){
                mejor_score = inds[i]->score;
                mejor_individuo = i;
            }
        }

        // Reset mutant
        memset(mutant, 0, (int) ceilf((float) D/BitsInt)+1);

        /********** End of population loop; swap arrays **********/

        media_poblacion = acc/(float) NP;

        all_bests[count] += (float) mejor_score/ (float) reps;
        all_means[count] += media_poblacion/ (float) reps;


        if (mejor_score > mejor_score_overall) {
            mejor_score_overall = mejor_score;

            sigtermMsg.assigment = inds[mejor_individuo]->assigment;
            sigtermMsg.sol = (int) cnf->max_cost - mejor_score;
            printf("o %ld\n",cnf->max_cost - mejor_score );
        }


        genEnd = clock();
        seconds = (float) (genEnd - genStart) / CLOCKS_PER_SEC;
        all_times[count] += seconds;

        fprintf(fp, "%lu , %lu, %f, %f\n", cnf->max_cost - mejor_score_overall, cnf->max_cost - mejor_score, cnf->max_cost - media_poblacion, seconds);

        count++;
    }

    for (int i=0; i < NP; i++){
        free_individual(&inds[i]);
    }


    /************************* FREES *************************/

    free(inds);
    free(mutant);
    
    fclose(fp);

    free(all_bests);
    free(all_means);
    free(all_times);

}
