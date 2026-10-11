#ifndef FIND_NAKED_TRIPLE_H
#define FIND_NAKED_TRIPLE_H

#include <stdbool.h>
#include "game.h"

/**
 * Recherche un Triplet Nu (Naked Triple).
 * Renvoie true si un indice a été trouvé et affiché.
 */
bool find_naked_triple(const SudokuGame *game);

#endif