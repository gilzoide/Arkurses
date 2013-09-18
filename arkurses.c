#define FASEDOIS
#define PULAHIST

/*
 *						Arkanoid versus Ncurses: ARKURSES
 *
 *		Sinopse: Você está dentro do personagem de uma barra. Uma barra feita de
 *	caracteres! O mundo está sendo atacado por terríveis bloquinhos coloridos do
 *	mal! Direcione a bolinha aos blocos e derrote todos eles, salvando o mundo
 *	das garras da guangue dos "MenorIgualMaior".
 *
 *		Regras: Primeiramente, digite o número de linhas de tropas de bloquinhos
 *	para batalhar (entre 1 e 8). Mova a barra com as setinhas. Tecla '+' ou '='
 *	aumentam a velocidade da bolinha, e '-' a diminui. A bolinha move-se
 *	em 45° ao trombar no meio da barra e em L ao colidir com as pontas '<'/'>'.
 *	O jogo pode ser pausado com a barra de espaço, e pode-se sair a qualquer hora
 *	apertando a tecla 'q'. Clique F2 para opção "novo jogo", e 's' para confirmar;
 *	'n' volta para o jogo e 'q' ainda sai do jogo.
 *
 *	Colisões e inversões de sentido:
 *		em 45°:
 *			p/ Esquerda com canto direito '>': inversão em ambas as direções;
 *			p/ Direita com canto esquerdo '<': inversão em ambas as direções;
 *			p/ Esquerda com resto '='/'<': inverte direção vertical;
 *			p/ Direita com resto '='/'>': inverte direção vertical;
 *		em L:
 *			na horizontal: inverte sentido horizontal;
 *			diagonal em qualquer sentido horizontal, com bloco em geral: inverte direção vertical;
 *
 *	Algumas considerações:
 *		-Compilando no gcc? Não esqueça de adicionar a flag "-lpanel -lncurses" [senão não funfa];
 *		-Sim, esse bagulho roda nos terminais virtuais [tty] [é até melhor de jogar];
 *		-Esse jogo buga bastante;
 *		-Não existe tal coisa como física;
 *		-Não reclame da vida.
 *
 *	Hell, yeah
 *	Gil Barbosa Reis
 *
 */


#include <ncurses.h>
#include <panel.h>
#include <stdlib.h>	// para o rand ()/srand ()
#include <time.h>	// time (), pra por no srand ()
#include <ctype.h>
#include <locale.h>	// pra chars doidos [ê, ç]

#define BGhelp 10
#define HELP_WIDTH 36

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
void GanhaTiro ();
void Shoot ();


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



int main () {
	int i;

	setlocale (LC_ALL, "");	// para aparecer os chars doidos
	srand (time (NULL));

// inicializações do curses
	initscr ();	// inicia o modo curses
	start_color (); // cores ;]
	keypad (stdscr, TRUE); // permite uso de 'F's e setinhas
	init_pair (1, COLOR_WHITE, COLOR_GREEN);	// cor do HUD
	init_pair (2, COLOR_CYAN, COLOR_BLACK);		// outras
	init_pair (3, COLOR_RED, COLOR_BLACK);		// cores;
	init_pair (4, COLOR_YELLOW, COLOR_BLACK);	// para
	init_pair (5, COLOR_BLUE, COLOR_BLACK);		// os
	init_pair (6, COLOR_MAGENTA, COLOR_BLACK);	// bloquinhos
	init_pair (7, COLOR_GREEN, COLOR_BLACK);	//
	init_pair (8, COLOR_WHITE, COLOR_BLACK);	// e uma pra barrinha e pra bolinha [e pra bloco também, uai]
	init_pair (9, COLOR_BLACK, COLOR_BLACK);	// e mais uma para os bloquinhos [wow! que tanto!]
	init_pair (BGhelp, COLOR_WHITE, COLOR_BLUE);	// help color
	bkgd (COLOR_PAIR (8));
	
	hud = subwin (stdscr, 1, 0, 0, 0);			// cria o HUD
	wbkgd (hud, COLOR_PAIR (1));				// com o nome do
	wattron (hud, A_BOLD);						// jogo e quantas
	mvwaddstr (hud, 0, COLS/2 - 4, "ARKURSES");	// vidas tem e quantos
	wrefresh (hud);

#ifndef FASEDOIS
	mvaddstr (6, 0, "Dificulty [1~8] >");
	do {
		mvscanw (6, 18, "%d", &dificuldade);
	} while (dificuldade < 1 || dificuldade > 8);
	move (6, 0);
	clrtoeol ();
#endif
#ifdef FASEDOIS
	dificuldade = 1;
#endif

	cbreak (); // não espera a tecla 'enter'
	noecho (); // não escreve as teclas apertadas, para interatividades
	curs_set (0); // esconde o cursor
	
	mvwaddstr (hud, 0, 0, "'?': Help");
	AtualizaHud ();

	int frame;	// frame em que está: para mover a bolinha e o último bloco com velocidades diferentes
	char tiro = 0;
	while (s != 'q') {
		CriaCampo ();				// novo jogo:
		CriaBlocos ();
		CriaBarra ();				// cria as coisinhas
		CriaBola ();				// em seu devido lugar
#ifndef FASEDOIS
		vidas = 5;					// e também seu
		numblocos = 15*dificuldade;	// valor inicial
#endif
#ifdef FASEDOIS
		numblocos = 2;
		vidas = 15;
#endif
		periodo = 14;
		vida_chefe = -1;	// -1 for "not living yet" [0 is for dead]
		movimento = 'X';
		v_dir = 'C';
		h_dir = 'D';
		l_mov = 'H';

		AtualizaHud ();
		frame = 0;
		s = 0;
		nodelay (stdscr, FALSE);	// começa jogo só se 
		getch ();					// clicar alguma coisa
		nodelay (stdscr, TRUE);	// não espera o getch(), pra jogar mesmo

		while (s != KEY_F(2) && s != 'q') {
			if ((s = tolower (getch ())))
				flushinp ();
			
// último bloquinho: falas e ele começa a mexer
			if (numblocos == 1 && frame % 5 == 0)
				MoveUltimo ();

// fase dois [depois de destruir todos os blocos]: chefe loko
			else if (vida_chefe > 0 && frame % 150 == 0) {
				MoveChefe ();
				// ganha tiro depois dum tempo
				if (tiro == 30)
					GanhaTiro ();
					
				tiro++;
			}

// mexe a bolinha
			if (frame % 7 == 0)
				MoveBolinha ();

			switch (s) {
				case '?':
					Help ();
					break;
					
				case KEY_LEFT: case 'a':
					MoveBarraEsq ();
					break;
				case KEY_RIGHT: case 'd':
					MoveBarraDir ();
					break;
				
				// tiro, pro chefe
				case KEY_UP: case 'w':
					if (tiro >= 30)
						Shoot ();
					break;
					
// aumenta a velocidade ['=' para quem usa notebook sem teclado numérico e não quer segurar o shift, que nem eu]
				case '+': case '=':
					if (periodo > 10 && periodo <= 30)
						periodo -= 2;
					break;
// diminui a velocidade
				case '-':
					if (periodo >= 10 && periodo < 30)
						periodo += 2;
					break;
					
// jogador pausou → barra de espaço
				case ' ':
					Pause ();
					if (s != KEY_F(2))
						break;
						
// em caso de novo jogo [F2] (ou perdeu o jogo, ou ganhou o jogo):
				case KEY_F(2):
					Restart ();
					break;
			}
			movimento == 'X' ? napms (periodo) : napms (periodo*0.89);
// próximo frame
			frame++;
		}
	}

	endwin ();
	return 0;
}




