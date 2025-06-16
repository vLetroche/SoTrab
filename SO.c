#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct NoFila *PtrNoFila;

typedef struct
{
    char processo;
    int tempo;
    int chegada;
    int prioridade;
} Processo;

typedef struct NoFila
{
    Processo x;
    PtrNoFila proximo;
} NoFila;

typedef struct
{
    PtrNoFila inicio;
    PtrNoFila fim;
    int tamanho;
} FilaDinamica;

void iniciaFila(FilaDinamica *fila)
{
    fila->tamanho = 0;
    fila->inicio = NULL;
    fila->fim = NULL;
}

bool estaVazia(FilaDinamica *fila)
{
    return (fila->inicio == NULL);
}
int tamanhoFila(FilaDinamica *fila)
{
    return (fila->tamanho);
}
void enfileira(FilaDinamica *fila, Processo x)
{
    int help = 0;
    PtrNoFila aux;
    aux = (PtrNoFila)malloc(sizeof(NoFila));
    aux->x = x;

    if (estaVazia(fila))
    {
        fila->inicio = aux;
        fila->fim = aux;
        aux->proximo = NULL;
        fila->tamanho++;
    }
    else
    {
        PtrNoFila ptr;
        for (ptr = fila->inicio; ptr != NULL; ptr = ptr->proximo)
        {
            //verifica se já existe
            if (ptr->x.processo == x.processo)
            {
                help = 1;
            }
        }
        if (help == 0)
        {
            aux->proximo = fila->fim->proximo;
            fila->fim->proximo = aux;
            fila->fim = fila->fim->proximo;
            fila->tamanho++;
        }
        else
        {
            printf("Valor ja inserido\n");
        }
    }
}
Processo desenfileira(FilaDinamica *fila)
{
    Processo retorno;
    if (!estaVazia(fila))
    {
        PtrNoFila aux;
        aux = fila->inicio;
        fila->inicio = fila->inicio->proximo;
        retorno = aux->x;
        free(aux);
        fila->tamanho--;
    }
    else
    {
        printf("Warning: não removeu elementos pois a fila está vazia\n");
    }
    return retorno;
}

void imprimeFila(FilaDinamica *fila)
{
    printf("Fila = {");
    PtrNoFila ptr;
    for (ptr = fila->inicio; ptr != NULL; ptr = ptr->proximo)
    {
        printf("%c ", ptr->x.processo);
    }
    printf("}\n");
}

void destroiFila(FilaDinamica *fila)
{
    int aux = fila->tamanho;
    for (int i = 0; i < aux; i++)
    {
        printf("%d\n", i);
        desenfileira(fila);
    }
    printf("a\n");
}

/*
Processo	  A	 B	 C	 D	 E	 F	 G	 H	 I	 J
Tempo	10	11	12	13	14	15	16	17	18	19
Chegada	0	1	2	3	4	5	6	7	8	9
Prioridade	 0	0	0	0	0	0	0	0	0	0
*/

Processo *lerArquivo(int *tamanho)
{
    Processo *vet;
    int tam;
    FILE *arq = fopen("arquivo.txt", "r");
    if (arq == NULL)
    {
        printf("Erro ao abrir arquivo");
    }

    // lendo o tamanho;
    fscanf(arq, "p=%d\n", &tam);
    if (tam == 0)
    {
        fclose(arq);
        *tamanho = tam;
        return NULL;
    }
    // linhas
    vet = (Processo *)malloc(tam * sizeof(Processo));
    if (vet == NULL)
    {
        printf("Erro ao malloc processos");
    }

    char label[20];
    for (int i = 1; i < 4; i++)
    {
        // ler a primeira strng
        fscanf(arq, "%s", label);
        for (int j = 0; j < tam; j++)
        {
            switch (i)
            {
            case 1:
                fscanf(arq, " %c", &vet[j].processo);
                break;
            case 2:
                fscanf(arq, " %d", &vet[j].tempo);
                break;
            case 3:
                fscanf(arq, " %d", &vet[j].chegada);
                break;
            case 4:
                fscanf(arq, " %d", &vet[j].prioridade);
                break;

            default:
                break;
            }
        }
        fscanf(arq, "\n");
    }
    fclose(arq);
    *tamanho = tam;
    return vet;
}

void desempilhaApresentando(FilaDinamica *fila)
{
    int aux = fila->tamanho;
    Processo p;
    int tempo = 0;

    for (int i = 0; i < aux; i++)
    {
        printf("|%d|-----", tempo);
        p = desenfileira(fila);
        tempo += p.tempo;
        printf("%c-----",p.processo);
    }
    printf("|%d|\n", tempo);
}

int main()
{
    int tamanho;
    Processo *vetor = lerArquivo(&tamanho);

    FilaDinamica f;
    iniciaFila(&f);
    
    
    printf("tamanho = %d\n\n", tamanho);
    for (int i = 0; i < tamanho; i++)
    {
        enfileira(&f,vetor[i]);
        printf("Processos[%d]:%c, %d, %d, %d\n", i, vetor[i].processo, vetor[i].tempo, vetor[i].chegada, vetor[i].prioridade);
    }
    imprimeFila(&f);

    desempilhaApresentando(&f);
}