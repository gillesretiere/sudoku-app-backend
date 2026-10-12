#ifndef FIND_Y_WING_H
#define FIND_Y_WING_H

#include <stdbool.h>
#include "game.h"

/**
 * Cherche un Y-Wing (XY-Wing) sur la grille.
 * Affiche l'indice s'il est trouvé.
 * 
 * @param game Pointeur sur la structure du jeu
 * @return true si un Y-Wing a été trouvé et a permis une élimination, false sinon.
 */
bool find_y_wing(SudokuGame *game);

#endif