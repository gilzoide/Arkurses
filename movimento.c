#include "movimento.h"

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

