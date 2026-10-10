#ifndef FIND_HIDDEN_PAIR_H
#define FIND_HIDDEN_PAIR_H

#include <stdbool.h>
#include "game.h"

/**
 * Recherche une Paire Cachée (Hidden Pair) dans la grille.
 * Renvoie true si un indice a été trouvé et affiché.
 */
bool find_hidden_pair(const SudokuGame *game);

#endif