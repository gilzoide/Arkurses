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
#define HELP_WIDTH 36

#define LINES 24

#define SHOT_TIME 150	// boss moves for player to get a shot

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

void InitCurses ();

void Help ();	// displays the help
void Restart ();	// ask for restarting the game, and does it if needed
void Pause ();	// pause the game
void AtualizaHud ();	// acho que você já sabe o que faz, né?

void CriaBlocos ();	// inicializadores
void CriaBarra ();	// dos
void CriaBola ();	// gráficos
void CriaCampo ();	// do jogo

void MoveBarraEsq ();	// mexe a
void MoveBarraDir ();	// barrinha

void MoveBolinha ();		// movimento
void AndaX (int y, int x);	// da
void AndaL (int y, int x);	// bolinha
void MoveUltimo ();	// move o último bloquinho
void MoveChefe ();	// move o chefe da segunda [e última] fase

char AlgoNoCaminhoBarra (int y, int x);	// vai trombar na barra?
char AlgoNoCaminhoCampo (int y, int x);	// ou talvez nos blocos?
void Quebra (char obst, int y, int x);	// se sim, que bom, então quebra lá
char AlgoNoCaminhoBola (int y, int x);	// e o último bloco, bate na bola?

void FogoArtificio (int x);	// um fogo de artifício [que lindo ^^]
void Morreu ();		// vai que ele morre, né?
void FaseDois ();	// ou passa do começo?
void Ganhou ();		// ou até ganha o jogo!
int BateChefe (int y, int x);	// bateu no chefão; porrada, manow!
void AtualizaVidaChefe ();

void Shoot (char*);	// todas as funcoes sobre o tiro
void ClickShoot ();


WINDOW *hud, *campo, *barra, *bola, *chefe, *HP_chefe;
int vidas;
int vida_chefe;	// vida do chefe: número de vezes que ainda falta bater nele
int numblocos;	// número de blocos: 15 vezes o número de linhas de blocos [dificuldade]

char movimento;	// tipo do movimento: X para 45°, L para 31° (anda em L, como no xadrez)
char h_dir;	// direção horizontal do movimento: E para esquerda e D para direita
char v_dir;	// direção vertical do movimento: C para cima e B para baixo
char l_mov;	// movimento em L: alterna entre 'H' para horizontal e 'D' para diagonal

int y_ultim, x_ultim;	// coordenadas do último bloquinho
int y_chefe, x_chefe;	// coordenadas do chefe
char dificuldade;	// número de linhas de bloquinhos

int s;	// escolhas do teclado
int periodo;	// tempim entre os frames: 10~20
unsigned char tiro;	// contagem pra tiro: <SHOT_TIME, conta;		SHOT_TIME, disponivel;		>SHOT_TIME, atirou
int y_tiro, x_tiro;	// coordenadas do tiro

