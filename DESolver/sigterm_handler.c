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

struct sigtermMsg {
    int assigment_size;
    int *assigment;
    int sol;
    int satisfie_hard;
} sigtermMsg;

void assigment_to_string (char **dest ,const int *assigment, int assigment_size){
    memset((*dest), 0, assigment_size);
    for (int i=0; i < assigment_size; i++){
        sprintf(&(*dest)[i],"%d", GetBit(assigment, i));
    }
}

void print_solution_not_found(){
    const char *out = "c SOLUTION NOT FOUND\n";
    //write(STDERR_FILENO, out, 21);
    printf("%s", out);
}


void print_final_line(){
    char *final_line = malloc(sizeof(char) * (sigtermMsg.assigment_size + 128) );
    char *vline = malloc(sizeof(char) * sigtermMsg.assigment_size + 1);
    assigment_to_string(&vline, sigtermMsg.assigment, sigtermMsg.assigment_size);
    size_t msg_size = sizeof(char) * (sigtermMsg.assigment_size + 128);
    memset(final_line, 0x00, msg_size );
    const char *format = "s UNKNOWN\no %d\nv %s\n";
    snprintf(final_line, msg_size, format, sigtermMsg.sol, vline );
    //write(STDERR_FILENO, final_line, sigtermMsg.assigment_size + sizeof(int) + 18);
    printf("%s", final_line);
    free(vline);
    free(final_line);
}

void print_message(){
    if (sigtermMsg.satisfie_hard)
        print_final_line();
    else
        print_solution_not_found();
}

void sigterm_handler(int signum, siginfo_t *info, void *ptr){
    print_message();
    exit(0);
}

void catch_sigterm(){
    static struct sigaction _sigact;
    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = sigterm_handler;
    _sigact.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &_sigact, NULL);
}
