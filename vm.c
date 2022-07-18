#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "leggi.h"
#include "stampa.h"
#include "esegui.h"

int main(int argc, char** argv) {
    if (argc != 3){
        printf ("Devi dare i 2 parametri\n");           /*Verifico che ci siano 3 parametri in ingresso.*/
        return 1;
    }else{
        
        int* prog;
        unsigned int progsize;
        prog=leggi(argv[2], &progsize);                 /*Richiamo della funzione "leggi" per aprire il contenuto del file.*/
    
        if (prog == NULL) { 
            free(prog);
            printf("Il file non è stato trovato\n.");
            return 1;
        } 
        
        if (strcmp(argv[1], "stampa") == 0) {    
            stampa(prog, progsize);                 /*Richiamo della funzione stampa.*/
            
            free(prog);
            return 0;
        }else if(strcmp(argv[1], "esegui") == 0) {   /*Richiamo della funzione esegui.*/
            esegui(prog, progsize);
            
            free(prog);
            return 0;
        }else{
            printf("Comando non valido.\n");
            free(prog);
            return 1;
        }
    }
    
}

