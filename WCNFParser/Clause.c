#include <limits.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>


static inline size_t Clause_max_capacity (){
    return (SIZE_MAX - sizeof(Clause))/sizeof(Literal);
}

static size_t Clause_size_for(size_t const count_of_literals) {
    assert(count_of_literals);
    if (count_of_literals > Clause_max_capacity())
        return 0;
    return sizeof(Clause) + count_of_literals * sizeof(Literal);
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


static Clause* new_clause_impl(size_t const capacity, int const weigth){
    size_t const alloc_size = Clause_size_for(capacity);
    assert(alloc_size);
    assert(weigth);
    Clause *const clause = calloc(capacity, alloc_size);
    if (!clause)
        return NULL;
    if (weigth > SIZE_MAX)
        return NULL;
    clause->size = 0;
    clause->capacity = capacity;
    clause->weigth = weigth;
    return clause;
}

static Clause *new_clause(int const weigth){
    return new_clause_impl(4, weigth);
}

static void free_clause(Clause *clause){
    free(clause);
}

static bool grow_clause(Clause **clause_ptr, size_t by){
    assert(clause_ptr);
    if (!*clause_ptr)
        return false;//new_clause_impl(by, 1);
    Clause *const clause = *clause_ptr;
    // Overflow
    if (clause->size > (SIZE_MAX - by))
        return false;
    if (by > Clause_max_capacity()) 
        return false;
    if (clause->size > (Clause_max_capacity() - by))
        return false;
    // New size
    size_t const new_size = clause->size + by;
    assert(new_size <= Clause_max_capacity());
    assert(clause->size <= clause->capacity);
    if (new_size > clause->capacity){
        size_t new_capacity = clause->capacity;
        while (new_capacity && new_capacity < new_size){
            new_capacity = Clause_next_capacity(new_capacity);
        }
        if (!new_capacity)
            return false;
        // Aumenta el espacio de la clausula
        Clause *const new_clause = realloc(clause, Clause_size_for(new_capacity));
        if (!new_clause)
            return false;
        *clause_ptr = new_clause;
        (*clause_ptr)->capacity = new_capacity;

    }
    (*clause_ptr)->literals[new_size] = 0;
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
