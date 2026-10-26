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
    int n_total[5] = {0};
    int n3_xwing = 0, n3_ywing = 0, n3_both = 0;
    int n4_sword = 0, n4_ur = 0, n4_both = 0;

    printf("=== ANALYSE DE LA BANQUE DE GRILLES (5 NIVEAUX) ===\n\n");

    while (fgets(line, sizeof(line), file)) {
        int level, clue_count;
        char puzzle_str[82], solution_str[82];

        if (sscanf(line, "%d;%d;%81[^;];%81s", &level, &clue_count, puzzle_str, solution_str) != 4) {
            continue;
        }

        if (level >= 0 && level <= 4) {
            n_total[level]++;
            SudokuGame game;
            memset(&game, 0, sizeof(SudokuGame));

            for (int r = 0; r < 9; r++) {
                for (int c = 0; c < 9; c++) {
                    game.initial_grid[r][c] = puzzle_str[r * 9 + c] - '0';
                    game.player_grid[r][c] = game.initial_grid[r][c];
                    game.solution[r][c] = solution_str[r * 9 + c] - '0';
                }
            }

            EvalResult res = evaluate_puzzle_difficulty(&game);

            if (level == 3) {
                if (res.x_wing_count > 0 && res.y_wing_count > 0) n3_both++;
                else if (res.x_wing_count > 0) n3_xwing++;
                else if (res.y_wing_count > 0) n3_ywing++;
            } else if (level == 4) {
                if (res.swordfish_count > 0 && res.unique_rectangle_count > 0) n4_both++;
                else if (res.swordfish_count > 0) n4_sword++;
                else if (res.unique_rectangle_count > 0) n4_ur++;

                printf("Grille N4 #%02d [%d indices] -> Swordfish : %d | Rectangle Unique : %d\n",
                       n_total[4], clue_count, res.swordfish_count, res.unique_rectangle_count);
            }
        }
    }

    fclose(file);

    printf("\n=== RÉCAPITULATIF DE LA BANQUE ===\n");
    for (int i = 0; i <= 4; i++) {
        printf("Niveau %d : %d grilles\n", i, n_total[i]);
    }
    printf("\n--- Détails Niveau 3 ---\nExigent X-Wing seul : %d | Y-Wing seul : %d | Les deux : %d\n",
           n3_xwing, n3_ywing, n3_both);
    printf("--- Détails Niveau 4 ---\nExigent Swordfish seul : %d | Rectangle Unique seul : %d | Les deux : %d\n",
           n4_sword, n4_ur, n4_both);

    return EXIT_SUCCESS;
}