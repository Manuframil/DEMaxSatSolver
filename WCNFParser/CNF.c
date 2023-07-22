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

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include "HardClause.c"
#include "SoftClause.c"
#include "Entry.c"


static inline size_t CNF_max_clause_count() {
  return (SIZE_MAX-sizeof(CNF))/sizeof(SoftClause*);
}

static inline size_t CNF_max_variable_count() {
  return (SIZE_MAX-sizeof(CNF))/sizeof(Entry*);
}

static size_t CNF_size_for(size_t const clause_count, size_t const variable_count) {
    if (clause_count >= CNF_max_clause_count()) 
        return 0;
    if (variable_count >= CNF_max_variable_count())
        return 0;
    return sizeof(CNF) + clause_count * sizeof(SoftClause*) + variable_count * sizeof(Entry*);
}

static CNF *new_CNF(size_t variable_count, size_t clause_count, size_t top) {
    assert(variable_count <= LiteralMax);
    size_t const cnf_size = CNF_size_for(clause_count, variable_count);
    CNF *cnf = malloc(cnf_size);
    if (!cnf)
        return NULL; 
    cnf->variable_count = variable_count;
    cnf->clause_count = clause_count;
    cnf->top = top;
    cnf->max_cost = 0;
    cnf->soft_count = 0;
    cnf->hard_count = 0;
    cnf->entries = calloc(variable_count, sizeof(Entry));
    cnf->soft = calloc(clause_count, sizeof(SoftClause));
    cnf->hard = calloc(clause_count, sizeof(HardClause));

    for (int i = 0; i < variable_count; i++)
        cnf->entries[i] = new_entry();
    return cnf;
}

static void free_CNF(CNF *cnf) {
    if (!cnf) 
        return;
    for (int i=0; i < cnf->soft_count; i++){
        free_soft_clause(cnf->soft[i]);
    }
    free(cnf->soft);
    for (int i=0; i < cnf->hard_count; i++){
        free_hard_clause(cnf->hard[i]);
    }
    free(cnf->hard);
    for (int i=0; i < cnf->variable_count; i++){
        free_entry(cnf->entries[i]);
    }
    free(cnf->entries);
    free(cnf);
}
