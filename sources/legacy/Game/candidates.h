#ifndef CANDIDATES_H
#define CANDIDATES_H

#include "game.h"

// Active ou désactive l'AutoNote sur toute la grille
void compute_autonote(SudokuGame *game);

// Manipulations individuelles des candidats (utile pour la future IHM)
void add_candidate(SudokuGame *game, int r, int c, int val);
void remove_candidate(SudokuGame *game, int r, int c, int val);
bool has_candidate(const SudokuGame *game, int r, int c, int val);
int count_candidates(const SudokuGame *game, int r, int c);

#endif