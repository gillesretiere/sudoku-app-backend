#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "solver_eval.h"

int main(void) {
    FILE *file = fopen("puzzles_bank.txt", "r");
    if (!file) {
        printf("❌ Erreur : Impossible d'ouvrir puzzles_bank.txt\n");
        return EXIT_FAILURE;
    }

    char line[256];
    int puzzle_index = 0;
    int n3_total = 0;
    int n3_with_xwing = 0;
    int n3_with_ywing = 0;
    int n3_with_both = 0;

    printf("=== ANALYSE DE LA BANQUE DE GRILLES ===\n\n");

    while (fgets(line, sizeof(line), file)) {
        int level, clue_count;
        char puzzle_str[82], solution_str[82];

        if (sscanf(line, "%d;%d;%81s;%81s", &level, &clue_count, puzzle_str, solution_str) != 4) {
            continue;
        }

        puzzle_index++;

        if (level == 3) {
            n3_total++;
            SudokuGame game;

            /* Reconstruction de la grille pour évaluation */
            for (int r = 0; r < 9; r++) {
                for (int c = 0; c < 9; c++) {
                    game.initial_grid[r][c] = puzzle_str[r * 9 + c] - '0';
                    game.player_grid[r][c] = game.initial_grid[r][c];
                    game.solution[r][c] = solution_str[r * 9 + c] - '0';
                }
            }

            EvalResult res = evaluate_puzzle_difficulty(&game);

            if (res.x_wing_count > 0 && res.y_wing_count > 0) n3_with_both++;
            else if (res.x_wing_count > 0) n3_with_xwing++;
            else if (res.y_wing_count > 0) n3_with_ywing++;

            printf("Grille N3 #%02d [%d indices] -> X-Wing : %d | Y-Wing : %d\n",
                   n3_total, clue_count, res.x_wing_count, res.y_wing_count);
        }
    }

    fclose(file);

    printf("\n=== RÉSULTAT DU NIVEAU 3 ===\n");
    printf("Total grilles N3     : %d\n", n3_total);
    printf("Exigent X-Wing seul  : %d\n", n3_with_xwing);
    printf("Exigent Y-Wing seul  : %d\n", n3_with_ywing);
    printf("Exigent les DEUX     : %d\n", n3_with_both);

    return EXIT_SUCCESS;
}