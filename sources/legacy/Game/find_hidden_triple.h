#ifndef FIND_HIDDEN_TRIPLE_H
#define FIND_HIDDEN_TRIPLE_H

#include <stdbool.h>
#include "game.h"

/**
 * Recherche et élimine les candidats superflus d'un Triplet Caché.
 * Renvoie true si au moins un candidat a été éliminé.
 */
bool find_hidden_triple(SudokuGame *game);

#endif /* FIND_HIDDEN_TRIPLE_H */