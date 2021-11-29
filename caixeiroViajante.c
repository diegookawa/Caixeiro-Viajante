#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
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
void exportarCiclo(int ciclo[], Ponto *pontos, int tam);
double calcularCustoTotal(Ponto pontos[], int ciclo[], int tam);
Ponto *lerArquivo(char nomeArquivo[], int *tam);
Grafo *prim(Grafo *grafo, int vertice, Ponto pontos[]);
Grafo *criarGrafo(int tamanho);
Grafo *preencherGrafo(Ponto pontos[], int tam);

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

int main(int argc, char *argv[]){

    clock_t inicio = clock();
    Grafo *grafo, *agm;
    Ponto *pontos;
    char *nomeArquivo = (argc > 1) ? argv[1] : "input.txt";
    int tam, *ciclo;

    pontos = lerArquivo(nomeArquivo, &tam);
    grafo = preencherGrafo(pontos, tam);
    
    //printf("Grafo completo: \n");
    //imprimirGrafo(grafo);

    //printf("\n");

    agm = prim(grafo, 0, pontos);
    ciclo = buscaProfundidade(agm, 0);

    exportarAGM(agm, pontos);
    exportarCiclo(ciclo, pontos, tam);

    printf("%.6f %.6f", (clock() - inicio) / (double)CLOCKS_PER_SEC, calcularCustoTotal(pontos, ciclo, agm->vertices));

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

    int prodecessores[grafo->vertices], tamHeap = grafo->vertices;
    double custos[grafo->vertices];
    VerticeCusto heap[tamHeap];
    Grafo *agm;

    agm = criarGrafo(grafo->vertices);    

    for(int i = 0; i < tamHeap; i++){

        custos[i] = DBL_MAX;
        heap[i].vertice = i;
        heap[i].custo = custos[i];
        prodecessores[i] = -1;

    }
    custos[0] = 0;

    criarHeapMinimo(heap, tamHeap);
    diminuirValorChave(heap, vertice, 0);

    while(tamHeap > 0){

        VerticeCusto u = extrairMinimo(heap, &tamHeap);

        for(No *aux = grafo->adjacencias[u.vertice]; aux != NULL; aux = aux->proximo){

            if(existe(heap, aux->id, tamHeap) && aux->peso < custos[aux->id]){
                
                int posicao = retornarPosicao(heap, aux->id, tamHeap);
                custos[aux->id] = aux->peso;
                prodecessores[aux->id] = u.vertice;
                diminuirValorChave(heap, posicao, aux->peso);

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

        for(No *aux = agm->adjacencias[i]; aux != NULL; aux = aux->proximo){

            fprintf(arquivo, "%d %d\n%d %d\n", (int) pontos[i].x, (int) pontos[i].y, (int) pontos[aux->id].x, (int) pontos[aux->id].y);

        }

    }

    fclose(arquivo);

}

double calcularDistanciaPontos(Ponto p1, Ponto p2){

    return sqrt(pow((p1.x - p2.x) , 2) + pow((p1.y - p2.y), 2));

}

int pai(int i){

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