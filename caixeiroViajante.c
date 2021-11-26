#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <float.h>

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

typedef struct ponto {
    
    double x;
    double y;

} Ponto;

//Funcoes para pontos
double calcularDistanciaPontos(Ponto p1, Ponto p2);

//Funcoes para grafos
Grafo *criarGrafo(int tamanho);
Grafo *lerArquivo(char nomeArquivo[], int *tam);
void destruirGrafo(Grafo *grafo);
void adicionarAresta(int v1, int v2, double peso, Grafo *grafo);
void prim(Grafo *grafo, int vertice);
void buscaProfundidade(Grafo *grafo, int visitados[], int *tempo);
void buscaProfundidadeAuxiliar(Grafo *grafo, int vertice, int prodecessores[], int visitados[], int **tempos, int *tempo);
void iniciarProdecessores(Grafo *grafo, int prodecessores[]);
void marcarNaoVisitados(Grafo *grafo, int visitados[]);
void imprimirGrafo(Grafo *grafo);
void imprimirPrim(Grafo *grafo, int prodecessoes[]);

//Funcoes para HEAP minimo
int pai(int i);
int existe(VerticeCusto V[], int vertice, int tam);
int retornarPosicao(VerticeCusto V[], int vertice, int tam);
void trocar(VerticeCusto V[], int a, int b);
void atualizarHeapMinimo(VerticeCusto V[], int tam, int i);
void criarHeapMinimo(VerticeCusto V[], int tam);
void diminuirValorChave(VerticeCusto V[], int i, double chave);
void inserirHeapMinimo(VerticeCusto V[], double chave, int *tam);
VerticeCusto extrairMinimo(VerticeCusto V[], int *tam);
void imprimirHeap(VerticeCusto V[], int tam);

int main(int argc, char *argv[]){

    Grafo *grafo;
    int tam;

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
    adicionarAresta(4, 1, 141, grafo);
    adicionarAresta(4, 2, 141, grafo);
    adicionarAresta(4, 3, 223, grafo);
    adicionarAresta(4, 5, 100, grafo);

    adicionarAresta(5, 0, 200, grafo);
    adicionarAresta(5, 1, 223, grafo);
    adicionarAresta(5, 2, 223, grafo);
    adicionarAresta(5, 3, 141, grafo);
    adicionarAresta(5, 4, 100, grafo);

    imprimirGrafo(grafo);

    printf("\n");

    prim(grafo, 0);

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

Grafo *lerArquivo(char nomeArquivo[], int *tam){

    FILE *arquivo;
    Grafo *grafo;
    Ponto *pontos;

    arquivo = fopen(nomeArquivo, "r");

    if(arquivo == NULL){

        printf("ERRO AO ABRIR O ARQUIVO.");
        getchar();
        exit(1);

    }

    fscanf(arquivo, "%d", tam);

    grafo = criarGrafo(*tam);
    pontos = (Ponto*) malloc ((*tam) * sizeof (Ponto));
    
    for(int i = 0; i < (*tam); i++)
        fscanf(arquivo, "%lf %lf", &pontos[i].x, &pontos[i].y);
        
    for(int i = 0; i < grafo->vertices; i++){
        
        for(int j = 0; j < grafo->vertices; j++)
            if(i != j)
                adicionarAresta(i, j, calcularDistanciaPontos(pontos[i], pontos[j]), grafo);

    }
    
    fclose(arquivo);

    return grafo;

}

void destruirGrafo(Grafo *grafo){

    for(int i = 0; i < grafo->vertices; i++)
        if(grafo->adjacencias[i] != NULL)
            free(grafo->adjacencias[i]);

    free(grafo->adjacencias);
    free(grafo);

}

void imprimirGrafo(Grafo *grafo){

    for(int i = 0; i < grafo->vertices; i++){

        printf("%d -> ", i);

        for(No *aux = grafo->adjacencias[i]; aux != NULL; aux = aux->proximo)
            printf("%d (peso: %lf) ", aux->id, aux->peso);

        printf("\n");

    }
    
}

void adicionarAresta(int v1, int v2, double peso, Grafo *grafo){

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

void prim(Grafo *grafo, int vertice){

    int prodecessores[grafo->vertices], tamHeap = grafo->vertices;
    double custos[grafo->vertices];
    VerticeCusto heap[tamHeap];

    for(int i = 0; i < tamHeap; i++){

        heap[i].vertice = i;
        heap[i].custo = 55555;
        prodecessores[i] = -1;

    }

    criarHeapMinimo(heap, tamHeap);
    diminuirValorChave(heap, vertice, 0);
    imprimirHeap(heap, tamHeap);

    while(tamHeap > 0){

        VerticeCusto u = extrairMinimo(heap, &tamHeap);

        for(No *aux = grafo->adjacencias[u.vertice]; aux != NULL; aux = aux->proximo){

            if(existe(heap, aux->id, tamHeap) && aux->peso < heap[retornarPosicao(heap, aux->id, tamHeap)].custo){
                
                int posicao = retornarPosicao(heap, aux->id, tamHeap);
                prodecessores[aux->id] = u.vertice;
                diminuirValorChave(heap, posicao, aux->peso);

            }

        }

        printf("%lf ", u.custo);

    }

    imprimirPrim(grafo, prodecessores);

}

void imprimirPrim(Grafo *grafo, int prodecessoes[]){

    printf("\n");
    for(int i = 0; i < grafo->vertices; i++)
        printf("%d - %d\n", i, prodecessoes[i]);

}

//Funcoes para HEAP minimo

int pai (int i){

    return (i - 1) / 2;

}

void atualizarHeapMinimo(VerticeCusto V[], int tam, int i){

    int esquerda = (i * 2) + 1;
    int direita = (i * 2) + 2;
    int menor;

    if((esquerda < tam) && (V[esquerda].custo < V[i].custo))   
       menor = esquerda;
   
    else
       menor = i;

    if((direita < tam) && (V[direita].custo < V[menor].custo))
       menor = direita;

    if(menor != i){

       trocar(V, i, menor);
       atualizarHeapMinimo(V, tam, menor);

    }

}

void trocar(VerticeCusto V[], int a, int b){

    VerticeCusto aux = V[a];
    V[a] = V[b];
    V[b] = aux;

}

void criarHeapMinimo(VerticeCusto V[], int tam){

    for(int i = 0; i < tam / 2; i++)
       atualizarHeapMinimo(V, tam, i);

}

VerticeCusto extrairMinimo(VerticeCusto V[], int *tam){

    VerticeCusto verticeCusto;
    int min;

    if((*tam) < 1)
       printf("Erro: heap underflow");

    verticeCusto = V[0];
    V[0] = V[(*tam) - 1];
    (*tam)--;
    atualizarHeapMinimo(V, *tam, 0);

    return verticeCusto;

}

void diminuirValorChave(VerticeCusto V[], int i, double chave){

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

void inserirHeapMinimo(VerticeCusto V[], double chave, int *tam){

    (*tam)++;
    V[(*tam) - 1].custo = INT_MAX;
    diminuirValorChave(V, (*tam) - 1, chave);

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

void imprimirHeap(VerticeCusto V[], int tam){
    int i;

    for (i = 0; i < tam; i++){

        printf("%lf ", V[i].custo);
        printf("\n");

    }

}