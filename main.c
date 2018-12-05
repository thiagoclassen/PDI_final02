#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "pdi.h"

#define ORIGINAL "./images/letra_s.bmp"
#define JANELA 3


double shortestDistance(Dot a,Dot b,Dot c);
void getPoints(Imagem *img, int y, int x, Header *head);
void calculaDistancias(Header *head);
double removeLowValues(Header *head);
float shortestDistanceTo(Dot pt1, Dot pt2, Dot ponto);

int main() {

    Imagem *imagem, *erosao, *kernel, *bordas;
    Coordenada centroKernel;
    centroKernel.x = 1;
    centroKernel.y = 1;

    Header head, headAux;

	head.head = NULL;
	head.tail = NULL;
	head.nodeCount = 0;

	headAux.head = NULL;
	headAux.tail = NULL;
	headAux.nodeCount = 0;

    //Abre a imagem a ser borrada
    printf("A carregar a imagem [ %s ]... ", ORIGINAL);
    imagem = abreImagem(ORIGINAL, 1);
    
    if(!imagem) {
        printf("ERRO: A imagem não pode ser aberta.\n");
        exit(1);
    }
    printf("OK\n");

    erosao = criaImagem(imagem->largura, imagem->altura, imagem->n_canais);
    bordas = criaImagem(imagem->largura, imagem->altura, imagem->n_canais);

    kernel = criaKernelCircular(3);
    
    erode(imagem, kernel, centroKernel, erosao);

    soma(imagem, erosao, 1.0f, -1.0f, bordas);


    // popula lista encadeada
    for(int y = 0, flag = 0; y < bordas->altura && flag != 1; y++){
        for(int x = 0; x < bordas->largura && flag != 1; x++){
            if(bordas->dados[0][y][x] == 1.0f) {
                getPoints(bordas, y, x, &head);
                flag = 1;
            }
        }
    }


    for(int idx = 0; idx< 500; idx++){
        calculaDistancias(&head);
        removeLowValues(&head);
    }

    //calculaDistancias(&head);
    //printList(&head);
    double removed = 0.0f;
    /*do{
        calculaDistancias(&head);
        removed = removeLowValues(&head);
    }while(removed <= 0.25f);*/

    Imagem *teste = criaImagem(imagem->largura, imagem->altura, imagem->n_canais);

    for(int idx = 0;idx < head.nodeCount; idx++){
        Dot ponto = getDotAtN(&head, idx);
        teste->dados[0][(int)ponto.y][(int)ponto.x] = 1.0f;
    }

    salvaImagem(teste, "teste.bmp");
    destroiImagem(teste);

    salvaImagem(erosao, "erosao.bmp");
    salvaImagem(bordas, "bordas.bmp");

    destroiImagem(imagem);
    destroiImagem(erosao);
    destroiImagem(bordas);

    wipeList(&head);

    return 0;
}

float shortestDistanceTo(Dot pt1, Dot pt2, Dot ponto)
{
	Dot a;
    a.x = ponto.x - pt1.x; a.y = ponto.y - pt1.y; // Vector ponto to pt1.
	Dot b;
    b.x = ponto.x - pt2.x; b.y = ponto.y - pt2.y; // Vector ponto to pt2.
    Dot c;
	c.x = pt2.x - pt1.x; c.y = pt2.y - pt1.y; // Vector pt2 to pt1.

    float length = sqrtf ((pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y));

	float d = (a.x*c.x + a.y*c.y) / (length*length);
	if (d <= 0) // The point is beyond pt1.
		return (sqrtf (a.x*a.x + a.y*a.y));
	else if (d >= 1.0f) // The point is beyond pt2.
		return (sqrtf (b.x*b.x + b.y*b.y));

	Dot proj;
    proj.x = pt1.x + d*c.x;
    proj.y = pt1.y + d*c.y;
	return (sqrtf ((ponto.x - proj.x)*(ponto.x - proj.x) + (ponto.y - proj.y)*(ponto.y - proj.y)));
}

void getPoints(Imagem *img, int y, int x, Header *head){

    if(img->dados[0][y][x] == 1.0f && checkNodeByValue(head, y, x) == 0){ // atual
        Dot temp;
        temp.x = x;
        temp.y = y;
        temp.dist = 0.0f;
        insertNodeAtTheEnd(head, temp);
        //printf(" \n White node [0][%d][%d] \n", y, x);

        if(img->dados[0][y][x+1] == 1.0f) { // direita
            getPoints(img, y, x+1, head);
        }

        if(img->dados[0][y+1][x] == 1.0f) { // baixo
            getPoints(img, y+1, x, head);
        }

        if(img->dados[0][y-1][x] == 1.0f) { // cima
            getPoints(img, y-1, x, head);
        }

        if(img->dados[0][y][x-1] == 1.0f) { // esquerda
            getPoints(img, y, x-1, head);
        }
    }
}

void calculaDistancias(Header *head){
    Node *aux;
    int c;
    aux = head->head;

    for (c = 1; aux->next != head->tail; aux = aux->next, c++) {
    
        //aux->dot.dist = shortestDistance(aux->dot, aux->next->next->dot, aux->next->dot);        
        aux->dot.dist = shortestDistanceTo(aux->dot, aux->next->next->dot, aux->next->dot);        

        printf("\n Distancia de C ate AB: A[%d][%d] B[%d][%d] C[%d][%d]\n", aux->dot.y, aux->dot.x, aux->next->next->dot.y, aux->next->next->dot.x, aux->next->dot.y, aux->next->dot.x);  

        if(aux->dot.dist > 0.0f){
          //printf("\n NodeCount: %d current: %d dist: %f \n", head->nodeCount, c, aux->dot.dist);  
        }
		//printf("\n NodeCount: %d current: %d dist: %f \n", head->nodeCount, c, aux->dot.dist);        
	}
    head->tail->dot.dist = shortestDistanceTo(head->tail->dot, head->head->next->dot, head->head->dot);        

}

double removeLowValues(Header *head){
    Node *aux;
    aux = head->head;
    double lowestDist = 999999.0f;
    int lowestIdx = 0;

    for (int i = 0; i < head->nodeCount; i++, aux = aux->next) {
        if(aux->dot.dist < lowestDist) {
            lowestDist = aux->dot.dist;
            lowestIdx = i; 
        }            
	}

    //printf("\n Deleting %d with the value %f \n", lowestIdx, lowestDist);
    deleteDotAtN(head, lowestIdx);

    return lowestDist;
}


void lineFromPoints(Dot Q, Dot P) 
{ 
    float a = Q.y - P.y; 
    float b = P.x - Q.x; 
    float c = a*(P.x) + b*(P.y); 
  

} 

double shortestDistance(Dot a,Dot b,Dot c)
    {
        //printf("\n A -> [%d][%d], B -> [%d][%d], C -> [%d][%d]", a.y, a.x, b.y,b.x,c.y,c.x);


        float px=b.x-a.x;
        float py=b.y-a.y;
        float temp=(px*px)+(py*py);
        float u=((c.x - a.x) * px + (c.y - a.y) * py) / (temp);
        if(u>1){
            u=1;
        }
        else if(u<0){
            u=0;
        }
        float x = a.x + u * px;
        float y = a.y + u * py;

        float dx = x - c.x;
        float dy = y - c.y;
        double dist = sqrt(dx*dx + dy*dy);
        return dist;

    }