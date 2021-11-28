#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
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

    int tamanhoTotal;
    int numeroVertices;
    int *posicoes;
    VerticeCusto **valores;

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
void imprimirGrafo(Grafo *grafo);
void exportarAGM(Grafo *agm, Ponto pontos[]);
double calcularCustoTotal(Ponto pontos[], int ciclo[], int tam);
Ponto *lerArquivo(char nomeArquivo[], int *tam);
Grafo *prim(Grafo *grafo, int vertice, Ponto pontos[]);
Grafo *criarGrafo(int tamanho);
Grafo *preencherGrafo(Ponto pontos[], int tam);

//Funcoes para HEAP minimo
int existe(HeapMinimo *heapMinimo, int vertice);
void trocar(VerticeCusto **v1, VerticeCusto **v2);
void atualizarHeapMinimo(HeapMinimo *heapMinimo, int i);
void diminuirValorCusto(HeapMinimo *heapMinimo, int vertice, int custo);
void inserirHeapMinimo(VerticeCusto V[], double chave, int *tam);
VerticeCusto *extrairMinimo(HeapMinimo *heapMinimo);
VerticeCusto *adicionarVerticeHeap(int vertice, double custo);
HeapMinimo *criarHeapMinimo(int tam);
int vazio(HeapMinimo *heapMinimo);