/* Displays the help (in a created window and panel, for going back to the normal field after) */
void Help () {
	WINDOW *help;
	PANEL *up;

	help = newwin (8, HELP_WIDTH, 1, 0);
	up = new_panel (help);
	update_panels ();
	doupdate ();

	box (help, 0, 0);
	wbkgd (help, COLOR_PAIR (BGhelp));
	wrefresh (help);
	mvwaddstr (help, 0, HELP_WIDTH/2 - 2, "HELP");
	wattron (help, A_BOLD);
	mvwaddstr (help, 1, 1, "Arrow Keys or A,D:");
	mvwaddstr (help, 2, 1, "'-':");
	mvwaddstr (help, 3, 1, "'+' or '=':");
	mvwaddstr (help, 4, 1, "Space:");
	mvwaddstr (help, 5, 1, "F2:");
	mvwaddstr (help, 6, 1, "Q:");

	wattrset (help, COLOR_PAIR (BGhelp));
	mvwaddstr (help, 1, 20, "move left/right");
	mvwaddstr (help, 2, 6, "slow down");
	mvwaddstr (help, 3, 13, "speed up");
	mvwaddstr (help, 4, 8, "pause");
	mvwaddstr (help, 5, 5, "reset game");
	mvwaddstr (help, 6, 4, "quit");

// writes the help window, wait for some key to be pressed and delete the help window
	wrefresh (help);
	nodelay (stdscr, FALSE);
	getch ();
	nodelay (stdscr, TRUE);

	wbkgd (help, COLOR_PAIR (0));
	werase (help);
	wrefresh (help);
	del_panel (up);
	delwin (help);
}


/* Pause the game, waiting for unpause/quit/reset game */
void Pause () {
	WINDOW *pause = newwin (1, COLS, FALA_y0, 0);
	PANEL *up = new_panel (pause);
	
	wattron (pause, A_BOLD);
	mvwaddstr (pause, 0, COLS/2 - 2, "PAUSE");
	
	update_panels ();
	doupdate ();
	
	nodelay (stdscr, FALSE);
	do {
		s = tolower (getch ());
	} while (s != ' ' && s != 'q' && s != KEY_F(2));
// jogador despausou, ou pediu novo jogo [taí embaixo], ou mandou sair
	nodelay (stdscr, TRUE);
	werase (pause);
	wrefresh (pause);
	del_panel (up);
	delwin (pause);
}


