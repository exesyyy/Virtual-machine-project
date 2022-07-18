#include "utils.h"
#include <stdio.h>

void stampa(int* prog, unsigned int len){
    
    unsigned int ip = 0;

    while(ip < len){
        int param1, param2, param3;                        /*Puntatori ai parametri delle varie istruzioni.*/
        int type = prog[ip];                               /*La variabile type corrisponde al numero del codice macchina.*/
        
        printf("[% 3d]  ", ip);
        
        ip = params(prog, ip, &param1, &param2, &param3);  /*Richiamo della funzione params.*/
        
        switch (type){                                     /*Assegnazione dei nomi delle istruzioni e gestione del numero di paremetri.*/
            case 0:                                       
                printf("HALT");
                break;
            case 1:
                printf("DISPLAY R%d", param1);
                break;
            case 2:
                printf("PRINT_STACK %d", param1);
                break;
            case 10:
                printf("PUSH R%d", param1);
                break;
            case 11:
                printf("POP R%d", param1);
                break;
            case 12:
                printf("MOV R%d %d", param1, param2);
                break;
            case 20:
                printf("CALL %d", param1);
                break;
            case 21:
                printf("RET");
                break;
            case 22:
                printf("JMP %d", param1);
                break;
            case 73:
                printf("JZ %d R%d", param1, param2);
                break;
            case 74:
                printf("JPOS %d R%d", param1, param2);
                break;
            case 75:
                printf("JNEG %d R%d", param1, param2);
                break;
            case 80:
                printf("ADD R%d R%d R%d", param1, param2, param3);
                break;
            case 81:
                printf("SUB R%d R%d R%d", param1, param2, param3);
                break;
            case 82:
                printf("MUL R%d R%d R%d", param1, param2, param3);
                break;
            case 83:
                printf("DIV R%d R%d R%d", param1, param2, param3);
                break;
        }
        
        printf("\n");
    }
    
}