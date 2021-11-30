#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <float.h>
#include <time.h>
#include <math.h>

typedef struct no {

    int id;
    double peso;
    struct no *proximo;
    
} No;

typedef struct grafo {

    int arestas;
    int vertices;
    No **adjacencias;

} Grafo;

typedef struct verticeCusto {

    int vertice;
    double custo;

} VerticeCusto;

typedef struct heapMinimo {

    int tamanho;
    int *posicoes;
    VerticeCusto *valores;

} HeapMinimo;

typedef struct ponto {
    
    double x;
    double y;

} Ponto;

//Funcoes para pontos
double calcularDistanciaPontos(Ponto p1, Ponto p2);

//Funcoes para grafos
int *buscaProfundidade(Grafo *agm, int vertice);
void destruirGrafo(Grafo *grafo);
void adicionarAresta(int v1, int v2, double peso, Grafo *grafo);
void buscaProfundidadeAuxiliar(Grafo *agm, int vertice, int ciclo[], int *itr, int visitados[]);
void exportarAGM(Grafo *agm, Ponto pontos[]);
void exportarCiclo(int ciclo[], Ponto *pontos, int tam);
void inicializarPrim(HeapMinimo *heapMinimo, double custos[], int prodecessores[], int tam);
void marcarNaoVisitados(int visitados[], int tam);
void imprimirCustoTotal(clock_t inicio, Ponto pontos[], int ciclo[], int tam);
void destruirEstruturas(int ciclo[], Ponto pontos[], Grafo *grafo, Grafo *agm);
double calcularCustoTotal(Ponto pontos[], int ciclo[], int tam);
Ponto *lerArquivo(char nomeArquivo[], int *tam);
Grafo *prim(Grafo *grafo, int vertice, Ponto pontos[]);
Grafo *criarGrafo(int tamanho);
Grafo *preencherGrafo(Ponto pontos[], int tam);

//Funcoes para HEAP minimo
int pai(int i);
int filhoEsquerda(int i);
int filhoDireita(int i);
int existe(HeapMinimo *heapMinimo, int vertice);
int vazio(HeapMinimo *heapMinimo);
void trocar(HeapMinimo *heapMinimo, int a, int b);
void atualizarHeapMinimo(HeapMinimo *heapMinimo, int i);
void construirHeapMinimo(HeapMinimo *heapMinimo);
void diminuirValorChave(HeapMinimo *heapMinimo, int i, double chave);
void destruirHeapMinimo(HeapMinimo *heapMinimo);
VerticeCusto extrairMinimo(HeapMinimo *heapMinimo);
HeapMinimo *criarHeapMinimo(int tam);

int main(int argc, char *argv[]){

    char *nomeArquivo = (argc > 1) ? argv[1] : "input.txt";
    int tam, *ciclo;
    clock_t inicio;
    Grafo *grafo, *agm;
    Ponto *pontos;
    
    inicio = clock();
    pontos = lerArquivo(nomeArquivo, &tam);
    grafo = preencherGrafo(pontos, tam);
    agm = prim(grafo, 0, pontos);
    ciclo = buscaProfundidade(agm, 0);

    exportarAGM(agm, pontos);
    exportarCiclo(ciclo, pontos, tam);
    imprimirCustoTotal(inicio, pontos, ciclo, agm->vertices);
    destruirEstruturas(ciclo, pontos, grafo, agm);

    return 0;

}

Ponto *lerArquivo(char nomeArquivo[], int *tam){

    FILE *arquivo;
    Ponto *pontos;

    arquivo = fopen(nomeArquivo, "r");

    if(arquivo == NULL){

        printf("ERRO AO ABRIR O ARQUIVO.");
        getchar();
        exit(1);

    }

    fscanf(arquivo, "%d", tam);

    pontos = (Ponto*) malloc ((*tam) * sizeof (Ponto));

    for(int i = 0; i < (*tam); i++)
        fscanf(arquivo, "%lf %lf", &pontos[i].x, &pontos[i].y);

    fclose(arquivo);

    return pontos;

}

Grafo *criarGrafo(int tamanho){

    Grafo *grafo = (Grafo *) malloc (sizeof(Grafo));

    grafo->arestas = 0;
    grafo->vertices = tamanho;
    grafo->adjacencias = (No **) malloc (tamanho * sizeof (No *));

    for(int i = 0; i < grafo->vertices; i++)
        grafo->adjacencias[i] = NULL;

    return grafo;

}

Grafo *preencherGrafo(Ponto pontos[], int tam){

    Grafo *grafo;

    grafo = criarGrafo(tam);
        
    for(int i = 0; i < grafo->vertices; i++)
        for(int j = 0; j < grafo->vertices; j++)
            if(i != j)
                adicionarAresta(i, j, calcularDistanciaPontos(pontos[i], pontos[j]), grafo);

    return grafo;

}