/* Restart the curses windows, for when restarting the game */
void Restart () {
	WINDOW *new = newwin (1, COLS, FALA_y0, 0);
	PANEL *up = new_panel (new);
	
	wattron (new, A_BOLD);
	mvwaddstr (new, 0, COLS/2 - 9, "NOVO JOGO? (y/n/q)"); // y: sim; n: não; q: sair (quit)
	
	update_panels ();
	doupdate ();

	nodelay (stdscr, FALSE);
// pega tecla até uma das opções válidas
	do {
		s = tolower (getch ());
	} while (s != 'y' && s != 'n' && s != 'q');
// em caso de fim de jogo (perdendo ou ganhando), escolher 'n' sai do jogo
	if ((vidas == 0 || vida_chefe == 0) && s == 'n')
		s = 'q';
// sim? então exorcisa a bola, barrinha e campo, e volta lá refazer o jogo
	else if (s == 'y') {
		werase (bola);
		delwin (bola);
		werase (barra);
		delwin (barra);
		werase (campo);
		delwin (campo);
		werase (HP_chefe);
		delwin (HP_chefe);
		
		standend ();
		mvaddstr (FALA_y0, COLS/2 - 9, "                  ");
		s = KEY_F(2);
	}
// clear the new game window/panel
	nodelay (stdscr, TRUE);
	werase (new);
	wrefresh (new);
	del_panel (up);
	delwin (new);
}


/* reescreve quantas vidas tem e quantos blocos faltam */
void AtualizaHud () {
	mvwprintw (hud, 0, COLS - 21, "vidas: %d  blocos: %3.d", vidas, numblocos);
	wrefresh (hud);
}


/* Cria o campo, com sua caixinha bonitinha */
void CriaCampo () {
	campo = subwin (stdscr, CAMPO_ALTURA, CAMPO_LARGURA, CAMPO_y0, CAMPO_x0);
	wattron (stdscr, COLOR_PAIR (8));
	box (campo, 0, 0);
}


/* Desenha os blocos na tela, na posição certa, uma linha de cada cor */
void CriaBlocos () {
	int cor = 2, x, y;
	
	wattron (campo, A_BOLD);
	for (y = 0; y < dificuldade; y++) {
		wattron (campo, COLOR_PAIR (cor));
// desenha os 15 blocos de 3 chars, no formato especificado
		for (x = 0; x < 15; x++)
			mvwaddstr (campo, y + 3, (3*x) + 1, "<=>");
// muda a cor pra próxima linha
		cor++;
	}
	wrefresh (campo);
}


/* Cria a barra, na posição de início */
void CriaBarra () {
	barra = subwin (stdscr, 1, 4, BARRA_y0, BARRA_x0);
	mvwaddstr (barra, 0, 0, "<xx>");
	wrefresh (barra);
}


/* Cria a bolinha, na posição de início */
void CriaBola () {
	bola = subwin (stdscr, 1, 1, BOLA_y0, BOLA_x0);
	mvwaddch (bola, 0, 0, 'O');
	wrefresh (bola);
}


/* Move a barrinha uma casa pra esquerda */
void MoveBarraEsq () {
	int x, y; // coordenadas atuais da barra (contadas a partir do '<')
	getbegyx (barra, y, x);

	if (x > CAMPO_x0 + 1) {
		x--;
// não tromba na bolinha
		if (AlgoNoCaminhoBola (y - CAMPO_y0, x - CAMPO_x0) != 'O') {
			werase (barra);					// apaga a barra
			wrefresh (barra);
			mvwin (barra, y, x);			// move-a
			mvwaddstr (barra, 0, 0, "<xx>");// e a reescreve
			wrefresh (barra);
		}
	}
}


/* Move a barrinha uma casa pra direita */
void MoveBarraDir () {
	int x, y; // coordenadas atuais da barra (contadas a partir do '<')
	getbegyx (barra, y, x);

	if (x < CAMPO_x0 + CAMPO_LARGURA - 5) {
		x++;
// não tromba na bolinha
		if (AlgoNoCaminhoBola (y - CAMPO_y0, x - CAMPO_x0 + 3) != 'O') {
			werase (barra);					// apaga a barra
			wrefresh (barra);
			mvwin (barra, y, x);			// move-a
			mvwaddstr (barra, 0, 0, "<xx>");// e a reescreve
			wrefresh (barra);
		}
	}
}


/* Move a bolinha, levando em consideração o tipo do movimento */
void MoveBolinha () {
	int y, x;	// coordenadas da bolinha

	getbegyx (bola, y, x);	// onde está a bola?

// tá na linha da barra, morreu
	if (y == BARRA_y0) {
		Morreu ();
// se acabaram todas as vidas, nem mexe a bolinha
		if (vidas == 0)
			return;
		v_dir = 'C';	// e volta a ir pra cima
	}
// inversão de sentido horizontal → lateral esquerda
	if (x == CAMPO_x0 + 1)
			h_dir = 'D';
// lateral direita
	if (x == CAMPO_x0 + CAMPO_LARGURA - 2)
		h_dir = 'E';
// inversão de sentido vertical → teto
	if (y == CAMPO_y0 + 1)
			v_dir = 'B';

	switch (movimento) {
		case 'X': AndaX (y, x); break;
		case 'L': AndaL (y, x); break;
	}
}


