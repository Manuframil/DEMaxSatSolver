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

#include <limits.h>
// TYPES

typedef int Literal;

static const int LiteralMax = INT_MAX;

typedef struct Clause {
    size_t size;
    size_t capacity;
    size_t weight;
    unsigned int is_hard;
    Literal *literals;
} Clause;

typedef struct Entry {
    size_t size;
    size_t capacity;
    int *clauses;
} Entry; 

typedef struct CNF {
  size_t variable_count;
  size_t clause_count;
  size_t top;
  size_t max_cost;
  Entry **entries;
  Clause **clauses;
} CNF;

