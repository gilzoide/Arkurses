#include "arkurses.h"
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




int main () {
	int i;

	setlocale (LC_ALL, "");	// para aparecer os chars doidos
	srand (time (NULL));

	InitCurses ();

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
#else
	dificuldade = 1;
#endif

	cbreak (); // não espera a tecla 'enter'
	noecho (); // não escreve as teclas apertadas, para interatividades
	curs_set (0); // esconde o cursor
	
	mvwaddstr (hud, 0, 0, "'?': Help");
	AtualizaHud ();

	int frame;	// frame em que está: para mover a bolinha e o último bloco com velocidades diferentes
	char fired;	// diz se atirou ou nao
	while (s != 'q') {
		CriaCampo ();				// novo jogo:
		CriaBlocos ();
		CriaBarra ();				// cria as coisinhas
		CriaBola ();				// em seu devido lugar
#ifndef FASEDOIS
		vidas = 5;					// e também seu
		numblocos = 15*dificuldade;	// valor inicial
#else
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
		tiro = 0;
		fired = 0;
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
			else if (vida_chefe > 0) {
				if (frame % 150 == 0)
					MoveChefe ();
				if (frame % 5 == 0)
					Shoot (&fired);
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
					if (tiro > SHOT_TIME && !fired) {
						ClickShoot ();
						fired = 1;
					}
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
