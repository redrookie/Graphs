#include<stdlib.h>
#include<stdio.h>
#include "Grafo.h"



//Cria a estrutura do grafo
TG* Cria(){
    TG *g = (TG*)malloc(sizeof(TG));
    g->prim=NULL;
    return g;

}

//Inserindo nó no grafo
//O valor default para a cor é 0
void insere_no(TG *g,int no){
    TNO *p = Busca_no(g,no);    //Se p for nulo, é porque o nó ja existe
    if(p) return;
    p = (TNO*)malloc(sizeof(TNO));
    p->id_no = no;
    p->prim_viz = NULL;
    p->prox_no = g->prim;
    p->cor=0;
    g->prim=p;
}

//Inserindo aresta no grafo
void insere_aresta(TG *g,int no1,int no2){
    TNO *p1 = Busca_no(g,no1);
    if(!p1) return;
    TNO *p2 = Busca_no(g,no2);
    if(!p2) return;
    TViz *v = Busca_Aresta(g,no1,no2);
    if(v) return;
    v = (TViz*)malloc(sizeof(TViz));
    v->id_viz = no2;
    v->prox_viz = p1->prim_viz;
    v->visto = 0;
    p1->prim_viz = v;
}


//Excluindo nó
void retira_no(TG *g, int id){
    //Retiramos todas as arestas que vão desse nó à outro
    TNO *p = g->prim, *ant=NULL;
    while(p && p->id_no!=id){
        ant = p;
        p = p->prox_no;
    }
    if(!p) return;
    TViz *v = p->prim_viz;
    while(v){
        retira_aresta(g,id,v->id_viz);
        v = p->prim_viz;
    }
    if(!ant) g->prim=p->prox_no;
    else ant->prox_no = p->prox_no;

    //Free no nó excluído
    free(p);

    //Retiramos todas as arestas que vão de outros nós a esse que deve ser excluído
    p = g->prim;
    while(p) {
        retira_aresta(g, p->id_no, id);
        p = p->prox_no;
    }
}


//Excluindo aresta
void retira_aresta(TG *g,int id1,int id2){
    TNO *p1 = Busca_no(g,id1);
    if(!p1) return;
    TViz *v = p1->prim_viz, *ant=NULL;
    while(v && v->id_viz!=id2){
        ant = v;
        v = v->prox_viz;
    }
    if(!v) return;
    if(!ant) p1->prim_viz = v->prox_viz;
    else ant->prox_viz = v->prox_viz;
    free(v);
}

//Buscando nó
TNO* Busca_no(TG *g,int no){
    TNO *p = g->prim;
    while(p && p->id_no!=no) p = p->prox_no;
    return p;
}

//Buscando aresta
TViz* Busca_Aresta(TG *g,int no1,int no2){
    TNO *viz1 = Busca_no(g,no1);
    if(!viz1) return NULL;

    TViz *p = viz1->prim_viz;
    while(p && p->id_viz!=no2) p = p->prox_viz;
    return p;
}

//Inprimindo grafo
void imprime(TG *g){
    TNO *p = g->prim;
    while(p){
        printf("Vizinhos do grafo %d:\n",p->id_no);
        TViz *v = p->prim_viz;
        if(!v) printf("Nenhum vizinho!\n");
        while(v){
            printf("%d\n",v->id_viz);
            v = v->prox_viz;
        }
        p = p->prox_no;
        printf("\n");
    }
}

//Liberando o grafo
void libera(TG *g){
    TNO *p = g->prim;
    while(p){
        TViz *v = p->prim_viz;
        while(v){
            TViz *t = v;
            v = v->prox_viz;
            free(t);
        }
        TNO *q = p;
        p = p->prox_no;
        free(q);
    }
    free(g);
}

//Zerando as cores
void zeraCor(TG *g){
    TNO *no = g->prim;
    while(no){
        no->cor = 0;
        no = no->prox_no;
    }
}
//Zera os valores de 'visto' das arestas
void zeraVistoVizinhos(TG *g){
    TNO *no = g->prim;
    while(no){
        TViz *viz = no->prim_viz;
        while(viz){
            viz->visto = 0;
            viz = viz->prox_viz;
        }
        no = no->prox_no;
    }
}


//Dado um vizinho, ele recursivamente pintará todos nós que conseguir alcançar
void pintando(TG *g, TViz *viz, int cor){
    while(viz){
        TNO *aux_no = Busca_no(g, viz->id_viz);
        if(aux_no->cor == 0) {
            aux_no->cor = cor;
            pintando(g, aux_no->prim_viz, cor);
        }
        viz = viz->prox_viz;
    }
}

//Nome autoexplicativo
TG* criarGrafoPorArquivo(FILE* arq){
    TG* g = Cria();

    int tam;
    fscanf(arq,"%d",&tam);

    int no,viz;
    while(!feof(arq)){
        fscanf(arq,"%d%d",&no, &viz);
        if(!Busca_no(g,no)){
            insere_no(g, no);
            tam--;
        }
        if(!Busca_no(g, viz)){
            insere_no(g, viz);
            tam--;
        }
        insere_aresta(g, no, viz);
    }

    int id_inicial = 1;
    while(tam > 0){
        if(!Busca_no(g, id_inicial)){
            insere_no(g, id_inicial);
            tam--;
        }
        id_inicial++;
    }

    return g;
}
