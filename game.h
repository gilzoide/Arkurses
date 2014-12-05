
#ifndef GAME_H
#define GAME_H

#include "arkurses.h"

void InitCurses ();

void Help ();	// displays the help
void Restart ();	// ask for restarting the game, and does it if needed
void Pause ();	// pause the game
void AtualizaHud ();	// acho que você já sabe o que faz, né?

void CriaBlocos ();	// inicializadores
void CriaBarra ();	// dos
void CriaBola ();	// gráficos
void CriaCampo ();	// do jogo

/// Jogador morreu (deixou a bolinha cair), perde uma vida; se acabar, perde
void Morreu ();

#endif
