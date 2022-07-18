#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "exitCodes.c"


/* Dato il programma, un instruction pointer, e 3 puntatori per i parametri
   questa funzione legge l'istruzione all'IP passato, e a seconda del suo
   tipo, inserisce nei puntatori param1, param2, param3 i valori dei parametri.
   Restituisce il prossimo IP, saltando le posizioni occupate dai parametri.*/
   
int params(int* prog, unsigned int ip, int* param1, int* param2, int* param3){ 
    
    int p;          /* Numeri dei parametri */
    int type = prog[ip];

    switch (type){  /* Utilizzo di switch case per gestire il numero di parametri delle varie istruzioni */
        
        case 1:
        case 2:
        case 10:
        case 11:
        case 20:
        case 22:
            p = 1;  /* Queste istruzioni hanno un singolo parametro */
            break;
        case 12:
        case 73:
        case 74:
        case 75:
            p = 2;  /* 2 parametri */
            break;
        case 80:
        case 81:
        case 82:
        case 83:
            p = 3;  /* 3 parametri */
            break;
        default:
            p=0;
    }
    
    if (p >= 1){
        *param1 = prog[ip + 1];     
    }
    if (p >= 2){
        *param2 = prog[ip + 2];
    }
    if (p == 3){
        *param3 = prog[ip + 3];
    }

    return ip + p + 1;
    
}

int safeSub(int v1, int v2, int* result) {
    if ((v2 < 0 && v1 > INT_MAX + v2) || (v2 > 0 && v1 < INT_MIN + v2)){
        return 0;
    }
    
    *result = v1-v2;
    return 1;
}

int safeSum(int v1, int v2, int* result) {
    
    if ((v2 > 0 && v1 > INT_MAX - v2) || (v2 < 0 && v1 < INT_MIN - v2)){
        return 0;
    }
   
    *result = v1+v2;
    return 1;
}


/* La funzione error gestisce i vari casi di errore */
void error(int exitcode) {
    
    switch (exitcode){
            
        case EXIT_CODE_PARSE_ERROR:
            printf ("PARSE ERROR: Il file dato non esiste o non è valido\n");
            break;
        case EXIT_CODE_UNKNOWN_INSTRUCTION:
            printf("UNKNOWN INSTRUCTION\n");   /* Istruzione data non valida */
            break;
        case EXIT_CODE_INVALID_REGISTER:
            printf ("INVALID REGISTER\n");     /* Se il registro preso in considerazione non è dentro il limite */
            break;
        case EXIT_CODE_STACK_OVERFLOW:
            printf ("STACK OVERFLOW\n");
            break;
        case EXIT_CODE_STACK_UNDERFLOW:
            printf ("STACK UNDERFLOW\n");
            break;
        case EXIT_CODE_ARITHMETIC_OVERFLOW:
            printf ("ARITHMETIC OVERFLOW\n");
            break;
        case EXIT_CODE_DIVISION_BY_ZERO:
            printf ("DIVISION BY ZERO\n");
            break;
        default:
            printf("UNKNOWN ERROR\n"); /* non dovrebbe mai accadere */
    }
    exit(exitcode);
    
}

int safeMul(int v1, int v2, int *result) {
    if ((v1 > INT_MAX / v2) || (v1 < INT_MIN / v2)) { 
        return 0;
    }
    
    *result = v1*v2;
    return 1;
}