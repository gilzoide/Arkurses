/** @file movimento.h
 * Definições do movimento da barrinha e da bolinha, assim como colisão
 */
#ifndef MOVIMENTO_H
#define MOVIMENTO_H

#include "arkurses.h"

void MoveBarraEsq ();	// mexe a
void MoveBarraDir ();	// barrinha

void MoveBolinha ();		// movimento
void AndaX (int y, int x);	// da
void AndaL (int y, int x);	// bolinha
void MoveUltimo ();	// move o último bloquinho

char AlgoNoCaminhoBarra (int y, int x);	// vai trombar na barra?
char AlgoNoCaminhoCampo (int y, int x);	// ou talvez nos blocos?
char AlgoNoCaminhoBola (int y, int x);	// e o último bloco, bate na bola?
void Quebra (char obst, int y, int x);	// se sim, que bom, então quebra lá

#endif
