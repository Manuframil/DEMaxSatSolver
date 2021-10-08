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
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

static inline size_t Entry_max_capacity (){
    return (SIZE_MAX - sizeof(Entry))/sizeof(int);
}

static size_t Entry_size_for(size_t const count_of_clauses){
	assert(count_of_clauses);
	if (count_of_clauses > Entry_max_capacity())
		return 0;
	return sizeof(Entry) + count_of_clauses * sizeof(int);
}

static size_t Entry_next_capacity(size_t const capacity){
	assert(capacity);
	const size_t growth_factor = 2;
	if (capacity > Entry_max_capacity()/growth_factor){
		if (capacity < Entry_max_capacity())
			return Entry_max_capacity();
		return 0;
	}
	return capacity * growth_factor;
}

static Entry* new_entry_impl(size_t const capacity){
	size_t const alloc_size = Entry_size_for(capacity);
	assert(alloc_size);
	Entry *const entry = calloc(capacity, alloc_size);
	if (!entry)
		return NULL;
	entry->size = 0;
	entry->capacity = capacity;
	return entry;
}

static Entry *new_entry(){
	return new_entry_impl(4);
}

static void free_entry(Entry *entry){
	free(entry);
}

static bool grow_entry(Entry **entry_ptr, size_t by){
	assert(entry_ptr);
	if (!*entry_ptr)
		return (*entry_ptr = new_entry_impl(by));
	Entry *const entry = *entry_ptr;
	//Overflow
	if (entry->size > (SIZE_MAX - by))
		return false;
	if (by > Entry_max_capacity())
		return false;
	if (entry->size > (Entry_max_capacity() - by))
		return false;
	// New size
	size_t const new_size = entry->size + by;
    assert(new_size <= Entry_max_capacity());
	assert(entry->size <= entry->capacity);
	if (new_size > entry->capacity){
		size_t new_capacity = entry->capacity;
		while (new_capacity && new_capacity < new_size){
			new_capacity = Entry_next_capacity(new_capacity);
		}
		if (!new_capacity)
			return false;
		// Aumentamos el espacio para la entrada
		Entry *const new_entry = realloc(entry, Entry_size_for(new_capacity));
		if (!new_entry)
			return false;
		*entry_ptr = new_entry;
		(*entry_ptr)->capacity = new_capacity;
	}
	(*entry_ptr)->clauses[new_size] = 0;
	return true;
}

static bool entry_pushback_clause(Entry **entry_ptr, int clause_id){
	assert(entry_ptr);
	assert(clause_id >= 0);
	if (!grow_entry(entry_ptr, 1))
		return false;
	(*entry_ptr)->clauses[(*entry_ptr)->size++] = clause_id;
	return true;
}