#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "CNF.c"



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
    if (!cnf) 
        return false;
    size_t const variable_count = cnf->variable_count;
    //for (Clause **clause_ptr = &cnf->clauses[0]; clause_ptr < &cnf->clauses[(int)cnf->clause_count];) {
    for (int i = 0; i < cnf->clause_count;) {
        //Clause *clause_ptr = cnf->clauses[i];
        int literal;
        int match_count = fscanf(file, "%d", &literal);
        if (match_count != 1) 
            return false;
        if (isWeigth){
            cnf->clauses[i] = new_clause(literal);
            cnf->max_cost += literal;
            isWeigth = false;
            continue;
        }
        if (literal == 0) {
            if (!cnf->clauses[i]) 
                return false; // We disallow empty clauses.
            //clause_ptr++;
            i++;
            isWeigth = true; // El siguiente numero será el peso de la clausula
        }
        else{
            if (abs(literal) <= variable_count) {
                if (!clause_pushback_literal(&cnf->clauses[i], literal)){
                    return false;
                }
                int pos = abs(literal) - 1;
                if (!entry_pushback_clause(&cnf->entries[pos], i))
                    return false;
            } else 
                return false;
            

        } 
    }
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