/* Anda em 45°: 1×1 */
void AndaX (int y, int x) {
	char obst;
	
	if (v_dir == 'C')
		switch (h_dir) {
			case 'D':
// próxima posição [se aplica a todos os movimentos]
				y--; x++;

// se estiver na 2ª fase, vê se bate com o chefão, daí não destroi bloquinho
				if (vida_chefe > 0 && BateChefe (y, x))
					return;

// prevê colisão com bloquinhos [se aplica a todos os movimentos]
				obst = AlgoNoCaminhoCampo (y, x);
				if (obst == '=' || obst == '>') {
					v_dir = 'B';
					Quebra (obst, y, x);
					return;
				}
				else if (obst == '<') {
					if (AlgoNoCaminhoCampo (y + 1, x) != '<')
						v_dir = 'B';
					if (AlgoNoCaminhoCampo (y, x - 1) != '>')
						h_dir = 'E';
// se bater no cantinho côncavo
					if (AlgoNoCaminhoCampo (y, x - 1) == '>' && AlgoNoCaminhoCampo (y + 1, x) == '<') {
						v_dir = 'B';
						h_dir = 'E';
						Quebra ('>', y, x - 1);
						Quebra ('<', y + 1, x);
						return;
					}
					Quebra (obst, y, x);
					return;
				}
				
				werase (bola);
				wrefresh (bola);
				mvwin (bola, y, x);
				mvwaddch (bola, 0, 0, 'O');
				wrefresh (bola);
				break;
			case 'E':
				y--; x--;

				if (vida_chefe > 0 && BateChefe (y, x))
					return;

				obst = AlgoNoCaminhoCampo (y, x);
				if (obst == '=' || obst == '<') {
					v_dir = 'B';
					Quebra (obst, y, x);
					return;
				}
				else if (obst == '>') {
					if (AlgoNoCaminhoCampo (y + 1, x) != '>')
						v_dir = 'B';
					if (AlgoNoCaminhoCampo (y, x + 1) != '<')
						h_dir = 'D';
					if (AlgoNoCaminhoCampo (y, x + 1) == '<' && AlgoNoCaminhoCampo (y + 1, x) == '>') {
						v_dir = 'B';
						h_dir = 'D';
						Quebra ('<', y, x + 1);
						Quebra ('>', y + 1, x);
						return;
					}
					Quebra (obst, y, x);
					return;
				}
				
				werase (bola);
				wrefresh (bola);
				mvwin (bola, y, x);
				mvwaddch (bola, 0, 0, 'O');
				wrefresh (bola);
				break;
		}
	else	// v_dir == 'B'
		switch (h_dir) {
			case 'D':
				y++; x++;
// bate ne barra?
				if (y == BARRA_y0) {
					obst = AlgoNoCaminhoBarra (y, x);
					if (obst == 'x') {
						v_dir = 'C';
						return;
					}
					else if (obst == '<') {
						v_dir = 'C';
						h_dir = 'E';
						movimento = 'L';
						return;
					}
					else if (obst == '>') {
						v_dir = 'C';
						movimento = 'L';
						return;
					}
				}

				if (vida_chefe > 0 && BateChefe (y, x))
					return;

// bate em bloquinho?
				obst = AlgoNoCaminhoCampo (y, x);
				if (obst == '=' || obst == '>') {
					v_dir = 'C';
					Quebra (obst, y, x);
					return;
				}
				else if (obst == '<') {
					if (AlgoNoCaminhoCampo (y - 1, x) != '<')
						v_dir = 'C';
					if (AlgoNoCaminhoCampo (y, x - 1) != '>')
						h_dir = 'E';
					if (AlgoNoCaminhoCampo (y, x - 1) == '>' && AlgoNoCaminhoCampo (y - 1, x) == '<') {
						v_dir = 'C';
						h_dir = 'E';
						Quebra ('>', y, x - 1);
						Quebra ('<', y - 1, x);
						return;
					}
					Quebra (obst, y, x);
					return;
				}

				
				werase (bola);
				wrefresh (bola);
				mvwin (bola, y, x);
				mvwaddch (bola, 0, 0, 'O');
				wrefresh (bola);
				break;
			case 'E':
				y++; x--;
				if (y == BARRA_y0) {
					obst = AlgoNoCaminhoBarra (y, x);
					if (obst == 'x') {
						v_dir = 'C';
						return;
					}
					else if (obst == '<') {
						v_dir = 'C';
						movimento = 'L';
						return;
					}
					else if (obst == '>') {
						v_dir = 'C';
						h_dir = 'D';
						movimento = 'L';
						return;
					}
				}

				if (vida_chefe > 0 && BateChefe (y, x))
					return;

				obst = AlgoNoCaminhoCampo (y, x);
				if (obst == '<' || obst == '=') {
					v_dir = 'C';
					Quebra (obst, y, x);
					return;
				}
				else if (obst == '>') {
					if (AlgoNoCaminhoCampo (y - 1, x) != '>')
						v_dir = 'C';
					if (AlgoNoCaminhoCampo (y, x + 1) != '<')
						h_dir = 'D';
					if (AlgoNoCaminhoCampo (y, x + 1) == '<' && AlgoNoCaminhoCampo (y - 1, x) == '>') {
						v_dir = 'C';
						h_dir = 'D';
						Quebra ('<', y, x + 1);
						Quebra ('>', y - 1, x);
						return;
					}
					Quebra (obst, y, x);
					return;
				}
				
				
				werase (bola);
				wrefresh (bola);
				mvwin (bola, y, x);
				mvwaddch (bola, 0, 0, 'O');
				wrefresh (bola);
				break;
		}
}


