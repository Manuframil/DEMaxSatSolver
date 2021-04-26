#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Clause.c"
#include "Entry.c"


static inline size_t CNF_max_clause_count() {
  return (SIZE_MAX-sizeof(CNF))/sizeof(Clause*);
}

static inline size_t CNF_max_variable_count() {
  return (SIZE_MAX-sizeof(CNF))/sizeof(Entry*);
}

static size_t CNF_size_for(size_t const clause_count, size_t const variable_count) {
    if (clause_count >= CNF_max_clause_count()) 
        return 0;
    if (variable_count >= CNF_max_variable_count())
        return 0;
    return sizeof(CNF) + clause_count * sizeof(Clause*) + variable_count * sizeof(Entry*);
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
    cnf->entries = malloc(variable_count * sizeof(Entry));
    cnf->clauses = malloc(clause_count * sizeof(Clause));

    for (int i = 0; i < variable_count; i++)
        cnf->entries[i] = new_entry();

    return cnf;
}

static void free_CNF(CNF *cnf) {
    if (!cnf) 
        return;
    for (int i=0; i < cnf->clause_count; i++){
        free_clause(cnf->clauses[i]);
    }
    free(cnf->clauses);
    for (int i=0; i < cnf->variable_count; i++){
        free_entry(cnf->entries[i]);
    }
    free(cnf->entries);

    free(cnf);
}
