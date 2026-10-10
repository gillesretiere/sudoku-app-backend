#ifndef FIND_NAKED_PAIR_H
#define FIND_NAKED_PAIR_H

#include <stdbool.h>
#include "game.h"

/**
 * Recherche une Paire Nue (Naked Pair) dans la grille.
 * Renvoie true si un indice a été trouvé et affiché.
 */
bool find_naked_pair(const SudokuGame *game);

#endif