/* Anda em L: 1×2 → primeiro para o lado, e então diagonal */
void AndaL (int y, int x) {
	char obst;

// primeiro pro lado
	if (l_mov == 'H') {
		switch (h_dir) {
				case 'D':
					x++;
					
					if (vida_chefe > 0 && BateChefe (y, x))
						return;

					obst = AlgoNoCaminhoCampo (y, x);
					if (obst == '<') {
						h_dir = 'E';
						Quebra (obst, y, x);
						return;
					}
					
					werase (bola);
					wrefresh (bola);
					mvwin (bola, y, x);
					mvwaddch (bola, 0, 0, 'O');
					wrefresh (bola);
					break;
				case 'E':
					x--;

					if (vida_chefe > 0 && BateChefe (y, x))
						return;

					obst = AlgoNoCaminhoCampo (y, x);
					if (obst == '>') {
						h_dir = 'D';
						Quebra (obst, y, x);
						return;
					}
					
					werase (bola);
					wrefresh (bola);
					mvwin (bola, y, x);
					mvwaddch (bola, 0, 0, 'O');
					wrefresh (bola);
					break;
		}
		l_mov = 'D';
	}
// e então na diagonal
	else { 
		if (v_dir == 'C')
			switch (h_dir) {
				case 'D':
					y--; x++;

					if (vida_chefe > 0 && BateChefe (y, x))
						return;

					obst = AlgoNoCaminhoCampo (y, x);
					if (obst == '=' || obst == '>') {
						v_dir = 'B';
						Quebra (obst, y, x);
						return;
					}
					else if (obst == '<') {
						if (AlgoNoCaminhoCampo (y + 1, x) != '<')
							v_dir = 'B';
						else
							h_dir = 'E';
						if (AlgoNoCaminhoCampo (y, x - 1) == '>' && AlgoNoCaminhoCampo (y + 1, x) == '<') {
							v_dir = 'B';
							h_dir = 'E';
							Quebra ('>', y, x - 1);
							Quebra ('<', y + 1, x);
							return;
						}
						Quebra (obst, y, x);
						return;
					}
					
					werase (bola);
					wrefresh (bola);
					mvwin (bola, y, x);
					mvwaddch (bola, 0, 0, 'O');
					wrefresh (bola);
					break;
				case 'E':
					y--; x--;

					if (vida_chefe > 0 && BateChefe (y, x))
						return;

					obst = AlgoNoCaminhoCampo (y, x);
					if (obst == '<' || obst == '=') {
						v_dir = 'B';
						Quebra (obst, y, x);
						return;
					}
					else if (obst == '>') {
						if (AlgoNoCaminhoCampo (y + 1, x) != '>')
							v_dir = 'B';
						else
							h_dir = 'D';
						if (AlgoNoCaminhoCampo (y, x + 1) == '<' && AlgoNoCaminhoCampo (y + 1, x) == '>') {
							v_dir = 'B';
							h_dir = 'D';
							Quebra ('<', y, x + 1);
							Quebra ('>', y + 1, x);
							return;
						}
						Quebra (obst, y, x);
						return;
					}
					
					werase (bola);
					wrefresh (bola);
					mvwin (bola, y, x);
					mvwaddch (bola, 0, 0, 'O');
					wrefresh (bola);
					break;
			}
		else	// v_dir == 'B'
			switch (h_dir) {
				case 'D':
					y++; x++;
					
					if (y == BARRA_y0) {
						obst = AlgoNoCaminhoBarra (y, x);
						if (obst == 'x') {
							v_dir = 'C';
							movimento = 'X';
							return;
						}
						else if (obst == '<') {
							v_dir = 'C';
							h_dir = 'E';
							return;
						}
						else if (obst == '>') {
							v_dir = 'C';
							return;
						}
					}

					if (vida_chefe > 0 && BateChefe (y, x))
						return;

					obst = AlgoNoCaminhoCampo (y, x);
					if (obst == '=' || obst == '>') {
						v_dir = 'C';
						Quebra (obst, y, x);
						return;
					}
					else if (obst == '<') {
						if (AlgoNoCaminhoCampo (y - 1, x) != '<')
							v_dir = 'C';
						else
							h_dir = 'E';
						if (AlgoNoCaminhoCampo (y, x - 1) == '>' && AlgoNoCaminhoCampo (y - 1, x) == '<') {
							v_dir = 'C';
							h_dir = 'E';
							Quebra ('>', y, x - 1);
							Quebra ('<', y - 1, x);
							return;
						}
						Quebra (obst, y, x);
						return;
					}
					
					
					werase (bola);
					wrefresh (bola);
					mvwin (bola, y, x);
					mvwaddch (bola, 0, 0, 'O');
					wrefresh (bola);
					break;
				case 'E':
					y++; x--;
					
					if (y == BARRA_y0) {
						obst = AlgoNoCaminhoBarra (y, x);
						if (obst == 'x') {
							v_dir = 'C';
							movimento = 'X';
							return;
						}
						else if (obst == '<') {
							v_dir = 'C';
							return;
						}
						else if (obst == '>') {
							v_dir = 'C';
							h_dir = 'D';
							return;
						}
					}

					if (vida_chefe > 0 && BateChefe (y, x))
						return;

					obst = AlgoNoCaminhoCampo (y, x);
					if (obst == '<' || obst == '=') {
						v_dir = 'C';
						Quebra (obst, y, x);
					return;
					}
					else if (obst == '>') {
						if (AlgoNoCaminhoCampo (y - 1, x) != '>')
							v_dir = 'C';
						else
							h_dir = 'D';
						if (AlgoNoCaminhoCampo (y, x + 1) == '<' && AlgoNoCaminhoCampo (y - 1, x) == '>') {
							v_dir = 'C';
							h_dir = 'D';
							Quebra ('<', y, x + 1);
							Quebra ('>', y - 1, x);
							return;
						}
						Quebra (obst, y, x);
						return;
					}
					
					
					werase (bola);
					wrefresh (bola);
					mvwin (bola, y, x);
					mvwaddch (bola, 0, 0, 'O');
					wrefresh (bola);
					break;
			}
		l_mov = 'H';
	}
}


