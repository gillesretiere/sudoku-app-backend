#ifndef SOLVER_BRIDGE_H
#define SOLVER_BRIDGE_H

#include "../Game/game.h"

/**
 * Analyse la grille actuelle du joueur et affiche le premier indice disponible,
 * en testant les stratégies de la plus simple à la plus complexe.
 */
void provide_hint(const SudokuGame *game);

#endif /* SOLVER_BRIDGE_H */