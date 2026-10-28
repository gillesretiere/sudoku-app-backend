#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "core/core_board.h"
#include "solver/solver_dlx.h"
#include "solver/solver_eval.h"
#include "generator/generator_royle.h"

/* Fonction d'affichage propre d'une grille dans le terminal */
void print_board(const SudokuBoard *board) {
    printf("+-------+-------+-------+\n");
    for (int r = 0; r < BOARD_SIZE; r++) {
        printf("| ");
        for (int c = 0; c < BOARD_SIZE; c++) {
            uint8_t val = board->cells[r][c].value;
            if (val == 0) {
                printf(". ");
            } else {
                printf("%d ", val);
            }
            if ((c + 1) % 3 == 0) printf("| ");
        }
        printf("\n");
        if ((r + 1) % 3 == 0) {
            printf("+-------+-------+-------+\n");
        }
    }
}

int main(void) {
    printf("===========================================\n");
    printf("   TEST DU MOTEUR SUDOKU PHASE 2 (C11)     \n");
    printf("===========================================\n\n");

    /* Définition de la cible de génération (Exemple : Niveau Avancé / SE 2.8 - 4.5) */
    GeneratorTarget target = {
        .min_se = 2.8f,
        .max_se = 4.5f,
        .max_clues = 32,
        .max_attempts = 50
    };

    printf("Recherche d'une grille (Cible SE: %.1f - %.1f, Clues <= %d)...\n\n", 
           target.min_se, target.max_se, target.max_clues);

    clock_t start_time = clock();

    SudokuBoard puzzle;
    bool success = generator_royle_generate_puzzle(&target, &puzzle);

    clock_t end_time = clock();
    double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    if (!success) {
        printf("[X] Echec : Aucune grille conforme n'a ete generee dans le temps imparti.\n");
        return EXIT_FAILURE;
    }

    /* Compte des indices restants */
    int clues = 0;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (puzzle.cells[r][c].value != 0) clues++;
        }
    }

    /* Analyse finale par l'évaluateur logique */
    SudokuEvaluation eval;
    solver_eval_analyze(&puzzle, &eval);

    /* Affichage des résultats */
    printf("[OK] Grille generee avec succes en %.3f secondes !\n\n", elapsed);
    print_board(&puzzle);

    printf("\n--- BILAN DE LA GRILLE ---\n");
    printf("Indices restants : %d\n", clues);
    printf("Score SE Max     : %.1f\n", eval.max_se_rating);
    printf("Etapes logiques  : %d\n", eval.total_steps);
    printf("Resoluble        : %s\n", eval.is_solvable ? "Oui" : "Non");
    printf("--------------------------\n");

    return EXIT_SUCCESS;
}