/* Move o último bloquinho, quando só faltar ele */
void MoveUltimo () {
	static char lado_ultim='D';	// direção horizontal do movimento do último bloquinho: E para esquerda e D para direita
	
// começo do campo, agora vai pra direita
	if (x_ultim == 1)
		lado_ultim = 'D';
// fim do campo, agora vai pra esquerda
	if (x_ultim == CAMPO_LARGURA - 4)
		lado_ultim = 'E';

// indo pra direita
	switch (lado_ultim) {
		case 'D':
// se o bloco trombar na bola, ele quebra, né
			if (AlgoNoCaminhoBola (y_ultim, x_ultim + 3) == 'O') {
				Quebra ('<', y_ultim + CAMPO_y0, x_ultim + CAMPO_x0);
				return;
			}
			mvwaddstr (campo, y_ultim, x_ultim, " <=>");
			wrefresh (campo);
			x_ultim++;
			break;
// indo pra esquerda
		case 'E':
			if (AlgoNoCaminhoBola (y_ultim, x_ultim - 1) == 'O') {
				Quebra ('<', y_ultim + CAMPO_y0, x_ultim + CAMPO_x0);
				return;
			}
			x_ultim--;
			mvwaddstr (campo, y_ultim, x_ultim, "<=> ");
			wrefresh (campo);
			break;
	}
}


/* Move o chefe da 2ª parte do jogo */
void MoveChefe () {
	char *cara[] = {
		"< > < >",
		"< 0 0 >",
		" <===> "
	};

// desdesenha-o
	int i;
	for (i = 0; i < 3; i++) {
		mvwaddstr (campo, y_chefe + i, x_chefe, "       ");
		wrefresh (campo);
	}
	
// onde está a bolinha?
	int y_bola, x_bola;
	getbegyx (bola, y_bola, x_bola);

// se a bolinha está numa linha do chefão, não move pra lá
	if (y_bola <= (y_chefe + CAMPO_y0) + 2)
		do {
			x_chefe = (rand () % (CAMPO_LARGURA - 8)) + 1;
		} while (x_bola - (x_chefe + CAMPO_x0) < 7 && x_bola - (x_chefe + CAMPO_x0) >= 0);
// mas se a bola tá pros otros lado, pode ir pra qualquer lugar
	else
		x_chefe = (rand () % (CAMPO_LARGURA - 8)) + 1;

// e desenha-o novamente
	for (i = 0; i < 3; i++)
		mvwaddstr (campo, y_chefe + i, x_chefe, cara[i]);
	wrefresh (campo);
}


/* prevê colisão com a barra */
char AlgoNoCaminhoBarra (int y, int x) {
	int y_bar, x_bar;

	getbegyx (barra, y_bar, x_bar);
// coordenada tela → coordenada barra
	y -= y_bar;
	x -= x_bar;

	return (mvwinch (barra, y, x));
}


/* prevê colisão com a bolinha [a partir de coordenadas do campo] */
char AlgoNoCaminhoBola (int y, int x) {
	int y_bola, x_bola;

	getbegyx (bola, y_bola, x_bola);
// bola: coordenada tela → coordenada campo
	y_bola -= CAMPO_y0;
	x_bola -= CAMPO_x0;
// coordenada campo → coordenada bola
	y -= y_bola;
	x -= x_bola;

	return (mvwinch (bola, y, x));
}


/* prevê colisão com os bloquinhos */
char AlgoNoCaminhoCampo (int y, int x) {
// coordenada tela → coordenada campo
	y -= CAMPO_y0;
	x -= CAMPO_x0;

	return (mvwinch (campo, y, x));
}


/* destrói o bloquinho, quando acertado */
void Quebra (char obst, int y, int x) {
	if (obst == '<') {
		mvwaddstr (campo, y - CAMPO_y0, x - CAMPO_x0, "   ");
		wrefresh (campo);
	}
	else if (obst == '=') {
		mvwaddstr (campo, y - CAMPO_y0, x - CAMPO_x0 - 1, "   ");
		wrefresh (campo);
	}
	else if (obst == '>') {
		mvwaddstr (campo, y - CAMPO_y0, x - CAMPO_x0 - 2, "   ");
		wrefresh (campo);
	}

// quebrou um bloco!
	numblocos--;
	AtualizaHud ();

// último bloquinho: falinhas [na main ele começa a mexer]
	if (numblocos == 1) {
		attron (A_BOLD);
		mvaddstr (BARRA_y0 + 2, COLS/2 - 5, "Finish him!");
		refresh ();
		napms (1000);
// onde está o último bloquinho?
		for (y_ultim = BLOCO_y0; y_ultim < BLOCO_y0 + dificuldade; y_ultim++) {
			for (x_ultim = CAMPO_x0 + 1; x_ultim < CAMPO_x0 + CAMPO_LARGURA - 3; x_ultim += 3)
				if (AlgoNoCaminhoCampo (y_ultim, x_ultim) == '<')
					break;
			if (AlgoNoCaminhoCampo (y_ultim, x_ultim) == '<')
				break;
		}
		
		mvaddstr (y_ultim - 1, x_ultim, "NO!");
		refresh ();
		napms (1000);
		mvaddstr (y_ultim - 1, x_ultim, "   ");
		refresh ();

// último: coordenadas tela → coordenadas campo
		y_ultim -= CAMPO_y0;
		x_ultim -= CAMPO_x0;
		
		wattron (campo, COLOR_PAIR (y_ultim - 1)); // continua com a cor dele mesmo
	}
	
// matou o último: passa pra próxima fase [chefão, VWAHAHAHAHA!]
	else if (numblocos == 0)
		FaseDois ();
}