void destruirGrafo(Grafo *grafo){

    for(int i = 0; i < grafo->vertices; i++)
        for(No *aux = grafo->adjacencias[i]; aux != NULL; aux = aux->proximo)
            free(aux);

    free(grafo->adjacencias);
    free(grafo);

}

void adicionarAresta(int v1, int v2, double peso, Grafo *grafo){

    No *novo = (No *) malloc (sizeof(No));
    novo->id = v2;
    novo->peso = peso;

    if(grafo->adjacencias[v1] == NULL)
        novo->proximo = NULL;

    else {

        No *aux = grafo->adjacencias[v1];
        novo->proximo = aux;

    }

    grafo->adjacencias[v1] = novo;
    grafo->arestas++;

}

Grafo *prim(Grafo *grafo, int vertice, Ponto pontos[]){

    int prodecessores[grafo->vertices];
    double custos[grafo->vertices];
    HeapMinimo *heapMinimo;
    Grafo *agm;
    
    heapMinimo = criarHeapMinimo(grafo->vertices);
    agm = criarGrafo(grafo->vertices);    

    inicializarPrim(heapMinimo, custos, prodecessores, grafo->vertices);

    custos[0] = 0;
    construirHeapMinimo(heapMinimo);
    diminuirValorChave(heapMinimo, vertice, 0);

    while(!vazio(heapMinimo)){

        VerticeCusto u = extrairMinimo(heapMinimo);

        for(No *aux = grafo->adjacencias[u.vertice]; aux != NULL; aux = aux->proximo){

            if(existe(heapMinimo, aux->id) && aux->peso < custos[aux->id]){
                
                custos[aux->id] = aux->peso;
                prodecessores[aux->id] = u.vertice;
                diminuirValorChave(heapMinimo, heapMinimo->posicoes[aux->id], aux->peso);

            }

        }

    }
    
    for(int i = 1; i < grafo->vertices; i++){

        adicionarAresta(prodecessores[i], i, custos[i], agm);
        adicionarAresta(i, prodecessores[i], custos[i], agm);

    }

    destruirHeapMinimo(heapMinimo);

    return agm;

}

void inicializarPrim(HeapMinimo *heapMinimo, double custos[], int prodecessores[], int tam){

    for(int i = 0; i < tam; i++){

        custos[i] = DBL_MAX;
        heapMinimo->valores[i].vertice = i;
        heapMinimo->valores[i].custo = custos[i];
        heapMinimo->posicoes[i] = i;
        prodecessores[i] = -1;

    }

}

int *buscaProfundidade(Grafo *agm, int vertice){

    int *ciclo, itr = 1, visitados[agm->vertices];

    ciclo = malloc ((agm->vertices + 1) * sizeof (int));

    marcarNaoVisitados(visitados, agm->vertices);
    
    ciclo[0] = vertice;
    buscaProfundidadeAuxiliar(agm, vertice, ciclo, &itr, visitados);
    ciclo[agm->vertices] = vertice;
    
    return ciclo;

}

void buscaProfundidadeAuxiliar(Grafo *agm, int vertice, int ciclo[], int *itr, int visitados[]){

    visitados[vertice] = 1;

    for(No *aux = agm->adjacencias[vertice]; aux != NULL; aux = aux->proximo){

        if(visitados[aux->id] == 0){

            ciclo[(*itr)] = aux->id;
            (*itr)++;
            buscaProfundidadeAuxiliar(agm, aux->id, ciclo, itr, visitados);

        }
        
    }

}

double calcularCustoTotal(Ponto pontos[], int ciclo[], int tam){

    double custoTotal = 0;

    for(int i = 0; i < tam - 1; i++)
        custoTotal += calcularDistanciaPontos(pontos[ciclo[i]], pontos[ciclo[i + 1]]);

    custoTotal += calcularDistanciaPontos(pontos[ciclo[(tam - 1)]], pontos[ciclo[0]]);

    return custoTotal;

}

void exportarCiclo(int ciclo[], Ponto *pontos, int tam){

    FILE *arquivo;

    arquivo = fopen("cycle.txt", "w");

    if(arquivo == NULL){

        printf("ERRO AO ABRIR O ARQUIVO.");
        getchar();
        exit(1);

    }

    for(int i = 0; i <= tam; i++)
        fprintf(arquivo, "%d %d\n", (int) pontos[ciclo[i]].x, (int) pontos[ciclo[i]].y);   

    fclose(arquivo);
 
}

void exportarAGM(Grafo *agm, Ponto pontos[]){

    FILE *arquivo;

    arquivo = fopen("tree.txt", "w");

    if(arquivo == NULL){

        printf("ERRO AO ABRIR O ARQUIVO.");
        getchar();
        exit(1);

    }

    for(int i = 0; i < agm->vertices; i++){

        for(No *aux = agm->adjacencias[i]; aux != NULL; aux = aux->proximo)
            fprintf(arquivo, "%d %d\n%d %d\n", (int) pontos[i].x, (int) pontos[i].y, (int) pontos[aux->id].x, (int) pontos[aux->id].y);

    }

    fclose(arquivo);

}

