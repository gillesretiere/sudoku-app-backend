#ifndef FIND_SWORDFISH_H
#define FIND_SWORDFISH_H

#include <stdbool.h>
#include "game.h"

/**
 * Recherche et applique la stratégie du Swordfish sur les lignes et colonnes.
 * Renvoie true si au moins un candidat a été éliminé.
 */
bool find_swordfish(SudokuGame *game);

#endif /* FIND_SWORDFISH_H */