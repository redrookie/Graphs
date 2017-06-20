#include "Grafo.h"
#include <stdio.h>
#include <stdlib.h>

TG* criarGrafoPorArquivo(FILE* arq);
int eOrientado(TG *g);
int eConexo(TG *g);
void exibe_articulacoes(TG *g);
void exibe_pontes(TG *g);
void imprimePelaCor(TG *g, int n);
void componentesFortementeConexos(TG *g);
int consigoChegarNoId(TG *g, int id_no, TNO *no);
void calculaGrafo(TG *g);
int opsMenu(int code, TG *g);


int main(int argc, char *argv[]){//Função principal, cria o grafo, chama a função de calculo e em seguida entra no loop de menu.

    FILE *fp;
    fp = fopen (argv[1], "r");
    if(!fp) exit(1);
    TG *g = criarGrafoPorArquivo(fp);
    fclose(fp);
    calculaGrafo(g);
    int code = 7;
    while(code){
	code = opsMenu(code,g);
	}

    return 0;
}


//Verifica se o grafo é orientado. Se for, retorna 1. Senao retorna 0.
int eOrientado(TG *g){
    TNO *no = g->prim;
    while(no){
        TViz *viz = no->prim_viz;
        while(viz){
            if(!(Busca_Aresta(g, no->id_no, viz->id_viz) && Busca_Aresta(g,viz->id_viz, no->id_no))) return 1;
            viz = viz->prox_viz;
        }
        no = no->prox_no;
    }
    return 0;
}

//Verifica se é conexo. Se for conexo, o grafo terá apenas uma cor
//ou seja, todos estão conectados entre si.
int eConexo(TG *g){
    zeraCor(g);//Zeramos a cor, para garantir que ele já não está pintado

    int cor = 0;
    TNO *no = g->prim;

    while(no){
        if(no->cor == 0){
            cor++;
            no->cor = cor;
            pintando(g, no->prim_viz, cor);
        }
        no = no->prox_no;
    }

    if(cor == 1) return 1;
    return 0;
}


//Exibe os pontos de articulação do grafo. A função funciona criando um novo
//Grafo chamado "aux" e para cada Nó do meu Grafo original eu
//Retiro os seus vizinhos e os adiciono no auxiliar(como se fossem novos Nós). Após testar se o grafo
//continua conexo ou não, eu insiro o nós mais uma vez e em sequencia os seus vizinhos.
void exibe_articulacoes(TG *g){
    TG *aux = Cria();
    TNO *no = g->prim;
    while(no){
        int no0 = no->id_no;
        TViz *viz = no->prim_viz;
        no = no->prox_no;
        while(viz){
            insere_no(aux,viz->id_viz);
            viz = viz->prox_viz;
        }
        retira_no(g,no0);
        if(!eConexo(g)) printf("Ponto de Articulacao: %d\n",no0);
        insere_no(g,no0);
        TNO *no2 = aux->prim;
        while(no2){
            insere_aresta(g,no0,no2->id_no);
            insere_aresta(g,no2->id_no,no0);
            TNO *ant = no2;
            no2= no2->prox_no;
            retira_no(aux,ant->id_no);
        }
    }
    libera(aux);
}

//Para verificar as pontes, as arestas de ida e volta são retiradas e depois é verificado
//se o grafo continua conexo ou não. Se não continuar, essa aresta é uma ponte.
//O visto da aresta é para impedir que seja printado uma ponte de 'ida e volta', (x y e y x).
void exibe_pontes(TG *g){
    printf("Pontes:\n");
    TNO *no = g->prim;
    while(no){
        TViz *viz = no->prim_viz;
        while(viz){
            //Como retiraremos as arestas, perderemos o ponteiro do 'viz'.
            //Por isso salvamos as variaveis necessárias e damos um 'viz= viz->prox_viz'
            //antes de apagar as arestas.
            int id_vizinho = viz->id_viz, visto = viz->visto;
            viz = viz->prox_viz;
            if(!visto){
                //Retiramos a aresta de ida e volta
                retira_aresta(g, no->id_no, id_vizinho);
                retira_aresta(g, id_vizinho, no->id_no);
                //Se ele não for mais conexo, essa aresta é uma ponte e printo ela.
                if(!eConexo(g)) printf("%d %d\n",no->id_no, id_vizinho);
                //Adiciono as arestas retiradas
                insere_aresta(g, no->id_no, id_vizinho);
                insere_aresta(g, id_vizinho, no->id_no);
                //Marco tanto a aresta de 'ida' quanto de 'volta' como visto.
                TViz *aux = Busca_Aresta(g, no->id_no, id_vizinho);
                aux->visto = 1;
                aux = Busca_Aresta(g, id_vizinho, no->id_no);
                aux->visto = 1;
            }
        }
        no = no->prox_no;
    }
    //Zeramos o atributo vizinhos de todos os TViz, para que essa função possa ser chamada novamente
    //com o gráfico alterado.
    zeraVistoVizinhos(g);
    printf("\n");
}


