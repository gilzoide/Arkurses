#include "game.h"

void InitCurses () {
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
}


/* Displays the help (in a created window and panel, for going back to the normal field after) */
void Help () {
	WINDOW *help;
	PANEL *up;

	const int num_comandos = 6;
	const char *comandos[] = {
		"Setinhas ou A,D", "move para esquerda/direita",
		"'-'", "diminui velocidade",
		"'+' ou '='", "aumenta velocidade",
		"Barra de Espaço", "pausa jogo",
		"F2", "recomeça jogo",
		"Q", "sair"
	};

	help = newwin (num_comandos + 2, HELP_WIDTH, 1, 0);
	up = new_panel (help);
	update_panels ();
	doupdate ();

	box (help, 0, 0);
	wbkgd (help, COLOR_PAIR (BGhelp));
	wrefresh (help);
	mvwaddstr (help, 0, HELP_WIDTH/2 - 2, "HELP");

	int i, j;
	for (i = j = 0; i < num_comandos; i++, j++) {
		wattron (help, A_BOLD);
		mvwaddstr (help, 1 + i, 1, comandos[j]);
		waddstr (help, ": ");

		j++;

		wattrset (help, COLOR_PAIR (BGhelp));
		waddstr (help, comandos[j]);
	}

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
