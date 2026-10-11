#ifndef FIND_BOX_LINE_REDUCTION_H
#define FIND_BOX_LINE_REDUCTION_H

#include <stdbool.h>
#include "game.h"

/**
 * Recherche une Réduction Bloc-Ligne (Box-Line Reduction).
 * Renvoie true si un indice a été trouvé et affiché.
 */
bool find_box_line_reduction(const SudokuGame *game);

#endif