void imprimePelaCor(TG *g, int n){
    TNO *no = g->prim; int cor = 1;
    //Primeira parte, veremos quantas cores temos
    while(no){
        if(no->cor > cor) cor = no->cor;
        no = no->prox_no;
    }
    //Agora printaremos os nós
    int i;
    for(i = 1; i<cor+1;i++){
        if(n == 1) printf("Componentes conexas %d:\n",i);
        else printf("Componentes fortemente conexas %d:\n",i);

        no = g->prim;
        while(no){
            if(no->cor == i) printf("%d\n",no->id_no);
            no = no->prox_no;
        }
        printf("\n");
    }
}
/* O que ele faz é simples:
 * - Para cada nó, ele percorrerá todos os nós. Se um nó consegue chegar no outro e vice-versa
 * eles terão a mesma cor e consequentemente fará parte do mesmo conjunto de componente
 * fortemente conexa.
 */
void componentesFortementeConexos(TG *g){
    zeraCor(g);
    TNO *no = g->prim;

    int cor = 1;
    while(no){
        if(!no->cor) {
            no->cor = cor;
            TNO *no_aux = g->prim;
            while(no_aux){
                if(!no_aux->cor && no->id_no != no_aux->id_no){
                    zeraVistoVizinhos(g);
                    if(consigoChegarNoId(g, no->id_no, no_aux)){//Se a partir do no_aux eu consigo chegar no nó com id no->id_no
                        zeraVistoVizinhos(g);
                        if(consigoChegarNoId(g, no_aux->id_no, no)){//Se a partir do no, eu consigo chegar no nó com id no_aux->id
                            no_aux->cor = cor;
                        }
                    }
                }
                no_aux = no_aux->prox_no;
            }
            cor++;
        }
        no = no->prox_no;
    }
}
/*Ele verá se, a partir do nó informado no terceiro parêmetro, se ele consegue chegar até o
 * id_no (informado no segundo parâmetro). Se sim, retorna 1. Senão, retorna 0.
*/
int consigoChegarNoId(TG *g, int id_no, TNO *no){//Se a partir desse no, eu chego nesse id
    TViz *viz = no->prim_viz;
    while(viz){
        if(!viz->visto){
            viz->visto = 1;
            if(viz->id_viz == id_no) return 1;
            if(consigoChegarNoId(g, id_no, Busca_no(g, viz->id_viz))) return 1;
        }
        viz = viz->prox_viz;
    }
    return 0;
}


//Realiza os calculos desejados no grafo através das funções específicas para cada tipo de grafo
void calculaGrafo(TG *g){
    int ori = eOrientado(g);
    printf("É orientado? %d\n\n", ori);
    if(!ori){
        int conexo = eConexo(g);
        printf("É conexo? %d\n\n",conexo);
        if(conexo){
            exibe_articulacoes(g);
            printf("\n");
            exibe_pontes(g);
        }else{
            imprimePelaCor(g, 1);
        }
    }else{
        componentesFortementeConexos(g);
        imprimePelaCor(g, 2);
    }
}


//Realiza as operações do menu
int opsMenu(int code, TG* g){//Imprime o menu na tela e logo em seguida verifica o código digitado pelo usuário.
    printf("\n--------------------Menu-------------------\nDigite o número correspondente à opção desejada:");
    printf("\n1- Inserir nó\n");
    printf("2- Inserir aresta\n");
    printf("3- Remover nó\n");
    printf("4- Remover aresta\n");
    printf("5- Buscar Nó\n");
    printf("6- Buscar Aresta\n");
    printf("7- Calcular grafo\n");
    printf("0 - Finalizar programa\n");

    printf("\n--------------------------------------------\n");
    printf("\nQue operação deseja realizar? ");


    scanf("%d", &code );

    if(code==0){//libera o grafo e retorna valor que sai do loop de menu
	libera(g);
	return code;
}
    else if(code==1){//Pede o nó e insere no grafo.
        int idNo;
        printf("\nId do nó desejado: ");
        scanf("%d",&idNo);
        insere_no(g,idNo);
	return code;
    }
    else if(code ==2){//Pede os nós e insere a aresta no grafo.
	int idNo1,idNo2;
	printf("\nId do nó de partida: ");
	scanf("%d",&idNo1);
	printf("\nId do nó de chegada: ");
	scanf("%d",&idNo2);
	insere_aresta(g,idNo1,idNo2);
	return code;
    }
    else if(code==3){//Pede o nó e o remove.
	int idNo;
        printf("\nId do nó desejado: ");
        scanf("%d",&idNo);
	retira_no(g,idNo);
	return code;
    }
    else if(code==4){//Pede os nós e remove a aresta que os liga.
	int idNo1,idNo2;
	printf("\nId do nó de partida: ");
	scanf("%d",&idNo1);
	printf("\nId do nó de chegada: ");
	scanf("%d",&idNo2);
	retira_aresta(g,idNo1,idNo2);
	return code;
	}
    else if(code==5){//Pede o nó que será buscado e imprime mensagem que diz se o nó existe.
	int idNo;
        printf("\nId do nó desejado: ");
        scanf("%d",&idNo);
	if(Busca_no(g,idNo)) printf("\nO nó existe!");
	else printf("\nO nó não existe no grafo.");
	return code;
	}
    else if(code==6){//Pede os nós e imprime mensagem dizendo se a aresta existe
	int idNo1,idNo2;
	printf("\nId do nó de partida: ");
	scanf("%d",&idNo1);
	printf("Id do nó de chegada: ");
	scanf("%d",&idNo2);
	if(Busca_Aresta(g,idNo1,idNo2)) printf("\n\nA aresta existe!") ;
	else printf("\n\nA aresta não existe.");
	return code;
    }
    else if (code==7){//chama a função que faz os cálculos no grafo
	calculaGrafo(g);
	return code;
    }	  
}

