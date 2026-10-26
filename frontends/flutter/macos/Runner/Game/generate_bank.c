#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "game.h"
#include "../Generator/generator.h"
#include "solver_eval.h"

#define TARGET_PER_LEVEL 20
#define TARGET_N3_SUB 7

int main(void) {
    srand((unsigned int)time(NULL));

    FILE *file = fopen("puzzles_bank.txt", "w");
    if (!file) {
        printf("❌ Erreur : Impossible de créer le fichier puzzles_bank.txt\n");
        return EXIT_FAILURE;
    }

    int counts[5] = {0, 0, 0, 0, 0};
    int n3_x_only = 0, n3_y_only = 0, n3_both = 0;
    int total_attempts = 0;
    int rejected_unsolved = 0;
    int skipped_already_full = 0;

    printf("🚀 Démarrage de la génération optimisée (N0 à N4)...\n\n");
    fflush(stdout);

    while (counts[0] < TARGET_PER_LEVEL || counts[1] < TARGET_PER_LEVEL || 
           counts[2] < TARGET_PER_LEVEL || counts[3] < TARGET_PER_LEVEL || 
           counts[4] < TARGET_PER_LEVEL) {

        total_attempts++;
        SudokuGame game;
        memset(&game, 0, sizeof(SudokuGame));

        /* Calibrage du creusement ciblé sur le besoin actuel */
        int gen_diff = 1;
        if (counts[0] < TARGET_PER_LEVEL) {
            gen_diff = 1;
        } else if (counts[1] < TARGET_PER_LEVEL) {
            gen_diff = 2;
        } else if (counts[2] < TARGET_PER_LEVEL) {
            gen_diff = 2;
        } else {
            /* gen_diff = 3 est la zone optimale pour N3 et N4 (26 à 30 indices) */
            gen_diff = 3;
        }

        generate_sudoku(game.initial_grid, game.solution, gen_diff);

        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                game.player_grid[r][c] = game.initial_grid[r][c];
            }
        }

        int clue_count = 0;
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                if (game.initial_grid[r][c] != 0) clue_count++;
            }
        }

        EvalResult res = evaluate_puzzle_difficulty(&game);

        /* Case 1 : La grille n'est pas entièrement résoluble par nos règles logiques */
        if (!res.is_solved) {
            rejected_unsolved++;
            if (total_attempts % 500 == 0) {
                printf("... %d essais | Rejets(Insolvables): %d | Ignorés(Pleins): %d | N0:%d N1:%d N2:%d N3:%d N4:%d\n",
                       total_attempts, rejected_unsolved, skipped_already_full,
                       counts[0], counts[1], counts[2], counts[3], counts[4]);
                fflush(stdout);
            }
            continue;
        }

        bool valid = false;

        /* Case 2 : Vérification du quota du niveau obtenu */
        if (res.level >= 0 && res.level < 3 && counts[res.level] < TARGET_PER_LEVEL) {
            valid = true;
        } else if (res.level == 3 && counts[3] < TARGET_PER_LEVEL) {
            if (res.x_wing_count > 0 && res.y_wing_count == 0 && n3_x_only < TARGET_N3_SUB) {
                n3_x_only++; valid = true;
            } else if (res.x_wing_count == 0 && res.y_wing_count > 0 && n3_y_only < TARGET_N3_SUB) {
                n3_y_only++; valid = true;
            } else if (res.x_wing_count > 0 && res.y_wing_count > 0 && n3_both < (TARGET_PER_LEVEL - 2 * TARGET_N3_SUB)) {
                n3_both++; valid = true;
            }
        } else if (res.level == 4 && counts[4] < TARGET_PER_LEVEL) {
            valid = true;
        }

        if (valid) {
            counts[res.level]++;

            fprintf(file, "%d;%d;", res.level, clue_count);
            for (int r = 0; r < 9; r++) {
                for (int c = 0; c < 9; c++) {
                    fprintf(file, "%d", game.initial_grid[r][c]);
                }
            }
            fprintf(file, ";");
            for (int r = 0; r < 9; r++) {
                for (int c = 0; c < 9; c++) {
                    fprintf(file, "%d", game.solution[r][c]);
                }
            }
            fprintf(file, "\n");
            fflush(file);

            printf("✅ Grille N%d validée (%d indices) | N0:%d, N1:%d, N2:%d, N3:%d, N4:%d\n",
                   res.level, clue_count, counts[0], counts[1], counts[2], counts[3], counts[4]);
            fflush(stdout);
        } else {
            /* La grille était résoluble mais son niveau était déjà complet */
            skipped_already_full++;
        }
    }

    fclose(file);
    printf("\n🎉 Génération terminée avec succès en %d essais !\n", total_attempts);
    return EXIT_SUCCESS;
}