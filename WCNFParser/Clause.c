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

#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

static inline size_t Clause_max_capacity (){
    return SIZE_MAX/sizeof(Literal);
}

static size_t Clause_size_for(size_t const count_of_literals) {
    assert(count_of_literals);
    if (count_of_literals > Clause_max_capacity())
        return 0;
    return count_of_literals * sizeof(Literal);
}

static size_t Clause_next_capacity(size_t const capacity) {
    assert(capacity);
    const size_t growth_factor = 2;
    if (capacity > Clause_max_capacity()/growth_factor) {
        if (capacity < Clause_max_capacity())
            return Clause_max_capacity();
        return 0;
    }
    return capacity * growth_factor;
}


static Clause* new_clause_impl(size_t const capacity, int const weight, unsigned int const is_hard){
    size_t const clause_alloc_size = sizeof(Clause);
    size_t const literal_alloc_size = Clause_size_for(capacity);
    assert(clause_alloc_size);
    assert(weight);
    Clause *const clause = malloc(clause_alloc_size);
    int *const literals = malloc(literal_alloc_size);
    if (!clause) return NULL;
    if (!literals) return NULL;
    if (weight > SIZE_MAX) return NULL;
    clause->size = 0;
    clause->capacity = capacity;
    clause->weight = weight;
    clause->is_hard = is_hard;
    clause->literals = literals;
    memset(clause->literals, 0, capacity);
    return clause;
}

static Clause *new_clause(int const weight, unsigned int const is_hard ){
    return new_clause_impl(4, weight, is_hard);
}

static void free_clause(Clause *clause){
    free(clause->literals);
    free(clause);
}

static bool grow_clause(Clause **clause_ptr, size_t by){
    assert(clause_ptr);
    if (!*clause_ptr) return false;
    Clause *const clause = *clause_ptr;
    // Overflow
    if (clause->size > (SIZE_MAX - by)) return false;
    if (by > Clause_max_capacity()) return false;
    if (clause->size > (Clause_max_capacity() - by)) return false;
    // New size
    size_t const new_size = clause->size + by;
    assert(new_size <= Clause_max_capacity());
    assert(clause->size <= clause->capacity);
    if (new_size > clause->capacity){
        size_t new_capacity = clause->capacity;
        while (new_capacity && new_capacity < new_size){
            new_capacity = Clause_next_capacity(new_capacity);
        }
        if (!new_capacity) return false;
        Literal *const new_literals = realloc(clause->literals, Clause_size_for(new_capacity));
        if (!new_literals) return false;
        (*clause_ptr)->literals = new_literals;
        (*clause_ptr)->capacity = new_capacity;
        memset((*clause_ptr)->literals + (*clause_ptr)->size, 0, new_capacity-(*clause_ptr)->size);
    }
    return true;
}

static bool clause_pushback_literal(Clause **clause_ptr, Literal literal){
    assert(clause_ptr);
    assert(literal);
    if (!grow_clause(clause_ptr, 1) )
        return false;
    (*clause_ptr)->literals[(*clause_ptr)->size++] = literal;
    return true;
}