/* morreu, diminui uma vida; não tem mais, recomeça o jogo [se quiser, claro] */
void Morreu () {
	static char i = 0;

	i++;
// andando em L, morre só uma vez
	if (movimento == 'L' && i == 1) {
		return;
	}
	i = 0;

	vidas--;
	AtualizaHud ();

// dá aquela piscadinha vermelha na bolinha, pra avisar que realmente morreu
	wbkgd (bola, COLOR_PAIR (3));
	wrefresh (bola);
	napms (300);
	wbkgd (bola, COLOR_PAIR (8));
	wrefresh (bola);

	if (vidas == 0) {
		attron (A_BOLD);
		for ( ; i < 4; i++) {
			mvaddstr (BARRA_y0 + 2, COLS/2 - 5, "FIM DE JOGO");
			refresh ();
			napms (600);
			mvaddstr (BARRA_y0 + 2, COLS/2 - 5, "           ");
			refresh ();
			napms (600);
		}
		attroff (A_BOLD);
		s = KEY_F(2);
	}
}


/* Solta um fogo de artifício, pra próxima funçãozinha aí =] */
void FogoArtificio (int x) {
	int y;

	for (y = BARRA_y0 - 1; y >= BLOCO_y0; y--) {
		mvaddch (y, x, '|');
		refresh ();
		napms (100);
		mvaddch (y, x, ' ');
		refresh ();
	}
	
	mvaddstr (y, x - 1,     "\\|/");
	mvaddstr (y + 1, x - 2, "--O--");
	mvaddstr (y + 2, x - 1,  "/|\\");
	refresh ();
	napms (400);
	mvaddstr (y, x - 1,      "   ");
	mvaddstr (y + 1, x - 2, "     ");
	mvaddstr (y + 2, x - 1,  "   ");
	refresh ();
}


/* Passou do começo [os bloco], historinha, e chefe "VWAHAHAHAHA" */
void FaseDois () {
	char *historia[] = {
		"Parabéns! Você derrotou o último bloquinho!",
		"E assim, a paz retornou ao mundo... Mas o quê?"
	}, *falas[] = {
		"Você achou que ganharia fácil assim?",
		"Você vai ver, sua barrinha troxa!",
		
		"Sua mãe é tão gorda, mas tão gorda,",
		"que ela precisa de 2 bytes pra cada caractere!",
		
		"Ainda acha que pode me derrotar? VWAHAHAHA!",
		"E aí, vai ficar parado ou o quê?",
		
		"Aliás, toma uma maldiçãozinha aí:",
		"quero ver você segurar essa bolinha agora!"
	};
	char *cara[] = {
		"< > < >",
		"< 0 0 >",
		" <===> "
	};
	
// apaga o "Finish Him!"
	mvaddstr (FALA_y0, COLS/2 - 5, "           ");

// tem uma chance em dificuldade de ganhar uma vida extra [ebaa! xD]
	if (rand() % dificuldade == 0) {
		attron (COLOR_PAIR (7));
		mvaddstr (1, COLS - 14, "+1");
		refresh ();
		napms (2000);
		vidas++;
		AtualizaHud ();
		mvaddstr (1, COLS - 14, "  ");
		refresh ();
		attroff (COLOR_PAIR (7));
	}

// apaga a bolinha, prela começar no lugar de começo normal [pro chefe não nascer com a bolinha dentro]
	werase (bola);
	wrefresh (bola);
	mvwin (bola, BOLA_y0, BOLA_x0);
	v_dir = 'C';
	h_dir = 'D'; 
	

	attroff (A_BOLD);
// fogos de artifício, primeiro no começo do campo
	int x = CAMPO_x0 + 4;
	int y;
#ifndef PULAHIST
	FogoArtificio (x);
// daí no cantinho direito
	x += 38;
	FogoArtificio (x);
// e lá no meio do campo
	x -= 19;
	FogoArtificio (x);

// historinha, frase por frase
	for (y = 0; y < 2; y++) {
		mvaddstr (FALA_y0, FALA_x0, historia[y]);
		refresh ();
		napms (2500);
		move (FALA_y0, 0);
		clrtoeol ();
	}
#endif
// cria a janela do chefe e a desenha pausadamente
	y_chefe = 1;
	x_chefe = (x - CAMPO_x0) - 2;
	wattron (campo, A_BOLD);
	wattron (campo, COLOR_PAIR (3));
	for (y = 0; y < 3; y++)
		for (x = 0; x < 7; x++) {
			mvwaddch (campo, y_chefe + y, x_chefe + x, cara[y][x]);
			wrefresh (campo);
			napms (300);
		}
#ifndef PULAHIST
// e mais linhas de fala do chefão
	for (y = 0; y < 8; y++) {
		mvaddstr (FALA_y0, FALA_x0, falas[y]);
		refresh ();
		napms (2500);
// ó a maldição!
		if (y == 6) {
			int i;
			for (i = 0; i < 3; i++) {
				for (x = 2; x < 10; x++) {
					wbkgd (barra, COLOR_PAIR (x));
					wrefresh (barra);
					napms (100);
				}
			}
			wbkgd (barra, COLOR_PAIR (8));
			wrefresh (barra);
			napms (300);
		}
		move (FALA_y0, 0);
		clrtoeol ();
	}
#endif

	attron (A_BOLD);
	mvaddstr (FALA_y0, COLS/2 - 4, "Kill him!");
	refresh ();

// bolinha no começo
	mvwaddch (bola, 0, 0, 'O');
	wrefresh (bola);

// cria a barra de vida do chefão e a desenha
	HP_chefe = subwin (stdscr, CAMPO_ALTURA, 2, CAMPO_y0, CAMPO_x0 + CAMPO_LARGURA + 1);
	wattron (HP_chefe, A_BOLD);
	wattron (HP_chefe, COLOR_PAIR (7));
	mvwaddstr (HP_chefe, 0, 0, "/\\");
	mvwaddstr (HP_chefe, 1, 0, "\\/");
	napms (500);
	wrefresh (HP_chefe);
	for (vida_chefe = 0; vida_chefe < CAMPO_ALTURA - 2; ) {
		vida_chefe++;
		mvwaddstr (HP_chefe, vida_chefe, 0, "||");
		mvwaddstr (HP_chefe, vida_chefe + 1, 0, "\\/");
		wrefresh (HP_chefe);
		napms (150);
	}
// limpa as escritinhas e volta pro jogo
	for (y = 0; y < 2; y++) {
		move (CAMPO_y0 + y - 3, 0);
		clrtoeol ();
	}
// e fica rapidão
	periodo = 9;
}


