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
	// Clauses satisfied x their weights
    int score;
    // Clauses satisfied      
    int solution;
    // Bit Array containing the literals assigment   
    int *assigment; 
    int assigment_size;
    // Bit array containing 1->sat cl & 0->unsat cl
    int *clValues;  
    int clValues_size;
    // Number of supports for clause
    int *supports;  
    int supports_size;
    // Indices of unsat clauses
    int *unsatCl;
    int unsatCl_pos;
    int unsatCl_size;

} Individual;

static Individual *new_individual(int assigment_size, 
	int clValues_size, int supports_size, int unsatCl_size){
    
    Individual *ind = malloc(sizeof(Individual));

    ind->assigment_size = assigment_size;
    ind->assigment = calloc(ceil(assigment_size/BitsInt)+1, sizeof(int));
    memset(ind->assigment, 0, ceil(assigment_size/BitsInt)+1);

    ind->clValues_size = clValues_size;
    ind->clValues = calloc(ceil(clValues_size/BitsInt)+1, sizeof(int));
    memset(ind->clValues, 0,ceil(clValues_size/BitsInt)+1);

    ind->supports_size = supports_size;
    ind->supports = calloc(supports_size, sizeof(int));
    memset(ind->supports, 0,ceil(supports_size/BitsInt)+1);

    ind->unsatCl_size = unsatCl_size;
    ind->unsatCl_pos = 0;
    ind->unsatCl = calloc(unsatCl_size, sizeof(int));
    memset(ind->unsatCl, 0,ceil(unsatCl_size/BitsInt)+1);


    return ind;
}

static void random_initialize (Individual **ind){

	for (int i = 0; i < (*ind)->assigment_size; i++){
		AssignBit((*ind)->assigment, i, rand() % 2);
	}
}


static void free_individual(Individual **ind){

	free((*ind)->assigment);
	free((*ind)->clValues);
	free((*ind)->supports);
	free((*ind)->unsatCl);
	free((*ind));

}

static void copy_assigment(int **or, int **dest, int assigment_size){
    for (int i = 0; i < assigment_size; i++){
        AssignBit((*dest), i, GetBit((*or), i));
    }
}

static void deep_copy_ind(Individual **or, Individual **dest){

    assert((*or)->assigment_size == (*dest)->assigment_size);
    assert((*or)->clValues_size == (*dest)->clValues_size);
    assert((*or)->supports_size == (*dest)->supports_size);
    assert((*or)->unsatCl_size == (*dest)->unsatCl_size);


    (*dest)->score = (*or)->score;
    (*dest)->solution = (*or)->solution;

    for (int i = 0; i < (*or)->assigment_size; i++){
        AssignBit((*dest)->assigment, i, GetBit((*or)->assigment, i));
    }

    for (int i = 0; i < (*or)->clValues_size; i++){
        AssignBit((*dest)->clValues, i, GetBit((*or)->clValues, i));
        (*dest)->supports[i] = (*or)->supports[i];
        (*dest)->unsatCl[i] = (*or)->unsatCl[i];
    }

    (*dest)->unsatCl_pos = (*or)->unsatCl_pos;
}