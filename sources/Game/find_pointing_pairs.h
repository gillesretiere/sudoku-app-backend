#ifndef FIND_POINTING_PAIRS_H
#define FIND_POINTING_PAIRS_H

#include <stdbool.h>
#include "game.h"

/**
 * Recherche des Paires ou Triplets Pointés (Pointing Pairs/Triples).
 * Renvoie true si un indice a été trouvé et affiché.
 */
bool find_pointing_pairs(const SudokuGame *game);

#endif