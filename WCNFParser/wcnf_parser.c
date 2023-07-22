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

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "CNF.c"

#define IS_HARD(w, top)    (w >= top ? 1 : 0)

static CNF *read_p_line(FILE *file) {
    assert(file);
    size_t variable_count, clause_count, top;
    int match_count = fscanf(file, "p wcnf %zd %zd %zd", &variable_count, &clause_count, &top);
    if (match_count != 3){
        return NULL;    
    } 
    if (variable_count > LiteralMax){
        return NULL;
    }
    return new_CNF(variable_count, clause_count, top);
}

static bool read_c_line(FILE *file) {
    assert(file);
    char c = fgetc(file);
    if (c!='c')
        return false;
    while ((c = fgetc(file)) != EOF)
        if (c == '\n') return true;
    return false;
}  

static bool read_clauses(FILE *file, CNF *cnf) {
    assert(file);
    bool isWeigth = true;
    bool isHard = false;
    if (!cnf) 
        return false;
    size_t const variable_count = cnf->variable_count;
    //for (Clause **clause_ptr = &cnf->clauses[0]; clause_ptr < &cnf->clauses[(int)cnf->clause_count];) {
    int n_soft = 0;
    int n_hard = 0;
    for (; n_soft + n_hard < cnf->clause_count;) {
        //Clause *clause_ptr = cnf->clauses[i];
        int literal;
        int match_count = fscanf(file, "%d", &literal);
        if (match_count != 1) 
            return false;
        if (isWeigth){
            if (IS_HARD(literal, cnf->top)){
                isHard = true;
                cnf->hard[n_hard] = new_hard_clause();
            } else {
                isHard = false;
                cnf->soft[n_soft] = new_soft_clause(literal);
                cnf->max_cost += literal;
            }
            isWeigth = false;
            continue;
        }
        if (literal == 0) {
            //if (!cnf->clauses[i]) 
            //    return false; // We disallow empty clauses.
            isHard ? n_hard++ : n_soft++;
            isWeigth = true; // El siguiente numero será el peso de la clausula
        }else{
            if (abs(literal) <= variable_count) {
                if (isHard){
                    if (!hard_clause_pushback_literal(&cnf->hard[n_hard], literal)){
                        return false;
                    }
                } else {
                    if (!soft_clause_pushback_literal(&cnf->soft[n_soft], literal)){
                        return false;
                    }
                }
                int pos = abs(literal) - 1;
                if (!entry_pushback_clause(&cnf->entries[pos], n_hard+n_soft))
                    return false;
            } else 
                return false;
        } 
    }
    cnf->soft_count = n_soft;
    cnf->hard_count = n_hard;
    return true;
}

static CNF *read_file(FILE *file){
    assert(file);
    CNF *cnf = NULL;
    for (;;) {
        char const c = fgetc(file);
        if (isspace(c))
            continue;
        if (ungetc(c, file) == EOF){
            printf("Error reading the file...\n");
            free_CNF(cnf);
            exit(1);
        }
        if (c == 'c' && !read_c_line(file)){
            printf("Error reading comments...\n");
            free_CNF(cnf);
            exit(1);
        }        
        else if (c == 'p' && !(cnf = read_p_line(file))){
            printf("Error reading p line...\n");
            free_CNF(cnf);
            exit(1);
        }  
        else if (isdigit(c)){
            if (!read_clauses(file, cnf)){
                printf("Error reading the file...\n");
                free_CNF(cnf);
                exit(1);
            }
            return cnf;
        }
    }

}

