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

#define CONFIG_ARG_MAX_BYTES 128

typedef struct config_option config_option;
typedef config_option* config_option_t;

struct config_option {
    config_option_t prev;
    char key[CONFIG_ARG_MAX_BYTES];
    char value[CONFIG_ARG_MAX_BYTES];
};

config_option_t read_config_file(char* path) {

    FILE* fp;
    
    if ((fp = fopen(path, "r+")) == NULL) {
        // perror("Config file not found! Using default values");
        return NULL;
    }
    
    config_option_t last_co_addr = NULL;
    
    while(1) {
        config_option_t co = NULL;
        if ((co = calloc(1, sizeof(config_option))) == NULL)
            continue;
        memset(co, 0, sizeof(config_option));
        co->prev = last_co_addr;
        
        if (fscanf(fp, "%s %s", &co->key[0], &co->value[0]) != 2) {
            if (feof(fp)) {
                free(co);
                break;
            }
            if (co->key[0] == '#') {
                while (fgetc(fp) != '\n') {
                    // Do nothing (to move the cursor to the end of the line).
                }
                free(co);
                continue;
            }
            perror("fscanf()");
            free(co);
            continue;
        }
        //printf("Key: %s\nValue: %s\n", co->key, co->value);
        last_co_addr = co;
    }

    fclose(fp);
    return last_co_addr;
}

void freeConfig(config_option_t co){
    while(co != NULL) {
        config_option_t tmp = co;
        co = co->prev;
        free(tmp);
    }
    //free(co);
}

char* getval(char *key, config_option_t co){
	while(1) {
		if (strcmp(key, co->key) == 0){
			return co->value;
		}
        if (co->prev != NULL) {
            co = co->prev;
        } else {
            break;
        }
    }
    return NULL;
}