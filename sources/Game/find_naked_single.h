#ifndef FIND_NAKED_SINGLE_H
#define FIND_NAKED_SINGLE_H

#include <stdbool.h>
#include "game.h"

// Cherche une case n'ayant qu'une seule valeur possible.
// Renvoie true si un indice a été trouvé, false sinon.
bool find_naked_single(const SudokuGame *game);

#endif