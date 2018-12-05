/*============================================================================*/
/* TIPOS E FUN��ES B�SICOS                                                    */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Tipos e fun��es b�sicos, �teis para diversos problemas envolvendo
 * manipula��o de imagens. */
/*============================================================================*/

#ifndef __BASE_H
#define __BASE_H

/*============================================================================*/

#include "imagem.h"

/* O b�sico do b�sico... */
#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)
unsigned char float2uchar (float x);

void soma (Imagem* in1, Imagem* in2, float mul1, float mul2, Imagem* out);

/* Normaliza��o */
void normaliza (Imagem* in, Imagem* out, float min, float max);
void normalizaSemExtremos8bpp (Imagem* in, Imagem* out, float min, float max, float descartados);
void normLocalSimples (Imagem* in, Imagem* out, float min, float max, int largura);

/* Histogramas */
void criaHistograma8bpp1c (Imagem* in, int canal, int histograma [256]);
void criaHistograma8bpp1cNorm (Imagem* in, int canal, float histograma [256]);

/*============================================================================*/
#endif /* __BASE_H */
