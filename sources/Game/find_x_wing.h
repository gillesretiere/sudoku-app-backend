#ifndef FIND_X_WING_H
#define FIND_X_WING_H

#include <stdbool.h>
#include "game.h"

/**
 * Recherche un motif X-Wing (orienté lignes ou colonnes).
 * Renvoie true si un indice a été trouvé et affiché.
 */
bool find_x_wing(const SudokuGame *game);

#endif