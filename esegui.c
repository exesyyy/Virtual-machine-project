#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "utils.h"
#include "exitCodes.c"


#define N_REGS 32   /* Numero dei registri */
#define STACK_SIZE 65536 /* Dimensione in byte dello stack (64 * 1024) */


/* Struttura che contiene le informazioni necessarie per eseguire la macchina virtuale */
struct memory{
    
    int* stack;                      /* Array che contiene gli elementi dello stack */
    unsigned int stacksize;          /* Dimensione massima dello stack */
    int* regs;                       /* Array che contiene il valore dei registri, di dimensione N_REGS */
    unsigned int SP;                 /* Stack pointer */
    unsigned int IP;                 /* Instruction Pointer */
};

typedef struct memory* Memory;

/* Crea e inizializza un'istanza della memory nell'heap e restituisce il suo puntatore */
Memory createMemory() {
    
    Memory m = (Memory)malloc(sizeof(struct memory));
    
    m->SP = 0;          /* All'inizio lo stack è vuoto */                   
    m->stacksize = STACK_SIZE / sizeof(int);                 /* Calcolo numero di interi per STACK_SIZE byte */
    m->stack = (int*)malloc(sizeof(int)*m->stacksize);       /* Alloco stack per STACK_SIZE byte */
    
    m->regs = (int*)malloc(sizeof(int)*N_REGS);              /* Alloco N_REGS registri */
    m->IP = 0;                  /* All'inizio l'IP punterà alla prima istruzione (0) */
    
    return m;
    
}

/* Libera la memoria */
void freeMemory(Memory m) {
    
    free(m->stack);
    free(m->regs);
    free(m);
}

void vmError(Memory mem, int exitcode) {
    
    freeMemory(mem);
    error(exitcode);
    
}

/* Questa funzione verifica che il registro da considerare rispetti il limite di 32 */
void checkRegister(Memory mem, int reg) {
    if(reg < 0 || reg >= N_REGS) {
        vmError(mem, EXIT_CODE_INVALID_REGISTER);
    }
}

/* Prende il valore del registro reg e lo restituisce */
int getRegisterValue(Memory mem, int reg){
    checkRegister(mem, reg);
    return mem->regs[reg];
}

/* Assegna il valore val al registro reg */
void setRegisterValue(Memory mem, int reg, int val){
    checkRegister(mem, reg);
    mem->regs[reg] = val;
}

/* Stampa su console il registro reg */
void printRegister(Memory m, int reg) {
    
    printf("REG[%d] = %d\n", reg, m->regs[reg]);       
    
}


/* Esegue una push nello stack del valore val */
void pushValue(Memory mem, int val) {
    
    if(mem->SP >= mem->stacksize){
        vmError(mem, EXIT_CODE_STACK_OVERFLOW);
    }
    
    mem->stack[mem->SP] = val;
    mem->SP++;
}

/* Esegue una pop dallo stack, e restituisce il valore "poppato" */
int popValue(Memory mem) {
    if(mem->SP <= 0){
        vmError(mem, EXIT_CODE_STACK_UNDERFLOW);
    }
    
    mem->SP--;
    return mem->stack[mem->SP]; 
}

/* INIZIO FUNZIONI PER ISTRUZIONI ASSEMBLY */

/* Riutilizzo della funzione "printRegister" per l'istruzione DISPLAY */
void DISPLAY(Memory mem, int reg) {
    if (reg == -1){
        int i;
        for (i = 0; i<32; i++){         /* La variabile i consiste nel numero dei registri */
            printRegister(mem, i);
        }
    }else{
        printRegister(mem, reg);
    }
}

/* Stampa gli ultimi n posizioni dello stack */
void PRINT_STACK(Memory mem, int n) {
    
    unsigned int i;
    for (i = mem->SP - 1; i >= mem->SP - n; i--){         /* Stampa in ordine INVERSO */
        printf("STACK[%d] = %d\n", i, mem->stack[i]);     /* Stampo l’indice della posizione ed il valore in essa contenuto */
    }
    
}


/* Esegue una push nello stack, del valore contenuto nel registro reg */
void PUSH(Memory mem, int reg) { 
    
    pushValue(mem, getRegisterValue(mem, reg));
    
}

/* Esegue una pop dallo stack e inserisce il valore nel registro reg */
void POP(Memory mem, int reg) {
    
    setRegisterValue(mem, reg, popValue(mem)); 
    
}

void MOV(Memory mem, int reg, int val) {
    
    setRegisterValue(mem, reg, val);
    
}

void JMP(Memory mem, int newIP) {
    
    mem->IP = newIP;

}

void CALL(Memory mem, int newIP) {
    
    pushValue(mem, mem->IP);
    JMP(mem, newIP);
    
}

void RET(Memory mem){
    
    mem->IP = popValue(mem);

}

void JZ(Memory mem, int newIP, int reg) {
    
    if (getRegisterValue(mem, reg) == 0){
        mem->IP = newIP;
    }
    
}

