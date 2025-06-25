#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

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
        // termina
        printf("|%d|-----", tempo);
        p = desenfileira(fila);
        tempo += p.tempo;
        // processo
        printf("%c-----", p.processo);
    }
    printf("|%d|\n", tempo);
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

FilaDinamica roundRobin(Processo *vetor, int tamanho, int quantum, float *TME, float *TMR)
{
    FilaDinamica fila;
    iniciaFila(&fila);
    Processo empilha;
    int endTime[tamanho];
    int arrivalTime[tamanho];
    int cpuTime[tamanho];
    // definindo cada arrivaltime/cpuTime do processo para calcular TME/TMR
    for (int i = 0; i < tamanho; i++)
    {
        arrivalTime[i] = vetor[i].chegada;
        cpuTime[i] = vetor[i].tempo;
    }
    int contador = 0;
    int tempo = 0;
    int encontrou = 0;
    int indice;
    //while até todos os precossos acabarem(verifica se tempo = 0)
    while (ProcessoExiste(vetor, tamanho))
    {
        encontrou = 0;
        for (int i = 0; i < tamanho; i++)
        {
            //encontra o processo a ser executado e salva indice
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
            // verifica se posso tirar 5 do tempo
            if (vetor[indice].tempo > quantum)
            {
                vetor[indice].tempo -= quantum;
                empilha.tempo = 5;
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
            vetor[indice].chegada = tempo;
            enfileira(&fila, empilha);

            if (encontrou)
            {
                endTime[indice] = tempo;
            }
        }
        else
        {
            // caso nao encontre, procure o proximo processo que começa
            contador++;
        }
    }

    // calculando tempoEspera/tempoResposta
    int somadorE = 0;
    int somadorR = 0;
    for (int i = 0; i < tamanho; i++)
    {
        somadorE += (endTime[i] - arrivalTime[i]) - cpuTime[i];
        printf("%d = (%d - %d) - %d\n", somadorE, endTime[i], arrivalTime[i], cpuTime[i]);
        somadorR += endTime[i] - arrivalTime[i];
    }

    *TME = (float)somadorE / tamanho;
    *TMR = (float)somadorR / tamanho;

    // TMEA = (processo final - arrival time) - cpu time
    return fila;
}

FilaDinamica STF(Processo *vetor, int tamanho, float *TME, float *TMR)
{
    FilaDinamica fila;
    iniciaFila(&fila);
    Processo empilha;
    Processo *antigo = NULL;
    int arrivalTime[tamanho];
    int cpuTime[tamanho];
    int somaCpuTime = 0;
    int endTime[tamanho];
    //contador considerado como arrivalTime
    int contador = 0;
    int tempo = 0;
    // definindo cada arrivaltime da
    for (int i = 0; i < tamanho; i++)
    {
        arrivalTime[i] = vetor[i].chegada;
        cpuTime[i] = vetor[i].tempo;
        somaCpuTime += cpuTime[i];
    }

    while (ProcessoExiste(vetor, tamanho))
    {
        //reseta se chegar no fim, e so finaliza se nenhum processo existir
        if(contador == somaCpuTime)
        {
            contador = 0;
        }


        for (int i = 0; i < tamanho; i++)
        {
            //verificar Tempo do primeiro processo e verificar todos os arrivalTime até esse processo acabar
            //dependendo do arrivalTime, e se o tempo for menor, esse novo processo toma prioridade
            if (contador == vetor[i].chegada && vetor[i].tempo != 0)
            {
                if(antigo != NULL)
                {
                    // fazer verificacao de prioridade
                    // fazer a operacao de adicionar na fila
                    // pois já existe um antigo e consigo verificar qual deles é maior
                    if(antigo->tempo-contador > vetor[i].tempo)
                    {
                        //parando no arrivalTime
                        //se condicao verdadeira, então vetor[i] assume prioridade
                        antigo->tempo -= contador;
                        empilha.tempo = contador;
                        tempo+= empilha.tempo;
                        empilha.processo = antigo->processo; 
                        enfileira(&fila, empilha);
                    } else {
                        //se o outro nao é menor entao o que é empilhado é o proprio antigo
                        empilha.tempo = antigo->tempo;
                        empilha.processo = antigo->processo;
                        tempo+= empilha.tempo;
                        antigo->tempo = 0;
                        enfileira(&fila, empilha);
                    }

                }else {
                    antigo = &vetor[i];
                }

                if(antigo->tempo == 0)
                {
                    antigo = NULL;
                    //endTime[]
                } else {
                    antigo = &vetor[i];
                }
            }
        }
        
        contador++;
    }
    return fila;
}

FilaDinamica STRF(Processo *vetor, int tamanho, float *TME, float *TMR) {
    FilaDinamica filaSaida;
    iniciaFila(&filaSaida);
    int arrivalTime[tamanho];
    int cpuTime[tamanho];
    int endTime[tamanho];
    int indice = 0;

    // Certifique-se de que cada processo tem um 'tempoRestante' inicializado com seu 'tempo' original
    for (int i = 0; i < tamanho; i++) {
        // Assumindo que 'tempo' é o tempo original necessário para o processo
        // e que você o modifica durante a execução
        arrivalTime[i] = vetor[i].chegada;
        cpuTime[i] = vetor[i].tempo;
    }

    int tempoAtual = 0;
    Processo *processoAtualNaCPU = NULL; // O processo que está executando no momento
    int processosFinalizados = 0;

    // Loop principal: Continua enquanto ainda houver processos que não terminaram
    while (processosFinalizados < tamanho) {

        Processo *melhorCandidato = NULL;
        int menorTempoRestante = INT_MAX;//definimos o maximo de um inteiro
        //para que quando ele verifique um menor sempre vai ser o primeiro a ser lido

        // 1. Encontrar o processo de menor tempo restante entre os que já chegaram
        for (int i = 0; i < tamanho; i++) {
            if (vetor[i].chegada <= tempoAtual && vetor[i].tempo > 0) {
                if (vetor[i].tempo < menorTempoRestante) {
                    menorTempoRestante = vetor[i].tempo;
                    melhorCandidato = &vetor[i];
                    indice = i;
                }
                // Critério de desempate: se os tempos restantes são iguais, priorize o que chegou primeiro
                else if (vetor[i].tempo == menorTempoRestante) {
                    // CUIDADO: melhorCandidato pode ser NULL aqui se for o primeiro processo a ser encontrado com menorTempoRestante
                    // Certifique-se de que melhorCandidato não é NULL antes de acessar 'chegada'
                    if (melhorCandidato == NULL || vetor[i].chegada < melhorCandidato->chegada) {
                         melhorCandidato = &vetor[i];
                         indice = i;
                    }
                }
            }
        }

        // 2. Executar o processo escolhido (ou CPU ociosa)
        if (melhorCandidato != NULL) {
            // Se o processo atual na CPU mudou (houve preempção ou um processo anterior terminou)
            if (processoAtualNaCPU != melhorCandidato) {
                // Se houve uma mudança de processo, resetamos o processoAtualNaCPU
                // para o novo melhorCandidato. A lógica de enfileirar vai lidar com isso.
            }
            
            processoAtualNaCPU = melhorCandidato; // Define o processo atual da CPU
            processoAtualNaCPU->tempo--; // Decrementa o tempo restante do processo que está executando

            // Lógica para empilhar de forma consolidada
            Processo fatiaExecutada;
            fatiaExecutada.processo = processoAtualNaCPU->processo;
            fatiaExecutada.tempo = 1; // Por enquanto, é 1 unidade de tempo

            // Verifica se a fila de saída está vazia OU se o último processo é diferente
            if (filaSaida.fim == NULL || filaSaida.fim->x.processo != fatiaExecutada.processo) {
                // Se a fila está vazia ou é um novo processo, enfileira um novo nó
                enfileira(&filaSaida, fatiaExecutada);
            } else {
                // Se o último processo é o mesmo, apenas incrementa o tempo daquele último nó
                filaSaida.fim->x.tempo++;
            }
        } else {
            // Se nenhum processo está pronto para executar no tempoAtual (CPU ociosa)
            // Aqui você pode decidir o que fazer:
            // 1. Não fazer nada (apenas avança o contador)
            // 2. Empilhar um "processo ocioso" para representar o tempo sem atividade.
            // Para sua saída, talvez não seja necessário, mas é bom estar ciente.
        }
        tempoAtual++; // AVANÇA O RELÓGIO DO SISTEMA

        // 3. Verificar se o processo atual terminou
        if (processoAtualNaCPU != NULL && processoAtualNaCPU->tempo == 0) {
            processosFinalizados++;
            //defino o endtime de cada processo
            endTime[indice] = tempoAtual;
        }

    }

    // Calcular TME e TMR médios finais

    // calculando tempoEspera/tempoResposta
    int somadorE = 0;
    int somadorR = 0;
    for (int i = 0; i < tamanho; i++)
    {
        somadorE += (endTime[i] - arrivalTime[i]) - cpuTime[i];
        printf("%d = (%d - %d) - %d\n", somadorE, endTime[i], arrivalTime[i], cpuTime[i]);
        somadorR += endTime[i] - arrivalTime[i];
    }

    *TME = (float)somadorE / tamanho;
    *TMR = (float)somadorR / tamanho;


    return filaSaida;
}
int main()
{
    int tamanho;
    float TME, TMR;
    Processo *vetor = lerArquivo(&tamanho);
    Processo *copia = vetor;

    //FilaDinamica RoundRobin = roundRobin(copia, tamanho, 5, &TME, &TMR);
    FilaDinamica fon = roundRobin(copia, tamanho,5, &TME, &TMR);
    printf("tamanho = %d\n\n", tamanho);
    for (int i = 0; i < tamanho; i++)
    {
        printf("Processos[%d]:%c, %d, %d, %d\n", i, vetor[i].processo, vetor[i].tempo, vetor[i].chegada, vetor[i].prioridade);
    }

    imprimeFila(&fon);

    printf("TME: %.2f\nTMR: %.2f\n", TME, TMR);
    desempilhaApresentando(&fon);
}
