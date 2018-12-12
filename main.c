#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "pdi.h"

#define ORIGINAL "./images/qg.bmp"
#define SCALE 0.25f

double shortestDistance(Dot a,Dot b,Dot c);
void getPoints(Imagem *img, int y, int x, Header *head);
void calculaDistancias(Header *head);
double removeLowValues(Header *head);
float shortestDistanceTo(Dot pt1, Dot pt2, Dot ponto);

int main() {

    Imagem *imagem, *erosao, *kernel, *bordas, *final;
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
    final = criaImagem(imagem->largura, imagem->altura, imagem->n_canais);

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


    /*for(int idx = 0; idx< 1100; idx++){
        calculaDistancias(&head);
        removeLowValues(&head);
    }*/

    calculaDistancias(&head);

    //printList(&head);
    
    double removed = 0.0f;
    do{
        calculaDistancias(&head);
        removed = removeLowValues(&head);
    }while(removed == 0.0f);

    printList(&head);

    Imagem *teste = criaImagem(imagem->largura, imagem->altura, imagem->n_canais);

    for(int idx = 0;idx < head.nodeCount; idx++){
        Dot ponto = getDotAtN(&head, idx);
        teste->dados[0][(int)ponto.y][(int)ponto.x] = 1.0f;
    }

    printf("\nGetting object limits... \n");

    int hx = 0, lx=0, hy=0, ly=0;
    Node *node;

    for(int idx = 0;idx < head.nodeCount; idx++){
        node = getNodeAtN(&head, idx);
        
        if(node->dot.x > hx)
            hx = node->dot.x;        
        else if(node->dot.x < lx)
            lx = node->dot.x;
        if(node->dot.y > hy)
            hy = node->dot.y;
        else if(node->dot.y < ly)
            ly = node->dot.y;
    }  

    Imagem *no_res = criaImagem(imagem->largura, imagem->altura, imagem->n_canais);

    Coordenada p1, p2;
    Cor cor = criaCor(1.0f, 1.0f, 1.0f);

    for(int idx = 0;idx < head.nodeCount-1; idx++){
        node = getNodeAtN(&head, idx);
        p1.x = node->dot.x;
        p1.y = node->dot.y;
        p2.x = node->next->dot.x;
        p2.y = node->next->dot.y;
        line(p1.x, p1.y, p2.x,p2.y,no_res);
    }

    p1.x = head.tail->dot.x;
    p1.y = head.tail->dot.y;
    p2.x = head.head->dot.x;
    p2.y = head.head->dot.y;

    line(p1.x, p1.y, p2.x,p2.y,no_res); 


    printf("\nResizing Object... \n");

    int x_axis = hx - lx, y_axis = hy - ly;

    printf("\nx_axis %d y_axis %d\n", x_axis, y_axis);

    for(int idx = 0;idx < head.nodeCount; idx++){
        node = getNodeAtN(&head, idx);
        if(node->dot.x > x_axis/2)
            node->dot.x = node->dot.x * (SCALE + 1.0f);
        else
            node->dot.x = node->dot.x * (1.0f - SCALE);

        if(node->dot.y > y_axis/2)
            node->dot.y = node->dot.y * (SCALE + 1.0f);
        else
            node->dot.y = node->dot.y * (1.0f - SCALE);
        
    }


    printf("\nReconstruindo Imagem... \n");

    
    for(int idx = 0;idx < head.nodeCount-1; idx++){
        node = getNodeAtN(&head, idx);
        p1.x = node->dot.x;
        p1.y = node->dot.y;
        p2.x = node->next->dot.x;
        p2.y = node->next->dot.y;
        line(p1.x, p1.y, p2.x,p2.y,final);
    }

    p1.x = head.tail->dot.x;
    p1.y = head.tail->dot.y;
    p2.x = head.head->dot.x;
    p2.y = head.head->dot.y;

    line(p1.x, p1.y, p2.x,p2.y,final);

    salvaImagem(final, "final.bmp");
    destroiImagem(final);

    salvaImagem(no_res, "no_res.bmp");
    destroiImagem(no_res);

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

void line(int x0, int y0, int x1, int y1, Imagem *img) {
 
  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;){
    img->dados[0][y0][x0] = 1.0f;
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
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
        aux->next->dot.dist = shortestDistanceTo(aux->dot, aux->next->next->dot, aux->next->dot);        

        // printf("\n Distancia de C ate AB: A[%d][%d] B[%d][%d] C[%d][%d]", aux->dot.y, aux->dot.x, aux->next->next->dot.y, aux->next->next->dot.x, aux->next->dot.y, aux->next->dot.x);  

        if(aux->dot.dist > 0.0f){
            //printf("\n NodeCount: %d current: %d dist: %f \n", head->nodeCount, c, aux->dot.dist);  
        }
		  //  printf("\n NodeCount: %d current: %d dist: %f \n", head->nodeCount, c, aux->dot.dist);        
	}
    head->tail->prev->dot.dist = shortestDistanceTo(head->tail->prev->dot, head->head->dot, head->tail->dot);
    head->tail->dot.dist = shortestDistanceTo(head->tail->dot, head->head->next->dot, head->head->dot);        
}

double removeLowValues(Header *head){
    Node *aux, temp;
    aux = head->head;
    double lowestDist = 999999.0f;
    int lowestIdx = 0;

    for (int i = 0; i < head->nodeCount; i++, aux = aux->next) {
        if(aux->dot.dist < lowestDist && aux->dot.dist < 1.0f) {
            lowestDist = aux->dot.dist;
            lowestIdx = i; 
        }            
	}

    if(lowestDist != 999999.0f){
        printf("\n Deleting %d with the value %f \n", lowestIdx, lowestDist);
        
        //temp = getNodeAtN(head, lowestIdx);
        deleteDotAtN(head, lowestIdx);
    }

    return lowestDist;
}