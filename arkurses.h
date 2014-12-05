/** @file arkurses.h
 * Variáveis globais e macros, pra geral manjar
 */
#ifndef ARKURSES_H
#define ARKURSES_H

/*#define FASEDOIS*/
/*#define PULAHIST*/

#include <curses.h>
#include <panel.h>
#include <stdlib.h>	// para o rand ()/srand ()
#include <time.h>	// time (), pra por no srand ()
#include <ctype.h>
#include <locale.h>	// pra chars doidos [ê, ç]

#define BGhelp 10
#define FGshot 4
#define FGboss 3
#define HELP_WIDTH 45

#define LINES 24

#define CAMPO_ALTURA 22
#define CAMPO_LARGURA 47
#define CAMPO_y0 (LINES/2 - CAMPO_ALTURA/2)
#define CAMPO_x0 (COLS/2 - CAMPO_LARGURA/2)
#define BLOCO_y0 (CAMPO_y0 + 3)

#define BARRA_y0 (LINES/2 + CAMPO_ALTURA/2 - 2)
#define BARRA_x0 (COLS/2 - 1)

#define BOLA_y0 (BARRA_y0 - 1)
#define BOLA_x0 (COLS/2)

#define FALA_y0 (BARRA_y0 + 2)
#define FALA_x0 CAMPO_x0

/// Movimentos do chefão necessários para se ganhar um tiro
#define SHOT_TIME 150

WINDOW *hud, *campo, *barra, *bola, *chefe, *HP_chefe;
int vidas;
int numblocos;	// número de blocos: 15 vezes o número de linhas de blocos [dificuldade]

char movimento;	// tipo do movimento: X para 45°, L para 31° (anda em L, como no xadrez)
char h_dir;	// direção horizontal do movimento: E para esquerda e D para direita
char v_dir;	// direção vertical do movimento: C para cima e B para baixo
char l_mov;	// movimento em L: alterna entre 'H' para horizontal e 'D' para diagonal

int y_ultim, x_ultim;	// coordenadas do último bloquinho
char dificuldade;	// número de linhas de bloquinhos

int s;	// escolhas do teclado
int periodo;	// tempim entre os frames: 10~20

int vida_chefe;	///< vida do chefe: número de vezes que ainda falta bater nele
/* coordenadas do chefe */
int y_chefe,	///< posição Y do chefe
	x_chefe;	///< posição X do chefe

unsigned char tiro;	///< Contagem pra tiro, em função de SHOT_TIME
/* Coordenadas do tiro */
int y_tiro,		///< posição Y do tiro
	x_tiro;		///< posição X do tiro

#endif
