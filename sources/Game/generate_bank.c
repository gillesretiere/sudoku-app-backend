#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "../Generator/generator.h"
#include "solver_eval.h"

#define TARGET_PER_LEVEL 20 // Nombre de grilles souhaité par niveau

int main(void) {
    srand((unsigned int)time(NULL));

    FILE *file = fopen("puzzles_bank.txt", "w");
    if (!file) {
        printf("❌ Erreur : Impossible de créer le fichier puzzles_bank.txt\n");
        return EXIT_FAILURE;
    }

    int counts[4] = {0, 0, 0, 0}; // Compteurs pour niveaux 0, 1, 2, 3
    int total_attempts = 0;

    printf("🚀 Démarrage de la génération de la banque de grilles...\n\n");

    while (counts[0] < TARGET_PER_LEVEL || counts[1] < TARGET_PER_LEVEL || 
           counts[2] < TARGET_PER_LEVEL || counts[3] < TARGET_PER_LEVEL) {

        total_attempts++;
        SudokuGame game;

        // Viser entre 22 et 35 indices restants
        int target_clues = 22 + (rand() % 14); 
        generate_sudoku(game.initial_grid, game.solution, 4);

        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                game.player_grid[r][c] = game.initial_grid[r][c];
            }
        }

        // Évaluation de la grille
        int level = evaluate_puzzle_difficulty(&game);

        if (level >= 0 && level <= 3 && counts[level] < TARGET_PER_LEVEL) {
            counts[level]++;

            // Écriture de la grille dans le fichier au format : NIVEAU;INDICES;PUZZLE_81_CHARS;SOLUTION_81_CHARS
            fprintf(file, "%d;", level);

            int clue_count = 0;
            for (int r = 0; r < 9; r++) {
                for (int c = 0; c < 9; c++) {
                    if (game.initial_grid[r][c] != 0) clue_count++;
                }
            }
            fprintf(file, "%d;", clue_count);

            // Grille de départ (81 caractères)
            for (int r = 0; r < 9; r++) {
                for (int c = 0; c < 9; c++) {
                    fprintf(file, "%d", game.initial_grid[r][c]);
                }
            }
            fprintf(file, ";");

            // Solution (81 caractères)
            for (int r = 0; r < 9; r++) {
                for (int c = 0; c < 9; c++) {
                    fprintf(file, "%d", game.solution[r][c]);
                }
            }
            fprintf(file, "\n");
            fflush(file);

            printf("✅ Grille enregistrée [Niveau %d | %d indices] (N0:%d, N1:%d, N2:%d, N3:%d)\n",
                   level, clue_count, counts[0], counts[1], counts[2], counts[3]);
        }
    }

    fclose(file);
    printf("\n🎉 Génération terminée avec succès en %d tentatives !\n", total_attempts);
    printf("📄 Fichier sauvegardé : puzzles_bank.txt\n");

    return EXIT_SUCCESS;
}