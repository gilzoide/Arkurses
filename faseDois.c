#include "faseDois.h"

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
	const char *historia[] = {
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
	const char *cara[] = {
		"< > < >",
		"< 0 0 >",
		" <===> "
	};
	
// apaga o "Finish Him!"
	mvaddstr (FALA_y0, COLS/2 - 5, "           ");

// tem uma chance em dificuldade de ganhar uma vida extra [ebaa! xD]
	if (rand () % dificuldade == 0) {
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
	wattron (campo, COLOR_PAIR (FGboss));
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

	wattron (campo, COLOR_PAIR (FGboss));
// e desenha-o novamente
	for (i = 0; i < 3; i++)
		mvwaddstr (campo, y_chefe + i, x_chefe, cara[i]);
	wrefresh (campo);
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


/* Atira contra o chefão, pra ficar mais divertido o negócio
 * cuida de ganhar o tiro, atirar e mover o tiro, assim como calcular a colisao com o chefe
 */
void Shoot (char *fired) {	
	if (!*fired) {
		// ganha o tiro
		if (tiro == SHOT_TIME) {
			attron (COLOR_PAIR (FGshot));
			mvaddstr (CAMPO_ALTURA/2, CAMPO_x0 - 17, "You got a shot!");
			mvaddstr (CAMPO_ALTURA/2 + 1, CAMPO_x0 - 17, "Press Up key or W");
			attroff (COLOR_PAIR (FGshot));
			refresh ();
			tiro++;
		}
		else if (tiro < SHOT_TIME) {
			tiro++;
		}
	}
	// tiro correndo
	else {
		// ainda nao tem nem esperanca de acertar
		if (y_tiro >= CAMPO_y0 + 3) {
			wattron (campo, COLOR_PAIR (FGshot));
			
			mvwaddstr (campo, y_tiro, x_tiro, "||");
			wrefresh (campo);
			
			wattroff (campo, COLOR_PAIR (FGshot));
			y_tiro--;
		}
		// ta no y do chefe, sera que acertou?
		else {
			// apaga rastro do tiro
			for (y_tiro++; y_tiro < BARRA_y0 - CAMPO_y0; y_tiro++) {
				mvwaddstr (campo, y_tiro, x_tiro, "  ");
				wrefresh (campo);
			}
			// acertou! UHUL!
			if (x_tiro >= x_chefe && x_tiro <= x_chefe + 6) {
				AtualizaVidaChefe ();
			}
			tiro = 0;
			*fired = 0;
		}
	}
}


/* Clicou pra atirar */
void ClickShoot () {
	// descobre o 'x' da barra, pro tiro sair do meio dela
	getbegyx (barra, y_tiro, x_tiro);
	y_tiro -= CAMPO_y0 + 1;
	x_tiro -= CAMPO_x0 - 1;
	
	mvaddstr (CAMPO_ALTURA/2, CAMPO_x0 - 17, "               ");
	mvaddstr (CAMPO_ALTURA/2 + 1, CAMPO_x0 - 17, "                 ");
	refresh ();
	return;
}
