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

    /* outras variáveis se necessário */

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
    int *fifoFila,
        fifoQnt; // para o algoritmo de substituição FIFO
    int preenchida;
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
    
    cache->fifoFila = (int *) calloc (5, sizeof(int));
    cache->fifoQnt = 0;
    cache->preenchida = 0; 
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

void imprimeLinha(int linha, int idCache, int dado)
{
    printf(  "\n[%d] | tag[bloco : %d, estado: %c]  | ", linha, CACHE[idCache-1].linha[linha].tag.posBloco, CACHE[idCache-1].linha[linha].tag.estado);
                        if(!dado)
                        {
                            printf("dado: %d <--", CACHE[idCache-1].linha[linha].bloco.dado[0]);
                            printf("\n    |                            | dado: %d\n", CACHE[idCache-1].linha[linha].bloco.dado[1]);
                        }
                        else
                            {
                                printf("dado: %d    ", CACHE[idCache-1].linha[linha].bloco.dado[0]);
                                printf("\n    |                           | dado: %d <--\n", CACHE[idCache-1].linha[linha].bloco.dado[1]);
                            }   
}

/* 
VERIFICA NAS OUTRAS CACHES SE HÁ CÓPIAS
    1- RETORNA 0 SE NÃO 
    2- RETORNA 1 SE HÁ UMA CÓPIA (E ou M), 2 SE HÁ 2 CÓPIAS (S e S)
        -ATRIBUI A LINHA DE CACHE EM QUE ELA(s) SE ENCONTRA(m) [0 - 4] NA POSICAO idCache-1 DO VETOR indiceCopias[idCache-1]
*/
int procuraCopias(int idCache, int *indCopias, int bloco)
{
    int copias = 0;

    switch (idCache)
    {
    // verifica nas cache 2 e 3
    case 1:
        for (int i = 0; i < CACHE_TAM; i++)
        {
            if(CACHE[1].linha[i].tag.posBloco == bloco)
            {
                copias++;           // aumenta qnt de copias encontradas (max. 2) 
                indCopias[1] = i;   // indica na posicao 1 do vetor == cache 2, qual linha da cache possui uma cópia do bloco 

                break;
            }
            
        }

        for (int i = 0; i < CACHE_TAM; i++)
        {
            if(CACHE[2].linha[i].tag.posBloco == bloco)
            {
                copias++;           // aumenta qnt de copias encontradas (max. 2) 
                indCopias[2] = i;   // indica na posicao 2 do vetor == cache 3, qual linha da cache possui uma cópia do bloco 

                break;
            }
            
        }

        break;

    // verifica nas cache 1 e 3
    case 2:
        for (int i = 0; i < CACHE_TAM; i++)
        {
            if(CACHE[0].linha[i].tag.posBloco == bloco)
            {
                copias++;           // aumenta qnt de copias encontradas (max. 2) 
                indCopias[0] = i;   // indica na posicao 0 do vetor == cache 1, qual linha da cache possui uma cópia do bloco 

                break;
            }
            
        }

        for (int i = 0; i < CACHE_TAM; i++)
        {
            if(CACHE[2].linha[i].tag.posBloco == bloco)
            {
                copias++;           // aumenta qnt de copias encontradas (max. 2) 
                indCopias[2] = i;   // indica na posicao 2 do vetor == cache 3, qual linha da cache possui uma cópia do bloco 

                break;
            }
            
        }

        break;

    // verifica nas cache 1 e 2 
    case 3:
        for (int i = 0; i < CACHE_TAM; i++)
        {
            if(CACHE[0].linha[i].tag.posBloco == bloco)
            {
                copias++;           // aumenta qnt de copias encontradas (max. 2) 
                indCopias[0] = i;   // indica na posicao 0 do vetor == cache 1, qual linha da cache possui uma cópia do bloco 

                break;
            }
            
        }

        for (int i = 0; i < CACHE_TAM; i++)
        {
            if(CACHE[1].linha[i].tag.posBloco == bloco)
            {
                copias++;           // aumenta qnt de copias encontradas (max. 2) 
                indCopias[1] = i;   // indica na posicao 1 do vetor == cache 2, qual linha da cache possui uma cópia do bloco 

                break;
            }
            
        }

        break;
    }

    return copias;
}



