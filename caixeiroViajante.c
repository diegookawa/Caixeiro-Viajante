#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>

typedef struct no {

    int id;
    int peso;
    struct no *proximo;
    
} No;

typedef struct grafo {

    int arestas;
    int vertices;
    No **adjacencias;

} Grafo;

typedef struct verticeCusto {

    int vertice;
    int custo;

} VerticeCusto;

typedef struct ponto {
    
    double x;
    double y;

} Ponto;

//Funcoes para pontos
double calcularDistanciaPontos(Ponto p1, Ponto p2);

//Funcoes para grafos
Grafo *criarGrafo(int tamanho);
void destruirGrafo(Grafo *grafo);
void adicionarAresta(int v1, int v2, int peso, Grafo *grafo);
void prim(Grafo *grafo, int vertice);
void buscaProfundidade(Grafo *grafo, int visitados[], int *tempo);
void buscaProfundidadeAuxiliar(Grafo *grafo, int vertice, int prodecessores[], int visitados[], int **tempos, int *tempo);
void iniciarProdecessores(Grafo *grafo, int prodecessores[]);
void marcarNaoVisitados(Grafo *grafo, int visitados[]);

//Funcoes para HEAP minimo
int pai(int i);
int existe(VerticeCusto V[], int vertice, int tam);
int retornarPosicao(VerticeCusto V[], int vertice, int tam);
void trocar(VerticeCusto V[], int a, int b);
void imprimir(VerticeCusto V[], int size);
void atualizarHeapMinimo(VerticeCusto V[], int size, int i);
void criarHeapMinimo(VerticeCusto V[], int size);
void diminuirValorChave(VerticeCusto V[], int i, int chave, int size);
void inserirHeapMinimo(VerticeCusto V[], int chave, int *size);
VerticeCusto extrairMinimo(VerticeCusto V[], int *size);

int main(int argc, char *argv[]){

    return 0;

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

void destruirGrafo(Grafo *grafo){

    for(int i = 0; i < grafo->vertices; i++)
        if(grafo->adjacencias[i] != NULL)
            free(grafo->adjacencias[i]);

    free(grafo->adjacencias);
    free(grafo);

}

void adicionarAresta(int v1, int v2, int peso, Grafo *grafo){

    No *ultimo = NULL;

    for(No *aux = grafo->adjacencias[v1]; aux != NULL; aux = aux->proximo){

        if(aux->id == v2){

            printf("A aresta ja existe\n");
            return;

        }

        ultimo = aux;

    }

    No *novo = (No *) malloc (sizeof(No));
    novo->id = v2;
    novo->proximo = NULL;
    novo->peso = peso;

    if(ultimo != NULL)
        ultimo->proximo = novo;

    else 
        grafo->adjacencias[v1] = novo;

    grafo->arestas++;

}

void marcarNaoVisitados(Grafo *grafo, int visitados[]){

    for(int i = 0; i < grafo->vertices; i++)
        visitados[i] = 0;

}

void iniciarProdecessores(Grafo *grafo, int prodecessores[]){

    for(int i = 0; i < grafo->vertices; i++)
        prodecessores[i] = -1;

}

int pai (int i) {

   return (i - 1) / 2;

}

void atualizarHeapMinimo(VerticeCusto V[], int size, int i) {

   int esquerda = (i * 2) + 1;
   int direita = (i * 2) + 2;
   int menor;

   if((esquerda < size) && (V[esquerda].custo < V[i].custo))   
      menor = esquerda;
   
   else
      menor = i;

   if((esquerda < size) && (V[direita].custo < V[menor].custo))
      menor = direita;

   if(menor != i){

      trocar(V, i, menor);
      atualizarHeapMinimo(V, size, menor);

   }

}

void trocar(VerticeCusto V[], int a, int b) {

   VerticeCusto aux = V[a];
   V[a] = V[b];
   V[b] = aux;

}

void criarHeapMinimo(VerticeCusto V[], int size) {

   for(int i = 0; i < size/2; i++)
      atualizarHeapMinimo(V, size, i);

}

VerticeCusto extrairMinimo(VerticeCusto V[], int *size) {

   VerticeCusto vc;
   int min;

   if((*size) < 1)
      printf("Erro: heap underflow");

   vc = V[0];
   V[0] = V[(*size) - 1];
   (*size)--;
   atualizarHeapMinimo(V, *size, 0);

   return vc;

}

void diminuirValorChave(VerticeCusto V[], int i, int chave, int size) {

   if(chave > V[i].custo){

      printf("Erro: chave maior que o atual");
      return;

   }

   V[i].custo = chave;
   while(i > 0 && V[pai(i)].custo > V[i].custo){

      trocar(V, i, pai(i));
      i = pai(i);
   
   }

}

void inserirHeapMinimo(VerticeCusto V[], int chave, int *size) {

   (*size)++;
   V[(*size) - 1].custo = INT_MAX;
   diminuirValorChave(V, (*size) - 1, chave, *size);

}

int existe(VerticeCusto V[], int vertice, int tam){

    for(int i = 0; i < tam; i++)
        if(vertice == V[i].vertice)
            return 1;

    return 0;
}

int retornarPosicao(VerticeCusto V[], int vertice, int tam){

    for(int i = 0; i < tam; i++)
        if(vertice == V[i].vertice)
            return i;

    return -1;

}

double calcularDistanciaPontos(Ponto p1, Ponto p2){

    return sqrt(pow((p1.x - p2.x) , 2) + pow((p1.y - p2.y), 2));

}