int main(int argc, char *argv[]){

    Grafo *grafo, *agm;
    Ponto *pontos;
    int tam, *ciclo;

    /*
    grafo = criarGrafo(6);

    adicionarAresta(0, 1, 223, grafo);
    adicionarAresta(0, 2, 360, grafo);
    adicionarAresta(0, 3, 316, grafo);
    adicionarAresta(0, 4, 223, grafo);
    adicionarAresta(0, 5, 200, grafo);

    adicionarAresta(1, 0, 223, grafo);
    adicionarAresta(1, 2, 200, grafo);
    adicionarAresta(1, 3, 360, grafo);
    adicionarAresta(1, 4, 141, grafo);
    adicionarAresta(1, 5, 223, grafo);

    adicionarAresta(2, 0, 360, grafo);
    adicionarAresta(2, 1, 200, grafo);
    adicionarAresta(2, 3, 300, grafo);
    adicionarAresta(2, 4, 141, grafo);
    adicionarAresta(2, 5, 223, grafo);

    adicionarAresta(3, 0, 316, grafo);
    adicionarAresta(3, 1, 360, grafo);
    adicionarAresta(3, 2, 300, grafo);
    adicionarAresta(3, 4, 223, grafo);
    adicionarAresta(3, 5, 141, grafo);

    adicionarAresta(4, 0, 223, grafo);
    adicionarAresta(4, 2, 141, grafo);
    adicionarAresta(4, 1, 141, grafo);
    adicionarAresta(4, 3, 223, grafo);
    adicionarAresta(4, 5, 100, grafo);

    adicionarAresta(5, 0, 200, grafo);
    adicionarAresta(5, 1, 223, grafo);
    adicionarAresta(5, 2, 223, grafo);
    adicionarAresta(5, 3, 141, grafo);
    adicionarAresta(5, 4, 100, grafo);
    */

    pontos = lerArquivo("input.txt", &tam);
    grafo = preencherGrafo(pontos, tam);
    
    //printf("Grafo completo: \n");
    //imprimirGrafo(grafo);

    //printf("\n");

    agm = prim(grafo, 0, pontos);
    ciclo = buscaProfundidade(agm, 0);

    printf("Custo total: %lf\n", calcularCustoTotal(pontos, ciclo, agm->vertices));

    free(ciclo);
    free(pontos);
    destruirGrafo(grafo);
    destruirGrafo(agm);

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

void imprimirGrafo(Grafo *grafo){

    for(int i = 0; i < grafo->vertices; i++){

        printf("%d -> ", i + 1);

        for(No *aux = grafo->adjacencias[i]; aux != NULL; aux = aux->proximo)
            printf("%d (peso: %lf), ", aux->id + 1, aux->peso);

        printf("\n");

    }
    
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

    for(int i = 1; i < grafo->vertices; i++){

        custos[i] = DBL_MAX;
        heapMinimo->valores[i] = adicionarVerticeHeap(i, custos[i]);
        heapMinimo->posicoes[i] = i;
        prodecessores[i] = -1;

    }

    custos[0] = 0;
    heapMinimo->valores[0] = adicionarVerticeHeap(0, custos[0]);
    heapMinimo->posicoes[0] = 0;

    heapMinimo->numeroVertices = grafo->vertices;

    while(!vazio(heapMinimo)){

        VerticeCusto *u = extrairMinimo(heapMinimo);

        for(No *aux = grafo->adjacencias[u->vertice]; aux != NULL; aux = aux->proximo){

            if(existe(heapMinimo, aux->id) && aux->peso < custos[aux->id]){
                
                custos[aux->id] = aux->peso;
                prodecessores[aux->id] = u->vertice;
                diminuirValorCusto(heapMinimo, aux->id, custos[aux->id]);

            }

        }

    }

    for(int i = 1; i < grafo->vertices; i++){

        adicionarAresta(prodecessores[i], i, custos[i], agm);
        adicionarAresta(i, prodecessores[i], custos[i], agm);

    }

    double custoAGM = 0;

    for(int i = 0; i < agm->vertices; i++)
        custoAGM += custos[i];

    printf("Custo agm: %lf\n", custoAGM);

    exportarAGM(agm, pontos);

    //imprimirGrafo(agm);

    return agm;

}

int *buscaProfundidade(Grafo *agm, int vertice){

    int *ciclo, itr = 1, visitados[agm->vertices];

    ciclo = malloc ((agm->vertices + 1) * sizeof (int));

    for(int i = 0; i < agm->vertices; i++)
        visitados[i] = 0;
    
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

void exportarAGM(Grafo *agm, Ponto pontos[]){

    FILE *arquivo;

    arquivo = fopen("tree.txt", "w");

    if(arquivo == NULL){

        printf("ERRO AO ABRIR O ARQUIVO.");
        getchar();
        exit(1);

    }

    for(int i = 0; i < agm->vertices; i++){

        for(No *aux = agm->adjacencias[i]; aux != NULL; aux = aux->proximo){

            fprintf(arquivo, "%d %d\n%d %d\n\n", (int) pontos[i].x, (int) pontos[i].y, (int) pontos[aux->id].x, (int) pontos[aux->id].y);

        }

    }

    fclose(arquivo);

}

double calcularDistanciaPontos(Ponto p1, Ponto p2){

    return sqrt(pow((p1.x - p2.x) , 2) + pow((p1.y - p2.y), 2));

}

HeapMinimo *criarHeapMinimo(int tam){

    HeapMinimo *heapMinimo = (HeapMinimo *) malloc (sizeof (HeapMinimo));

    heapMinimo->posicoes = (int *) malloc (tam * sizeof (int));
    heapMinimo->numeroVertices = 0;
    heapMinimo->tamanhoTotal = tam;
    heapMinimo->valores = (VerticeCusto **) malloc (tam * sizeof (VerticeCusto *));

    return heapMinimo;

}

VerticeCusto *adicionarVerticeHeap(int vertice, double custo){

    VerticeCusto *verticeCusto = (VerticeCusto *) malloc (sizeof (VerticeCusto));
    verticeCusto->vertice = vertice;
    verticeCusto->custo = custo;

    return verticeCusto;

}

void trocar(VerticeCusto **v1, VerticeCusto **v2){

    VerticeCusto *aux = *v1;
    *v1 = *v2;
    *v2 = aux;

}

void atualizarHeapMinimo(HeapMinimo *heapMinimo, int i){

    int direita = (2 * i) + 2, esquerda = (2 * i) + 1, menor = i;

    if(esquerda < heapMinimo->numeroVertices && heapMinimo->valores[esquerda]->custo < heapMinimo->valores[menor]->custo)
        menor = esquerda;

    if(direita < heapMinimo->numeroVertices && heapMinimo->valores[direita]->custo < heapMinimo->valores[menor]->custo)
        menor = direita;

    if(menor != i){

        VerticeCusto *menorVertice = heapMinimo->valores[menor];
        VerticeCusto *verticeI = heapMinimo->valores[i];

        heapMinimo->posicoes[menorVertice->vertice] = i;
        heapMinimo->posicoes[verticeI->vertice] = menor;

        trocar(&heapMinimo->valores[menor], &heapMinimo->valores[i]);

        atualizarHeapMinimo(heapMinimo, menor);

    }

}

int vazio(HeapMinimo *heapMinimo){

    return heapMinimo->numeroVertices == 0 ? 1 : 0;

}

VerticeCusto *extrairMinimo(HeapMinimo *heapMinimo){

    if(vazio(heapMinimo))
        return NULL;

    VerticeCusto *raiz = heapMinimo->valores[0];
    VerticeCusto *ultimo = heapMinimo->valores[heapMinimo->numeroVertices - 1];
    heapMinimo->valores[0] = ultimo;

    heapMinimo->posicoes[raiz->vertice] = heapMinimo->numeroVertices - 1;
    heapMinimo->posicoes[ultimo->vertice] = 0;

    heapMinimo->numeroVertices--;
    atualizarHeapMinimo(heapMinimo, 0);

    return raiz;

}

void diminuirValorCusto(HeapMinimo *heapMinimo, int vertice, int custo){

    int posicao = heapMinimo->posicoes[vertice];

    heapMinimo->valores[posicao]->custo = custo;

    while(posicao && heapMinimo->valores[posicao]->custo < heapMinimo->valores[(posicao - 1) / 2]->custo){

        heapMinimo->posicoes[heapMinimo->valores[posicao]->vertice] = (posicao - 1) / 2;
        heapMinimo->posicoes[heapMinimo->valores[(posicao - 1) / 2]->vertice] = posicao;
        trocar(&heapMinimo->valores[posicao], &heapMinimo->valores[(posicao - 1) / 2]);

        posicao = (posicao - 1) / 2;

    }

}

int existe(HeapMinimo *heapMinimo, int vertice){

    if(heapMinimo->posicoes[vertice] < heapMinimo->numeroVertices)
        return 1;

    return 0;
    
}