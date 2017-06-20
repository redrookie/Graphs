#ifndef GRAFO_H_INCLUDED
#define GRAFO_H_INCLUDED

typedef struct Vizinhos
{
    struct Vizinhos *prox_viz;
    int id_viz;
    int visto;

} TViz;

typedef struct No
{
    int id_no;
    struct No *prox_no;
    TViz *prim_viz;
    int cor;

} TNO;


typedef struct Grafo
{
    struct No *prim;

} TG;

TG* Cria();
TViz* Busca_Aresta(TG *g,int no1, int no2);
void insere_aresta(TG *g,int no1,int no2);
void insere_no(TG *g,int no);
TNO* Busca_no(TG *g,int no);
void imprime(TG *g);
void retira_aresta(TG *g,int id1,int id2);
void retira_no(TG *g, int id);
void libera(TG *g);
void zeraCor(TG *g);
void zeraVistoVizinhos(TG *g);
void pintando(TG *g, TViz *viz, int cor);



#endif // GRAFO_H_INCLUDED
