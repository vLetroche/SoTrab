
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

#define QUANTUM 5

typedef struct NoFila *PtrNoFila;

typedef struct
{
    char processo;
    int tempo;
    float chegada;
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
        aux->proximo = fila->fim->proximo;
        fila->fim->proximo = aux;
        fila->fim = fila->fim->proximo;
        fila->tamanho++;
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

void imprimeFila(FilaDinamica *fila, FILE *arq)
{
    fprintf(arq, "Fila = { ");
    PtrNoFila ptr;
    for (ptr = fila->inicio; ptr != NULL; ptr = ptr->proximo)
    {
        fprintf(arq, "%c ", ptr->x.processo);
    }
    fprintf(arq, "}\n");
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
        exit(1);
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
    int aux;
    char label[20];
    for (int i = 1; i <= 4; i++)
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
                fscanf(arq, " %d", &aux);
                printf("%d\n", aux);
                vet[j].chegada = aux;
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

void desempilhaApresentando(FilaDinamica *fila, FILE *arq)
{
    int aux = fila->tamanho;
    Processo p;
    int tempo = 0;

    for (int i = 0; i < aux; i++)
    {
        // termina
        fprintf(arq, "|%d|-----", tempo);
        p = desenfileira(fila);
        tempo += p.tempo;
        // processo
        fprintf(arq, "%c-----", p.processo);
    }
    fprintf(arq, "|%d|\n", tempo);
}

// retorna 1 para sim 0 nao
int ProcessoExiste(Processo *vetor, int tamanho)
{
    for (int i = 0; i < tamanho; i++)
    {
        if (vetor[i].tempo != 0)
        {
            return 1;
        }
    }
    return 0;
}


FilaDinamica roundRobin(Processo *vetor, int tamanho, int *endTime)
{
    FilaDinamica fila;
    iniciaFila(&fila);
    Processo empilha;

    float contador = 0;
    int tempo = 0;
    int encontrou = 0;
    int indice;
    // while até todos os precossos acabarem(verifica se tempo = 0)
    while (ProcessoExiste(vetor, tamanho))
    {
        encontrou = 0;
        for (int i = 0; i < tamanho; i++)
        {
            // encontra o processo a ser executado e salva indice
            if (contador == vetor[i].chegada && vetor[i].tempo != 0)
            {
                empilha = vetor[i];
                encontrou = 1;
                indice = i;
                break;
            }
        }

        // empilha que eu tenho que empilhar
        // porem só empilho 5;
        // so enfileira depois de achar, se nao soma ++contador
        if (encontrou)
        {
            encontrou = 0;
            // verifica se posso tirar QUANTUM do tempo
            if (vetor[indice].tempo > QUANTUM)
            {
                vetor[indice].tempo -= QUANTUM;
                empilha.tempo = QUANTUM;
            }
            else
            {
                empilha.tempo = vetor[indice].tempo;
                vetor[indice].tempo = 0;
                // hora que acaba processo
                encontrou = 1;
            }
            // defino as variaveis novas para enfileirar na fila
            tempo += empilha.tempo;

            empilha.chegada = tempo;
            vetor[indice].chegada = tempo + 0.5;
            enfileira(&fila, empilha);

            if (encontrou)
            {
                endTime[indice] = tempo;
            }
        }
        else
        {
            // caso nao encontre, procure o proximo processo que começa
            contador+= 0.5;
        }
    }
    // TMEA = (processo final - arrival time) - cpu time
    return fila;
}

FilaDinamica STF(Processo *vetor, int tamanho, int *endTime)
{
    FilaDinamica filaSaida;
    iniciaFila(&filaSaida);

    // aloco memoria para os vetores

    int indice = 0;
    int tempoAtual = 0;
    Processo *processoAtualNaCPU = NULL; // O processo que está executando no momento

    // Continua enquanto ainda houver processos que não terminaram
    while (ProcessoExiste(vetor, tamanho))
    {

        Processo *melhorCandidato = NULL;
        int menorTempoRestante = INT_MAX; // definimos o maximo de um inteiro
        // para que quando ele verifique um menor sempre vai ser o primeiro a ser lido

        // encontrar o processo de menor tempo restante entre os que já chegaram
        for (int i = 0; i < tamanho; i++)
        {
            if (vetor[i].chegada <= tempoAtual && vetor[i].tempo > 0)
            {
                if (vetor[i].tempo < menorTempoRestante)
                {
                    menorTempoRestante = vetor[i].tempo;
                    melhorCandidato = &vetor[i];
                    indice = i;
                }
                // se os tempos restantes são iguais, priorize o que chegou primeiro
                else if (vetor[i].tempo == menorTempoRestante)
                {

                    if (melhorCandidato == NULL || vetor[i].chegada < melhorCandidato->chegada)
                    {
                        melhorCandidato = &vetor[i];
                        indice = i;
                    }
                }
            }
        }

        // Executar o processo escolhido
        if (melhorCandidato != NULL)
        {

            processoAtualNaCPU = melhorCandidato; // Define o processo atual da CPU
            processoAtualNaCPU->tempo--;          // Decrementa o tempo restante do processo que está executando

            // Lógica para empilhar
            Processo fatiaExecutada;
            fatiaExecutada.processo = processoAtualNaCPU->processo;
            fatiaExecutada.tempo = 1; // Por enquanto, é 1 unidade de tempo

            // Verifica se a fila de saída está vazia ou se o último processo é diferente
            if (filaSaida.fim == NULL || filaSaida.fim->x.processo != fatiaExecutada.processo)
            {
                // Se a fila está vazia ou é um novo processo, enfileira um novo nó
                enfileira(&filaSaida, fatiaExecutada);
            }
            else
            {
                // Se o último processo é o mesmo, incrementa o tempo daquele último nó
                filaSaida.fim->x.tempo++;
            }
        }

        tempoAtual++;

        // processo atual terminou
        if (processoAtualNaCPU != NULL && processoAtualNaCPU->tempo == 0)
        {
            // defino o endtime de cada processo
            endTime[indice] = tempoAtual;
        }
    }

    return filaSaida;
}

void geraArquivo(FilaDinamica *fila, Processo *p, int tamanhoP, int *endTime, char *arquivo)
{
    // criando arquivo
    FILE *arq = fopen(arquivo, "w");

    // Processos na Fila do First Come First Served:
    // A    B    C    D    E    F    G    H    I    J
    fprintf(arq, "Processos na Fila do First Come First Served:\n");
    for (int i = 0; i < tamanhoP; i++)
    {
        fprintf(arq, "%c\t", p[i].processo);
    }
    fprintf(arq, "\n\n\n");

    fprintf(arq, "Tempo de CPU requerida pelos processos:\n");
    for (int i = 0; i < tamanhoP; i++)
    {
        fprintf(arq, "%d\t", p[i].tempo);
    }
    fprintf(arq, "\n\n\n");

    fprintf(arq, "Tempo de Chegada dos processos:\n");
    for (int i = 0; i < tamanhoP; i++)
    {
        fprintf(arq, "%d\t", (int)p[i].chegada);
    }
    fprintf(arq, "\n\n\n");

    imprimeFila(fila, arq);

    fprintf(arq, "LINHA DO TEMPO\n");

    desempilhaApresentando(fila, arq);
    fprintf(arq, "\n\n");

    int somadorE = 0;
    int aux;
    fprintf(arq, "Tempo de Espera de cada processo:\n");
    for (int i = 0; i < tamanhoP; i++)
    {
        fprintf(arq, "%c\t", p[i].processo);
    }
    fprintf(arq, "\n");

    for (int i = 0; i < tamanhoP; i++)
    {
        aux = (endTime[i] - p[i].chegada) - p[i].tempo;
        fprintf(arq, "%d\t", aux);
        somadorE += aux;
    }
    fprintf(arq, "\n\n\n");
    float TME = (float)somadorE / tamanhoP;

    int somadorR = 0;
    fprintf(arq, "Tempo de Resposta de cada processo:\n");
    for (int i = 0; i < tamanhoP; i++)
    {
        fprintf(arq, "%c\t", p[i].processo);
    }
    fprintf(arq, "\n");
    for (int i = 0; i < tamanhoP; i++)
    {
        aux = endTime[i] - p[i].chegada;
        fprintf(arq, "%d\t", aux);
        somadorR += aux;
    }

    fprintf(arq, "\n\n\n");

    float TMR = (float)somadorR / tamanhoP;

    fprintf(arq, "Tempo Médio de Resposta: %.2f\n\n", TMR);
    fprintf(arq, "\n\n");
    fprintf(arq, "Tempo Médio de Espera: %.2f\n\n", TME);
}
/*
Processo *lerArquivoSP(int *tamanho)
{
    Processo *vet = (Processo *)malloc(50 * sizeof(Processo)); // aloco 50 posicoes pq nao sei quantos processos vai ler
    // depois dou o realloc para o tamanho certo
    int tam = 0;
    char c;
    char label[30] = "";
    int indexL = 0;
    FILE *arq = fopen("arquivo.txt", "r");
    if (arq == NULL)
    {
        printf("Erro ao abrir arquivo");
        exit(1);
    }

    // lendo o tamanho;

    int aux = 0;
    while ((c = fgetc(arq)) != EOF)
    {
        if (aux == 0)
        {
            label[indexL] = c;
            indexL++;
            if (c == ' ')
            {
                //label[indexL] = '\0';
                aux = 1;
                continue;
            }
        } else if (strcasecmp(label, "Processo") == 0)
        {
            printf("%c", c);
            if (c == '\n')
            {
                break;
            }else if (c != ' ')
            {
                tam++;
            }
        }

        printf("%d - %s\n", indexL, label);

        

        // agora que temos definido o tamanho podemos simplesmente ler da outra maneira
    }

    vet = (Processo *)realloc(vet, tam * sizeof(Processo));
    for (int i = 1; i < 4; i++)
    {
        // ler a primeira strng
        fscanf(arq, "%s", label);
        for (int j = 0; j < tam; j++)
        {
            switch (i)
            {
            case 1:
                fscanf(arq, " %d", &vet[j].tempo);
                break;
            case 2:
                fscanf(arq, " %d", &vet[j].chegada);
                break;
            case 3:
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
*/
int main()
{
    int tamanho;
    int *endTime;
    Processo *vetor = lerArquivo(&tamanho);

    printf("tamanho = %d\n", tamanho);
    for (int i = 0; i < tamanho; i++)
    {
        printf("%c - %d - %d - %d\n", vetor[i].processo, vetor[i].tempo, (int)vetor[i].chegada, vetor[i].prioridade);
    }

    
    Processo *copia = malloc(tamanho * sizeof(Processo));
    if (copia == NULL)
    {
        printf("Erro ao alocar memória para copia\n");
        exit(1);
    }

    // Copiando os elementos do vetor original para a copia
    for (int i = 0; i < tamanho; i++)
    {
        copia[i] = vetor[i]; // Cópia membro a membro (shallow, mas ok se Processo não tiver ponteiros)
    }
    
    // inicializo as variaveis
    endTime = (int *)malloc(tamanho * sizeof(int));

    // FilaDinamica RoundRobin = roundRobin(copia, tamanho, 5, &TME, &TMR);
    FilaDinamica fila = STF(copia, tamanho, endTime);
    geraArquivo(&fila, vetor, tamanho, endTime, "STF.txt");
    
    // Copiando os elementos do vetor original para a copia
    for (int i = 0; i < tamanho; i++)
    {
        copia[i] = vetor[i]; // Cópia membro a membro (shallow, mas ok se Processo não tiver ponteiros)
    }

    fila = roundRobin(copia, tamanho, endTime);
    geraArquivo(&fila, vetor, tamanho, endTime, "RoundRobin.txt");
    // desempilhaApresentando(&fon);
    return 0;
}
