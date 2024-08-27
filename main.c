#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


/* ********************************************************************* */
/* ********************************************************************* */
/* Definições */


#define RAM_TAM 50
#define BLOCO_TAM 2
#define CACHE_TAM 5
#define CPU_QNT 3

// ESTADOS PROTOCOLO MESI (R: RWITM - Read With Intent To Modifie) 
char MESI[5] = {'R','M','E','S','I'};



typedef struct 
{
    int id;       
}tPacote;


/* ********************************************************************* */
/* ********************************************************************* */
/* Estrutura das Caches */


typedef struct
// CONTEÚDO DA TAG DAS CACHES 
{
    int posBloco;   // n° do bloco na RAM, obtido pela função obterBloco
    char estado;    // RWITM, M, E, S, I
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
    cacheLinha linha[CACHE_TAM];
    int *fifoIndice; // para o algoritmo de substituição FIFO
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


int obterBloco(int endereco) 
// Calcula o bloco da RAM com base no endereço
{
    return endereco / BLOCO_TAM; 
}

int obterOffset(int endereco) 
// Calcula o offset dentro do bloco
{
    return endereco % BLOCO_TAM; 
}



void initRAM(Ram *ram) 
// inicializa ram
{
    srand(time(NULL));
    for (int i = 0; i < RAM_TAM; i++) {
        ram->dado[i] = rand() % 1000; // Valores aleatórios
    }
}

void initCache(Cache *cache) 
// inicializa cache
{
    for (int i = 0; i < CACHE_TAM; i++) {
        cache->linha[i].bloco.dado[0] = -1; // 
        cache->linha[i].bloco.dado[1] = -1; // Indicando que a linha está vazia
        cache->linha[i].tag.estado = 'I';
        cache->linha[i].tag.posBloco = -1;
    }
    
    cache->fifoIndice = (int *) calloc (5, sizeof(int)); 
}

// MOSTRA CONTEÚDO DA RAM (AUXILIAR)
void auxConteudoRAM()
{
    for(int i = 0; i < RAM_TAM; i++)
        printf("\npos[%d] - %d", i, RAM.dado[i]);
}

// MOSTRA CONTEÚDO DAS CACHES (AUXILIAR)
void auxConteudoCache(int n)
{
    int i;

    printf("\n\nCACHE %d:", n);
    for(i = 0; i < CACHE_TAM; i++)
    {
        printf("\n[%d] | tag[bloco : %d, estado: %c] | dado: %d|", i, CACHE[n-1].linha[i].tag.posBloco, CACHE[n-1].linha[i].tag.estado, CACHE[n-1].linha[i].bloco.dado[0]);
        printf("\n    |                            | dado: %d|\n", CACHE[n-1].linha[i].bloco.dado[1]);
    }
}


/* ********************************************************************* */
/* ********************************************************************* */
/* Código main */

void main()
{
    printf("\nBEM VINDO AO SIMULADOR DE MEMORIA CACHE GERENCIADA POR PROTOCOLO MESI");

    // INICIALIZA A RAM E AS 3 CACHES 
    initRAM(&RAM);
    initCache(&CACHE[0]);
    initCache(&CACHE[1]);
    initCache(&CACHE[2]); 

    // INICIALIZAÇÃO DAS THREADS e seus pacotes (por enquanto só ID, talvez depois só tbm)
    pthread_t thread[CPU_QNT];
    int iret[CPU_QNT];
    tPacote t[CPU_QNT];
    
    for(int i=0; i<CPU_QNT; i++)
    {
        t[i].id = i+1;
    }

    // Auxiliares para verificação da RAM e CACHEs
    auxConteudoRAM();
    auxConteudoCache(1);
    auxConteudoCache(2);
    auxConteudoCache(3);

    char opcao;

    printf("\n\n***********************************************************");
    printf  ("\n*******             Escolha um <#CPU#>              *******");
    printf("\n\n******                <1>  <2>  <3>                  ******");
    printf("\n\n***********************************************************");
    printf("\n\n******   Ou tecle qualquer outra coisa para <SAIR>   ******");
    printf("\n\n*******                    <?>                      *******");
    printf("\n\n***********************************************************");
    scanf("%d", &opcao);



    switch (opcao)
    {
    case '1':
        
        break;
    

    case '2':

        break;


    case '3':


        break;


    default:
        break;
    }

}