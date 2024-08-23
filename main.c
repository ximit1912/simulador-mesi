#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/* ********************************************************************* */
/* ********************************************************************* */
/* Definições */


#define RAM_TAM 50
#define BLOCO_TAM 2
#define CACHE_TAM 5
#define CPU_QNT 3

typedef enum { RWITM, MODIFIED, EXCLUSIVE, SHARED, INVALID } MESIState;
// ESTADOS PROTOCOLO MESI (RWITM: Read With Intent To Modifie) 



/* ********************************************************************* */
/* ********************************************************************* */
/* Estrutura das Caches */


typedef struct
// CONTEÚDO DA TAG DAS CACHES 
{
    int pos;    // será obtido pela função obterBloco
    MESIState estado;
} Tag;

typedef struct
// CONTEÚDO DO BLOCO (2 Endereços da RAM)
{
    int dado[BLOCO_TAM];
} Bloco;

typedef struct
// CONTEÚDO LINHA DAS CACHES 
{   Tag tag;    
    Bloco bloco;
} cacheLinha;

typedef struct 
// CONTEÚDO DAS CACHES 
{
    cacheLinha linhas[CACHE_TAM];
    int fifoIndice; // para o algoritmo de substituição FIFO
} Cache;


/* ===================================================================== */
/* Estrutura da RAM */


typedef struct 
/* CONTEÚDO DA RAM */
{    
    int dado[RAM_TAM];
} Ram;


/* ********************************************************************* */
/* ********************************************************************* */
/* Inicialização das Caches e RAM */


Ram RAM;
Cache CACHE[CPU_QNT];


/* ********************************************************************* */
/* ********************************************************************* */
/* Funções */






/* ********************************************************************* */
/* ********************************************************************* */
/* Código main */

void main()
{
    printf("\nBEM VINDO AO SIMULADOR DE MEMORIA CACHE GERENCIADA POR PROTOCOLO MESI");



}