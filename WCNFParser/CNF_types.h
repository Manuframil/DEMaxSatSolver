#include <limits.h>
// TYPES

typedef int Literal;

static const int LiteralMax = INT_MAX;

typedef struct Clause {
    size_t size;
    size_t capacity;
    size_t weigth;
    Literal literals[1];
} Clause;

typedef struct Entry {
    size_t size;
    size_t capacity;
    int clauses[1];
} Entry; 

typedef struct CNF {
  size_t variable_count;
  size_t clause_count;
  size_t top;
  size_t max_cost;
  Entry **entries;
  Clause **clauses;
} CNF;