/* Bom jogo, jovem miriápode; tentarás outra vez? */
void Ganhou () {
	int i;
	
	attron (A_BOLD);
	for (i = 0; i < 4; i++) {
		mvaddstr (FALA_y0, COLS/2 - 5, "VOCÊ GANHOU!");
		refresh ();
		napms (600);
		mvaddstr (FALA_y0, COLS/2 - 5, "              ");
		refresh ();
		napms (600);
	}
	attroff (A_BOLD);
	s = KEY_F(2);
}


/* Vê se acertou o chefão, e se sim: PORRADA NELE! */
int BateChefe (int y, int x) {
// coordenadas gerais → coordenadas campo
	y -= CAMPO_y0;
	x -= CAMPO_x0;

// é possível trombar com o chefão [tá na área dele]
	if (y <= y_chefe + 2 && x >= x_chefe && x <= x_chefe + 6) {
// coordenadas campo → coordenadas chefe [começando do 0]
		x -= x_chefe;
		y -= y_chefe;

		switch (y) {
// linha da boca " <===> "
			case 2:
// quininha do desenho, que é em branco: então nem bateu =P
				if (x == 0 || x == 6)
					return 0;
				else {
					if (!(movimento == 'L' && l_mov == 'D' && v_dir == 'C')) {
						if (h_dir == 'D' && x == 1)
							h_dir = 'E';
						else if (h_dir == 'E' && x == 5)
							h_dir = 'D';
					}
					v_dir = 'B';
				}
				break;

// linha do meio: "< 0 0 >"
			case 1:
// bate naquele espacinho enquinado, daí de qualquer modo inverte tudo
				if (x == 1) {
					v_dir = 'B';
					h_dir = 'E';
					break;
				}
// e aqui no outro espacinho enquinado
				else if (x == 5) {
					v_dir = 'B';
					h_dir = 'D';
					break;
				}
// de resto
				if (!(movimento == 'L' && l_mov == 'D' && v_dir == 'C')) {
					if (h_dir == 'D')
						h_dir = 'E';
					else
						h_dir = 'D';
					
					if (l_mov == 'H')
						break;
				}
				v_dir = 'B';
				break;

// linha de cima: "< > < >"
// inverte sentido horizontal, pois o vertical já muda por bater no teto
			case 0:
				if (h_dir == 'D')
					h_dir = 'E';
				else
					h_dir = 'D';
				break;
		}
	
		AtualizaVidaChefe ();

		return 1;
	}
// se não tá nem no Y do chefão, nem tem como bater
	else
		return 0;
}


/* Tira um de vida do chefão */
void AtualizaVidaChefe () {
	mvwaddstr (HP_chefe, vida_chefe, 0, "\\/");
	mvwaddstr (HP_chefe, vida_chefe + 1, 0, "  ");
	wrefresh (HP_chefe);

	vida_chefe--;
	if (vida_chefe == 0)
		Ganhou ();
}


/* ganha o tiro, pra usar contra o chefão */
void GanhaTiro () {
	
}


/* Atira contra o chefão, pra ficar mais divertido o negócio */
void Shoot () {
	
}