/* 
LEITURA: 
1- VERIFICA SE EXISTE NA CACHE
    1a- SE SIM (READ HIT  - 0), APENAS MOSTRA O DADO (OU FAZ ALGUM PROCESSAMENTO)
    1b- SE NAO (READ MISS - 1)
        --VERIFICA SE EXISTEM CÓPIAS EM OUTRA CACHES
        --MOSTRA O DADO (OU FAZ ALGUM PROCESSAMENTO)
*/    
int leitura(int idCache, int enderecoRam)
{
    int bloco = obterBloco(enderecoRam),
        dado = obterOffset(enderecoRam),
        hit = -1;
    
        // verifica se o dado está na cache
        for (int i = 0; i < CACHE_TAM; i++)
        {
            // READ HIT - 0
            if(CACHE[idCache-1].linha[i].tag.posBloco == bloco  &&  CACHE[idCache-1].linha[i].tag.estado != 'I')
            {
                hit = i;
                printf("\n\nREAD HIT - O BLOCO ESTA NA CACHE: ");
                printf(  "\n[%d] | tag[bloco : %d, estado: %c] | ", i, bloco, CACHE[idCache-1].linha[i].tag.estado);
                if(!dado)
                {
                    printf("dado: %d <--|", CACHE[idCache-1].linha[i].bloco.dado[0]);
                    printf("\n    |                            | dado: %d    |\n", CACHE[idCache-1].linha[i].bloco.dado[1]);
                }
                else
                    {
                        printf("dado: %d    |", CACHE[idCache-1].linha[i].bloco.dado[0]);
                        printf("\n    |                            | dado: %d <--|\n", CACHE[idCache-1].linha[i].bloco.dado[1]);
                    }
                
                // retorna que foi RH
                return 0;
            }
            else if(CACHE[idCache-1].linha[i].tag.posBloco == bloco  &&  CACHE[idCache-1].linha[i].tag.estado == 'I')
                {
                    // ALGO SE O BLOCO ESTIVER INVALIDO, FUNÇÃO DE GERENCIAR PROVAVELMENTE
                    // return 1;
                }
        }
        
        // READ MISS - 1

        int indiceCopias[3],
            qntCopias = procuraCopias(idCache, indiceCopias, bloco),
            linha;

        switch (qntCopias)
        {
        // NAO HÁ CÓPIAS NAS OUTRAS CACHES, RETORNO IGUAL A 0
        case 0:
            // VER SE CACHE ESTÁ CHEIA COM A VARIAVEL PREENCHIDA. preenchida = 0: ATRIBUI A POSICAO ALEATORIA E VAZIA.
            if(CACHE[idCache-1].preenchida == 0)
            {
                srand(time(NULL));

                // mapeamento aleatório
                linha = rand() % 5;

                // se a linha aleatória tiver vazia mesmo
                if(CACHE[idCache-1].linha[linha].tag.posBloco == -1)
                {
                    CACHE[idCache-1].linha[linha].tag.posBloco = bloco;
                    CACHE[idCache-1].linha[linha].tag.estado = 'E';
                    CACHE[idCache-1].linha[linha].bloco.dado[0] = RAM.dado[bloco*2];
                    CACHE[idCache-1].linha[linha].bloco.dado[1] = RAM.dado[bloco*2 + 1];
                    CACHE[idCache-1].fifoFila[CACHE[idCache-1].fifoQnt] = linha;
                    CACHE[idCache-1].fifoQnt++;

                    printf("\n\nREAD MISS - O BLOCO NAO ESTA NA CACHE E FOI INSERIDO NA POSICAO [%d]: ", linha);

                    imprimeLinha(linha, idCache, dado);
                }
                // se a linha sorteada nao estar vazia, percorre ela até encontrar uma linha vazia
                else
                    {
                        do
                        {
                            if(linha == 4)
                                linha = 0;
                            else
                                linha++;
                        }while(CACHE[idCache-1].linha[linha].tag.posBloco != -1);

                        CACHE[idCache-1].linha[linha].tag.posBloco = bloco;
                        CACHE[idCache-1].linha[linha].tag.estado = 'E';
                        CACHE[idCache-1].linha[linha].bloco.dado[0] = RAM.dado[bloco*2];
                        CACHE[idCache-1].linha[linha].bloco.dado[1] = RAM.dado[bloco*2 + 1];
                        CACHE[idCache-1].fifoFila[CACHE[idCache-1].fifoQnt] = linha;
                        CACHE[idCache-1].fifoQnt++;

                        // verifica se preencheu a cache
                        if(CACHE[idCache-1].fifoQnt >= CACHE_TAM)
                            CACHE[idCache-1].preenchida = 1;

                        printf("\n\nREAD MISS - O BLOCO NAO ESTA NA CACHE E FOI INSERIDO NA POSICAO [%d]: ", linha);
                        imprimeLinha(linha, idCache, dado); 
                    }

            }
            //  preenchida = 1: ESTÁ PREENCHIDA ENTAO USA POLITICA DE SUBSTITUIÇÃO FIFO
            else
            // 
                {
                    // atribui a linha a ser substituida a que entrou por primeiro (First-In First-Out)
                    linha = CACHE[idCache-1].fifoFila[0];

                    // atualiza a fila
                    for (int i = 1; i < CACHE_TAM; i++)
                        CACHE[idCache-1].fifoFila[i - 1] = CACHE[idCache-1].fifoFila[i];
                    CACHE[idCache-1].fifoFila[4] = linha;
                    
                    // substitui a linha
                    CACHE[idCache-1].linha[linha].tag.posBloco = bloco;
                    CACHE[idCache-1].linha[linha].tag.estado = 'E';
                    CACHE[idCache-1].linha[linha].bloco.dado[0] = RAM.dado[bloco*2];
                    CACHE[idCache-1].linha[linha].bloco.dado[1] = RAM.dado[bloco*2 + 1];
                
                    printf("\n\nREAD MISS - O BLOCO NAO ESTA NA CACHE E FOI INSERIDO NA POSICAO [%d]: ", linha);
                        imprimeLinha(linha, idCache, dado);
                }

            break;
        
        case 1:

            break;

        case 2:

            break;
        }

}








