#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pdi.h"

#define ORIGINAL "./images/letra_s.bmp"
#define JANELA 3


double shortestDistance(Dot a,Dot b,Dot c);

int main() {

    Imagem *imagem;
    int y,x;

    Header head;

	head.head = NULL;
	head.tail = NULL;
	head.nodeCount = 0;

    Header headAux;

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

    int whiteCount = 0;
    int threshold = (((JANELA*JANELA))/4)*2;
    

    printf("\n getting possible points of interest \n");
    for(x=0;x<imagem->largura;x++){
        for(y=0;y<imagem->altura;y++){
            if(imagem->dados[0][y][x] == 0.0f){
                
                whiteCount = 0; // zera contagem de vizinhos brancos

                // percorre vizinhança procurando vizinhos brancos, só os pontos mais da borda nos interessam
                for(int aux_y = y-JANELA, flag = 0; aux_y < y+JANELA && flag == 0; aux_y++){
                    for(int aux_x = x-JANELA; aux_x < x+JANELA && flag == 0; aux_x++){
                        if(imagem->dados[0][aux_y][aux_x] == 1.0f && aux_y != y && aux_x != x){
                            whiteCount++;
                        }
                    }
                }

                // se enquadrar vai pra lista
                if(whiteCount > threshold){
                    //printf("\n [%d][%d] vizinhos brancos: %d \n", y,x, whiteCount);
                    Dot temp;
                    temp.x = x;
                    temp.y = y;
                    insertNodeAtTheEnd(&head, temp);
                }else {
                    imagem->dados[0][y][x] == 1.0f;
                }

            }
        }
    }

    printf("\n list size b4: %d\n", head.nodeCount);

    printf("\n doing distances \n");

    
    double dist = 0;
    int max = 100;
    for(int idx = 0;idx < head.nodeCount-100; idx+=100){
        for(int aux = idx; aux < idx+99; aux++){
            
            
            if(aux >= head.nodeCount - 100){
                max = head.nodeCount-aux -1;
            }else {
                max = 100;
            }
            dist = shortestDistance(getDotAtN(&head, idx), getDotAtN(&head, aux+max), getDotAtN(&head, aux));
            if(dist < 5.0){
                insertNodeAtTheEnd(&headAux,getDotAtN(&head, aux));
            }
        }
    }

    Imagem *teste = criaImagem(imagem->largura, imagem->altura, imagem->n_canais);

    for(int idx = 0;idx < head.nodeCount; idx++){
        Dot ponto = getDotAtN(&head, idx);
        teste->dados[0][(int)ponto.y][(int)ponto.x] = 1.0f;
    }

    printf("\n list size after: %d\n", headAux.nodeCount);


    //double derp = shortestDistance(getDotAtN(&head, 1), getDotAtN(&head, 500), getDotAtN(&head, 50));



    salvaImagem(teste, "teste.bmp");
    destroiImagem(imagem);
    destroiImagem(teste);

    return 0;
}


void lineFromPoints(Dot Q, Dot P) 
{ 
    float a = Q.y - P.y; 
    float b = P.x - Q.x; 
    float c = a*(P.x) + b*(P.y); 
  

} 

double shortestDistance(Dot a,Dot b,Dot c)
    {
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