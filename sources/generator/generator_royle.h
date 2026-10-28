#ifndef GENERATOR_ROYLE_H
#define GENERATOR_ROYLE_H

#include "core_board.h"
#include "solver_dlx.h"
#include "solver_eval.h"
#include <stdbool.h>

/**
 * @brief Spécifications de cible pour la génération d'une grille.
 */
typedef struct {
    float min_se;       /* Score SE minimal exigé */
    float max_se;       /* Score SE maximal autorisé */
    int max_clues;      /* Nombre maximal d'indices restants */
    int max_attempts;   /* Nombre maximal de tentatives de génération */
} GeneratorTarget;

/**
 * @brief Génère une grille complète valide (solution complète 9x9).
 * 
 * @param board Pointeur vers la structure SudokuBoard recevant la grille complète.
 */
void generator_royle_generate_full_grid(SudokuBoard *board);

/**
 * @brief Génère une grille de Sudoku symétrique (S180) et conforme au niveau ciblé.
 * 
 * @param target Critères de difficulté et d'indices
 * @param out_board Grille générée (avec les cases vides posées à 0)
 * @return true si une grille conforme a été générée, false sinon
 */
bool generator_royle_generate_puzzle(const GeneratorTarget *target, SudokuBoard *out_board);

#endif /* GENERATOR_ROYLE_H */