#ifndef BANK_LOADER_H
#define BANK_LOADER_H

#include <stdbool.h>
#include "game.h"

/**
 * Charge une grille aléatoire depuis puzzles_bank.txt pour le niveau demandé,
 * puis lui applique une transformation isométrique.
 * 
 * @param game Pointeur vers la structure du jeu
 * @param target_level Niveau désiré dans la banque (0, 1, 2 ou 3)
 * @return true si le chargement a réussi, false sinon.
 */
bool load_puzzle_from_bank(SudokuGame *game, int target_level);

#endif