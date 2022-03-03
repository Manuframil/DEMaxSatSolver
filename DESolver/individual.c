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

#include <assert.h>

typedef struct individual {
    int score; // Clauses satisfied x their weights
    int hard_unsat; // Number of unsatisfied clauses
    int *assigment; // Bit Array containing the literals assigment
    int assigment_size;
    int *clValues;  // Bit array containing 1->sat cl & 0->unsat cl
    int clValues_size;
    int *supports; // Number of supports per clause
    int supports_size;
    int *unsat_clauses;  // Indices of unsatisfied clauses
    int unsat_size;
    int unsat_capacity;
} Individual;

/**
 * Creates and allocates the memory needed for a new individual.
 *
 * @param assigment_size Number of variables of the cnf formula
 * @param clValues_size Number of clauses of the cnf formula
 * @param supports_size Number of clauses of the cnf formula
 * @param unsat_size Number of clauses of the cnf formula
 * @return Pointer to the new individual
 */
static Individual *new_individual(int assigment_size, int clValues_size, int supports_size, int unsat_size){
    Individual *ind = malloc(sizeof(Individual));
    ind->score = INT_MAX;
    ind->hard_unsat = 0;
    ind->assigment_size = assigment_size;
    ind->assigment = calloc((int) ceil((double) assigment_size/BitsInt)+1, sizeof(int));
    memset(ind->assigment, 0,(int) ceil((double) assigment_size/BitsInt)+1);
    ind->clValues_size = clValues_size;
    ind->clValues = calloc((int) ceil((double) clValues_size/BitsInt)+1, sizeof(int));
    memset(ind->clValues, 0,(int)ceil((double) clValues_size/BitsInt)+1);
    ind->supports_size = supports_size;
    ind->supports = calloc(supports_size, sizeof(int));
    memset(ind->supports, 0,(int)ceil((double) supports_size/BitsInt)+1);
    ind->unsat_capacity = unsat_size;
    ind->unsat_size = 0;
    ind->unsat_clauses = calloc(unsat_size, sizeof(int));
    memset(ind->unsat_clauses, 0, (int) ceil((double) unsat_size / BitsInt) + 1);
    return ind;
}

/**
 * Randomly initialize the assigment of a given individual
 *
 * @param ind
 */
static void random_initialize (Individual **ind){
	for (int i = 0; i < (*ind)->assigment_size; i++){
		AssignBit((*ind)->assigment, i, rand() % 2);
	}
}

/**
 * Liberates memory of a given individual.
 *
 * @param ind
 */
static void free_individual(Individual **ind){
	free((*ind)->assigment);
	free((*ind)->clValues);
	free((*ind)->supports);
	free((*ind)->unsat_clauses);
	free((*ind));
}

/**
 * Copy the assigment from one individual to another.
 *
 * @param or Origin individual
 * @param dest Destination individual
 * @param assigment_size Size of the assigment
 */
static void copy_assigment(int **or, int **dest, int assigment_size){
    for (int i = 0; i < assigment_size; i++){
        AssignBit((*dest), i, GetBit((*or), i));
    }
}

/**
 * Deep clone of one individual into another.
 *
 * @param or Origin individual
 * @param dest Destination individual
 */
static void deep_copy_ind(Individual **or, Individual **dest){
    assert((*or)->assigment_size == (*dest)->assigment_size);
    assert((*or)->clValues_size == (*dest)->clValues_size);
    assert((*or)->supports_size == (*dest)->supports_size);
    assert((*or)->unsat_capacity == (*dest)->unsat_capacity);

    (*dest)->score = (*or)->score;
    (*dest)->hard_unsat = (*or)->hard_unsat;
    for (int i = 0; i < (*or)->assigment_size; i++){
        AssignBit((*dest)->assigment, i, GetBit((*or)->assigment, i));
    }
    for (int i = 0; i < (*or)->clValues_size; i++){
        AssignBit((*dest)->clValues, i, GetBit((*or)->clValues, i));
        (*dest)->supports[i] = (*or)->supports[i];
        (*dest)->unsat_clauses[i] = (*or)->unsat_clauses[i];
    }
    (*dest)->unsat_size = (*or)->unsat_size;
}

/**
 * Check if a given individual satisfies all hard clauses.
 *
 * @param ind
 * @return True if all hard clauses are satisfied, false otherwise.
 */
static int satisfie_all_hard(Individual *ind){
    return !(ind->hard_unsat);
}