void JPOS(Memory mem, int newIP, int reg) {
    
    if (getRegisterValue(mem, reg) > 0){
        mem->IP = newIP;
    }
    
}

void JNEG(Memory mem, int newIP, int reg) {
    
    if (getRegisterValue(mem, reg) < 0){
        mem->IP = newIP;
    }
    
}


void ADD(Memory mem, int reg1, int reg2, int reg3) {
    
    int val1, val2, result;
    val1 = getRegisterValue(mem, reg1);
    val2 = getRegisterValue(mem, reg2);
    
    if(!safeSum(val1, val2, &result)) {
        vmError(mem, EXIT_CODE_ARITHMETIC_OVERFLOW);
    }
    
    setRegisterValue(mem, reg3, result);
}

void SUB(Memory mem, int reg1, int reg2, int reg3) {
    
    int val1, val2 ,result;
    val1 = getRegisterValue(mem, reg1);
    val2 = getRegisterValue(mem, reg2);

    
    if (!safeSub(val1, val2, &result)){
        vmError(mem, EXIT_CODE_ARITHMETIC_OVERFLOW);
    }
    
    setRegisterValue(mem, reg3, result);
    
}

void MUL(Memory mem, int reg1, int reg2, int reg3) {
    
    int val1, val2, result;
    val1 = getRegisterValue(mem, reg1);               
    val2 = getRegisterValue(mem, reg2);

    if (!safeMul(val1, val2, &result)) { 
        vmError(mem, EXIT_CODE_ARITHMETIC_OVERFLOW);
    }
    
    setRegisterValue(mem, reg3, result);
    
}

void DIV(Memory mem, int reg1, int reg2, int reg3){
    
    int val1, val2;
    val1 = getRegisterValue(mem, reg1);               
    val2 = getRegisterValue(mem, reg2);
    
    if(val2 == 0){
        vmError(mem, EXIT_CODE_DIVISION_BY_ZERO);
    }

    if (val1 == INT_MIN && val2 == -1)  {
        vmError(mem, EXIT_CODE_ARITHMETIC_OVERFLOW);
    }
    
    setRegisterValue(mem, reg3, val1 / val2);
    
}

/* 
Funzione principale che esegue un programma. I paramentri prog e progsize
sono rispettivamente l'array contenente le istruzioni e i loro parametri, e la dimensione 
di tale array. 
Per funzionare viene creata un'istanza della struttura memory nell'heap che contiene
i registri, i registri speciali, e lo stack che il programma andrà ad utilizzare.
Il programma parte con un Instruction Pointer inizialiazzato a 0 e termina quando:
    -o l'IP va oltre la progsize,
    -oppure il flag halt viene settato a 1 (dall'istruzione HALT);
Ad ogni giro viene presa l'istruzione da eseguire e i suoi rispettivi parametri
utilizzando la funzione params().
In base al tipo di istruzione viene chiamata la funzione corretta. In caso di
istruzione non riconosciuta, il programma termina con un errore.
In ogni caso al termine del programma, viene liberata dall'heap la memoria creata. 
*/
void esegui(int* prog, unsigned int progsize) {
    
    int halt;   /* Sfrutto un flag per terminare il programma nel caso dell'istruzione HALT */
    Memory mem = createMemory();
    halt = 0;   

    while(mem->IP < progsize && !halt) {
        
        int param1, param2, param3;         /* Puntatori ai parametri delle varie istruzioni */
        int type = prog[mem->IP];                /* La variabile type corrisponde al numero del codice macchina */
        
        mem->IP = params(prog, mem->IP, &param1, &param2, &param3);  /* Richiamo della funzione params */
        
        
        switch (type){                                               /* Assegnazione dei nomi delle istruzioni e gestione del numero di paremetri */
            case 0:                                       
                halt = 1;
                break;
            case 1:
                DISPLAY(mem, param1);
                break;
            case 2:
                PRINT_STACK(mem, param1);
                break;
            case 10:
                PUSH(mem, param1);
                break;
            case 11:
                POP(mem, param1);
                break;
            case 12:
                MOV(mem, param1, param2);
                break;
            case 20:
                CALL(mem, param1);
                break;
            case 21:
                RET(mem);
                break;
            case 22:
                JMP(mem, param1);
                break;
            case 73:
                JZ(mem, param1, param2);
                break;
            case 74:
                JPOS(mem, param1, param2);
                break;
            case 75:
                JNEG(mem, param1, param2);
                break;
            case 80:
                ADD(mem, param1, param2, param3);
                break;
            case 81:
                SUB(mem, param1, param2, param3);
                break;
            case 82:
                MUL(mem, param1, param2, param3);
                break;
            case 83:
                DIV(mem, param1, param2, param3);
                break;
            default:
                vmError(mem, EXIT_CODE_UNKNOWN_INSTRUCTION);
        }
    }
    
    freeMemory(mem);
    
}
