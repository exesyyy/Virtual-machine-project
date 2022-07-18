#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "exitCodes.c"

int isEndOfLine(char* line) {
    int i = 0;
    while(line[i] == ' ' || line[i] == '\n' || line[i] == '\r') i++;
    return line[i] == ';' || line[i]=='\0';
}

/* Data una linea, legge dal suo inizio un numero che viene inserito in n.
   Se il numero esiste ed è stato letto correttamente, viene restituito 1, altrimenti 0. 
*/
int leggiNumero(char* line, int* n) {
    
    int sign; /* Segno del numero: o 1 o -1 */
    int i;    /* Posizione nella line */
    
    i=0;      /* Parto dalla prima posizione della linea */
    sign = 1; /* Assumo all'inizio che il segno sia positivo */
    *n=0;     /* Inizializzo n a 0 */ 
    
    /* Se il primo carattere è un meno, cambio il segno e aumento i */
    if(line[0] == '-'){         
        sign=-1;
        i++;
    }
    
    /* Per ogni carattere nella riga, che è una cifra */
    while (line[i] != '\0' && line[i] >= '0' && line[i] <= '9') { 
        int digit = 9-('9' - line[i]);
        
        if (!safeMul(*n, 10, n)) {
            error(EXIT_CODE_PARSE_ERROR);
        }
        if(sign == -1) {
            if(!safeSub(*n, digit, n)) {
                error(EXIT_CODE_PARSE_ERROR);
            }
        }else{
            if(!safeSum(*n, digit, n)) {
                error(EXIT_CODE_PARSE_ERROR);
            }
        }
        i++;                       /* Aumento la posizione della riga */
    }   

    
    if((sign == 1 && i > 0) || (sign == -1 && i > 1)){          /* Significa che ho letto almeno una cifra */
        if(!isEndOfLine(line + i)){
            error(EXIT_CODE_PARSE_ERROR);
        }
        
        return 1;   /* Restituisco true */
    }else if(!isEndOfLine(line)) {
        /*Errore se non ho trovato un numero e la linea non inzia per ';' e non è vuota (per vuota si intende zero caratteri o un singolo Line Feed (a capo) */
        error(EXIT_CODE_PARSE_ERROR);
        return 0; /* Unreachable */
    }else{
        return 0;   /* Restituisco false */
    }
    
}

/* Legge il file "file" e restituisce un array contenente le istruzioni e i loro parametri,
e inserisce dentro progsize la dimensione di questo array.
Nel caso in cui il file non esista o sia malformato, restituisce NULL. */
int* leggi(char* file, unsigned int *progsize){
    
    char* line; /* Riga che verrà letta */
    size_t len;   /* Dimesione della riga letta */
    FILE *fp;   /* Puntatore al file dal leggere*/
    int *arr;   /* Array da ritornare */
    unsigned int pos;  /* Prossima posizione da riempire in arr */
    int err; /*Flag che indica se la lettura è entrata in errore */
    
    err = 0; 
    pos = 0;
    line = NULL; /* Inizializzo line a NULL */

    fp = fopen(file, "r");           /* Apertura del file */
        
    if (fp == NULL) return NULL;     /* Se fp è NULL, c'è stato un errore nell'apertura del file */
    
    arr=NULL; /* All'inizio non conosco la dimensione dell'array, quindi lo inizializzo a NULL in attesa
              di leggere la dimensione che dovrà avere dal file */
    
    while (!err && getline(&line, &len, fp) != -1) {      /* Lettura riga per riga del file */
        
        int n;
    
        if(leggiNumero(line, &n)){ /*Se ho letto il numero correttamente */
           
            if (arr == NULL){ /*se è il primo numero che leggo*/
                if(n<0) {
                    err = 1;  /* La dimensione non può essere negativa */
                }else{
                    arr = (int*)malloc(sizeof(int)*n);    /*Allocazione dinamica dell'array.*/
                    *progsize = (unsigned int) n;
                }
            }else if(pos >= *progsize) {    
                err = 1;  /* Troppi numeri rispetto alla progsize */
           }else{
               arr[pos] = n; /* Aggiungo il numero all'array di ritorno */
               pos++;
           }
        }
    }
    
    fclose(fp);          /*Chiusura del file tramite "fclose".*/
    
    if(line) {           /* libero la memoria a fine funzione.*/
        free(line);
    }
    
    if(arr && pos != *progsize) {
        err=1; /* Vado in errore se ho letto meno istruzioni di quante me ne aspettavo */ 
    }
    
    if(err || !arr) {         /* Se sono in uno stato d'errore o non ho letto niente */
        if(arr){
            free(arr);  /* Se l'array non è NULL, libero la memoria occupata */
        }
        error(EXIT_CODE_PARSE_ERROR);
    }
    
    return arr;

}