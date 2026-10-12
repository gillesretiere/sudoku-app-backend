#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "../Generator/generator.h"
#include "solver_eval.h"

#define TARGET_PER_LEVEL 20

int main(void) {
    srand((unsigned int)time(NULL));

    FILE *file = fopen("puzzles_bank.txt", "w");
    if (!file) {
        printf("Erreur : Impossible de créer le fichier puzzles_bank.txt\n");
        return EXIT_FAILURE;
    }

    int counts[4] = {0, 0, 0, 0};
    int total_attempts = 0;
    int rejected_unsolvable = 0;
    int rejected_too_easy = 0;

    printf("Démarrage de la génération ciblée N3...\n\n");

    while (counts[0] < TARGET_PER_LEVEL || counts[1] < TARGET_PER_LEVEL || 
           counts[2] < TARGET_PER_LEVEL || counts[3] < TARGET_PER_LEVEL) {

        total_attempts++;
        SudokuGame game;

        int gen_diff = 1;
        if (counts[3] < TARGET_PER_LEVEL)      gen_diff = 4;
        else if (counts[2] < TARGET_PER_LEVEL) gen_diff = 3;
        else if (counts[1] < TARGET_PER_LEVEL) gen_diff = 2;

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

        if (res.level == -1) {
            rejected_unsolvable++;
        } else if (res.level < 3 && counts[3] < TARGET_PER_LEVEL) {
            rejected_too_easy++;
        }

        bool valid = false;
        if (res.level == 3 && res.x_wing_count >= 1 && res.x_wing_count <= 3) {
            valid = true;
        } else if (res.level >= 0 && res.level < 3) {
            valid = true;
        }

        if (valid && counts[res.level] < TARGET_PER_LEVEL) {
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

            printf("Grille Niveau %d enregistrée (%d indices, %d X-Wing) | N0:%d/%d, N1:%d/%d, N2:%d/%d, N3:%d/%d\n",
                   res.level, clue_count, res.x_wing_count,
                   counts[0], TARGET_PER_LEVEL, counts[1], TARGET_PER_LEVEL,
                   counts[2], TARGET_PER_LEVEL, counts[3], TARGET_PER_LEVEL);
        }

        if (total_attempts % 100 == 0 && counts[3] < TARGET_PER_LEVEL) {
            printf("Tentatives : %d... (N3 : %d/%d | Trop faciles : %d, Trop complexes/Bloquées : %d)\n",
                   total_attempts, counts[3], TARGET_PER_LEVEL, rejected_too_easy, rejected_unsolvable);
        }
    }

    fclose(file);
    printf("\nGénération terminée avec succès en %d tentatives !\n", total_attempts);
    printf("Fichier sauvegardé : puzzles_bank.txt\n");

    return EXIT_SUCCESS;
}