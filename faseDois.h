/** @file faseDois.h
 * Segunda fase, incluindo a _cutscene_ e o chefe
 */
#ifndef FASEDOIS_H
#define FASEDOIS_H

#include "arkurses.h"

/// Um fogo de artifício [que lindo ^^]
void FogoArtificio (int x);

/// Conseguiu passar da primeira parte do jogo =]
void FaseDois ();

/// Quem sabe até ganhar o jogo!
void Ganhou ();

/// Move o chefe da segunda [e última] fase
void MoveChefe ();

/// Bate no chefão (ou não, verifica); porrada, manow!
int BateChefe (int y, int x);

/// Se acertou o chefão, diminui sua vida
void AtualizaVidaChefe ();

/// Atira! Só rola na segunda fase
void Shoot (char*);

/// Preparação para o tiro
void ClickShoot ();

#endif
