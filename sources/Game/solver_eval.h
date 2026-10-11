#ifndef SOLVER_EVAL_H
#define SOLVER_EVAL_H

#include "game.h"

/**
 * Évalue la difficulté exacte d'une grille en la résolvant silencieusement.
 * 
 * Valeurs de retour :
 *   0 : Résoluble uniquement avec Niveau 0 (Singles)
 *   1 : Requiert au moins le Niveau 1 (Paires)
 *   2 : Requiert au moins le Niveau 2 (Intersections / Triplets)
 *   3 : Requiert au moins le Niveau 3 (X-Wing)
 *  -1 : Impossible à résoudre avec les stratégies actuelles (rejetée)
 */
int evaluate_puzzle_difficulty(const SudokuGame *game);

#endif