// FUNÇÃO PRINCIPAL DAS THREADS (PROCESSADORES)
void *simula(void *ptr)
{
    tPacote *t = (void *) ptr;

    char op;
    int end, transacao;

    printf("\n\n###############################################");
    printf(  "\n############## PROCESSADOR <%d> ################", t->id);
    printf(  "\n## Voce deseja fazer uma leitura ou escrita? ##");
    printf(  "\n##               Leitura <1>                 ##");
    printf(  "\n##               Escrita <2>                 ##");
    printf(  "\n###############################################\nL/E: ");
    scanf(" %c", &op);

    switch (op)
    {
    case '1':
        printf("\n\n***** <1> LEITURA - SOLICITACAO DE DADO SEM ALTERACAO DO VALOR: *****");
        printf(  "\n -> Entre com o endereco da memoria principal desejado [0 <= end <= 49]: ");
        scanf("%d", &end);

        while(end < 0 || end > 49)
        {
            printf("\nOPCAO INVALIDA! INSIRA NOVAMENTE: ");
            scanf("%d", &end);
        }

        transacao = leitura(t->id, end);
            // void gerenciaCoerencia(int idCache, int transacao);

        break;
    
    case '2':
        printf("\n\n####### <2> ESCRITA - SOLICITACAO DE DADO COM ALTERACAO DO VALOR: #######");
        printf(  "\n## Entre com o endereco da memoria principal desejado [0 <= end <= 49]:  ");
        scanf("%d", &end);

        // transacao = int escrita(int idCache, int enderecoRam);
            // gerenciaCoerencia(int idCache, int transacao)

        break;
    
    // case para testes
    case '3':
        auxConteudoCache(1);
        auxConteudoCache(2);
        auxConteudoCache(3);

        break;

    default:
        printf("\nOPCAO INVALIDA! ABORTANDO ...");

        break;
    }


}









/* ********************************************************************* */
/* ********************************************************************* */
/* Código main */

void main()
{
    srand(time(NULL));

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
    printf("\n----------------------------------------------------------------\n");
    auxConteudoCache(1);
    printf("\n----------------------------------------------------------------\n");
    auxConteudoCache(2);
    printf("\n----------------------------------------------------------------\n");
    auxConteudoCache(3);
    printf("\n----------------------------------------------------------------\n");

    char opCPU;

    do
    {
        printf("\n\n***********************************************************");
        printf  ("\n*******             Escolha um <#CPU#>              *******");
        printf("\n\n******                <1>  <2>  <3>                  ******");
        printf("\n\n******                                               ******");
        printf("\n\n******            Ou tecle 0 para <SAIR>             ******");
        printf("\n\n*******                    <0>                      *******");
        printf("\n\n***********************************************************\nCPU: ");
        scanf(" %c", &opCPU);


        switch (opCPU)
        {
        case '1':
            iret[0] = pthread_create(&thread[0], NULL, simula, (void*) &t[0]);

            pthread_join(thread[0], NULL);
            break;
        

        case '2':
            iret[1] = pthread_create(&thread[1], NULL, simula, (void*) &t[1]);

            pthread_join(thread[1], NULL);
            break;


        case '3':
            iret[2] = pthread_create(&thread[2], NULL, simula, (void*) &t[2]);

            pthread_join(thread[2], NULL);
            break;

        case '0':
            printf("\nENCERRANDO PROGRAMA ...");

            break;

        default:
            printf("\nOPCAO INVALIDA!");

            break;
        }
    } while (opCPU != '0');
}