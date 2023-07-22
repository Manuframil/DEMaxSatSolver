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

static inline size_t soft_clause_max_capacity (){
    return SIZE_MAX/sizeof(Literal);
}

static size_t soft_clause_size_for(size_t const count_of_literals) {
    assert(count_of_literals);
    if (count_of_literals > soft_clause_max_capacity())
        return 0;
    return count_of_literals * sizeof(Literal);
}

static size_t soft_clause_next_capacity(size_t const capacity) {
    assert(capacity);
    const size_t growth_factor = 2;
    if (capacity > soft_clause_max_capacity()/growth_factor) {
        if (capacity < soft_clause_max_capacity())
            return soft_clause_max_capacity();
        return 0;
    }
    return capacity * growth_factor;
}


static SoftClause* new_soft_clause_impl(size_t const capacity, int const weight){
    size_t const clause_alloc_size = sizeof(SoftClause);
    size_t const literal_alloc_size = soft_clause_size_for(capacity);
    assert(clause_alloc_size);
    assert(weight);
    SoftClause *const clause = malloc(clause_alloc_size);
    int *const literals = malloc(literal_alloc_size);
    if (!clause) return NULL;
    if (!literals) return NULL;
    if (weight > SIZE_MAX) return NULL;
    clause->size = 0;
    clause->capacity = capacity;
    clause->weight = weight;
    clause->literals = literals;
    memset(clause->literals, 0, capacity);
    return clause;
}

static SoftClause *new_soft_clause(int const weight){
    return new_soft_clause_impl(4, weight);
}

static void free_soft_clause(SoftClause *clause){
    free(clause->literals);
    free(clause);
}

static bool grow_soft_clause(SoftClause **clause_ptr, size_t by){
    assert(clause_ptr);
    if (!*clause_ptr) return false;
    SoftClause *const clause = *clause_ptr;
    // Overflow
    if (clause->size > (SIZE_MAX - by)) return false;
    if (by > soft_clause_max_capacity()) return false;
    if (clause->size > (soft_clause_max_capacity() - by)) return false;
    // New size
    size_t const new_size = clause->size + by;
    assert(new_size <= soft_clause_max_capacity());
    assert(clause->size <= clause->capacity);
    if (new_size > clause->capacity){
        size_t new_capacity = clause->capacity;
        while (new_capacity && new_capacity < new_size){
            new_capacity = soft_clause_next_capacity(new_capacity);
        }
        if (!new_capacity) return false;
        Literal *const new_literals = realloc(clause->literals, soft_clause_size_for(new_capacity));
        if (!new_literals) return false;
        (*clause_ptr)->literals = new_literals;
        (*clause_ptr)->capacity = new_capacity;
        memset((*clause_ptr)->literals + (*clause_ptr)->size, 0, new_capacity-(*clause_ptr)->size);
    }
    return true;
}

static bool soft_clause_pushback_literal(SoftClause **clause_ptr, Literal literal){
    assert(clause_ptr);
    assert(literal);
    if (!grow_soft_clause(clause_ptr, 1) )
        return false;
    (*clause_ptr)->literals[(*clause_ptr)->size++] = literal;
    return true;
}