double calcularDistanciaPontos(Ponto p1, Ponto p2){

    return sqrt(pow((p1.x - p2.x) , 2) + pow((p1.y - p2.y), 2));

}

int pai(int i){

    return (i - 1) / 2;

}

int filhoEsquerda(int i){

    return (i * 2) + 1;

}

int filhoDireita(int i){

    return (i * 2) + 2;

}

HeapMinimo *criarHeapMinimo(int tam){

    HeapMinimo *heapMinimo;

    heapMinimo = (HeapMinimo *) malloc (sizeof(HeapMinimo));
    heapMinimo->valores = (VerticeCusto *) malloc (tam * sizeof (VerticeCusto));
    heapMinimo->posicoes = (int *) malloc (tam * sizeof (int));
    heapMinimo->tamanho = tam;

    return heapMinimo;

}

void destruirHeapMinimo(HeapMinimo *heapMinimo){

    free(heapMinimo->valores);
    free(heapMinimo->posicoes);
    free(heapMinimo);

}

void atualizarHeapMinimo(HeapMinimo *heapMinimo, int i){

    int esquerda = filhoEsquerda(i);
    int direita = filhoDireita(i);
    int menor;

    if((esquerda < heapMinimo->tamanho) && (heapMinimo->valores[esquerda].custo < heapMinimo->valores[i].custo))   
        menor = esquerda;
   
    else
        menor = i;

    if((direita < heapMinimo->tamanho) && (heapMinimo->valores[direita].custo < heapMinimo->valores[menor].custo))
        menor = direita;

    if(menor != i){

        VerticeCusto menorVertice = heapMinimo->valores[menor];
        VerticeCusto verticeI = heapMinimo->valores[i];

        heapMinimo->posicoes[menorVertice.vertice] = i;
        heapMinimo->posicoes[verticeI.vertice] = menor;

        trocar(heapMinimo, i, menor);
        atualizarHeapMinimo(heapMinimo, menor);

    }

}

void trocar(HeapMinimo *heapMinimo, int a, int b){

    VerticeCusto aux = heapMinimo->valores[a];
    heapMinimo->valores[a] = heapMinimo->valores[b];
    heapMinimo->valores[b] = aux;

}

void construirHeapMinimo(HeapMinimo *heapMinimo){

    for(int i = 0; i < heapMinimo->tamanho / 2; i++)
        atualizarHeapMinimo(heapMinimo, i);

}

VerticeCusto extrairMinimo(HeapMinimo *heapMinimo){

    VerticeCusto verticeCusto;

    if(heapMinimo->tamanho < 1)
        printf("Erro: heap underflow");

    verticeCusto = heapMinimo->valores[0];

    heapMinimo->valores[0] = heapMinimo->valores[heapMinimo->tamanho - 1];
    heapMinimo->posicoes[verticeCusto.vertice] = heapMinimo->tamanho - 1;
    heapMinimo->posicoes[heapMinimo->valores[0].vertice] = 0;
    heapMinimo->tamanho--;

    atualizarHeapMinimo(heapMinimo, 0);

    return verticeCusto;

}

void diminuirValorChave(HeapMinimo *heapMinimo, int i, double chave){

    if(chave > heapMinimo->valores[i].custo){
        
        printf("Erro: chave maior que o atual");
        return;
    }

    heapMinimo->valores[i].custo = chave;

    while(i > 0 && heapMinimo->valores[pai(i)].custo > heapMinimo->valores[i].custo){

        heapMinimo->posicoes[heapMinimo->valores[i].vertice] = pai(i);
        heapMinimo->posicoes[heapMinimo->valores[pai(i)].vertice] = i;

        trocar(heapMinimo, i, pai(i));
        i = pai(i);
   
    }
    
}

int existe(HeapMinimo *heapMinimo, int vertice){

    return (heapMinimo->posicoes[vertice] < heapMinimo->tamanho) ? 1 : 0;

}

int vazio(HeapMinimo *heapMinimo){

    return (heapMinimo->tamanho <= 0) ? 1 : 0;

}

void marcarNaoVisitados(int visitados[], int tam){

    for(int i = 0; i < tam; i++)
        visitados[i] = 0;

}

void imprimirCustoTotal(clock_t inicio, Ponto pontos[], int ciclo[], int tam){

    printf("%.6f %.6f", (clock() - inicio) / (double)CLOCKS_PER_SEC, calcularCustoTotal(pontos, ciclo, tam));
   
}

void destruirEstruturas(int ciclo[], Ponto pontos[], Grafo *grafo, Grafo *agm){

    free(ciclo);
    free(pontos);
    destruirGrafo(agm);
    destruirGrafo(grafo);

}