#ifndef GRID_TRANSFORM_H
#define GRID_TRANSFORM_H

#include <stdbool.h>

/**
 * Applique des transformations isométriques aléatoires (permutation de chiffres,
 * rotations et miroirs) à une grille et sa solution pour la rendre unique.
 * 
 * @param puzzle Grille de départ (0 pour les cases vides)
 * @param solution Grille de solution complète
 */
void transform_grid(char puzzle[9][9], char solution[9][9]);

#endif