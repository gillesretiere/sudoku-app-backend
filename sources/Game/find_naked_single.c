#include <stdio.h>
#include <stdbool.h>
#include "find_naked_single.h"

static bool is_valid(const SudokuGame *game, int r, int c, int val) {
    for (int i = 0; i < 9; i++) {
        if (game->player_grid[r][i] == val || game->player_grid[i][c] == val) {
            return false;
        }
    }

    int start_r = (r / 3) * 3;
    int start_c = (c / 3) * 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (game->player_grid[start_r + i][start_c + j] == val) {
                return false;
            }
        }
    }
    return true;
}

bool find_naked_single(const SudokuGame *game) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (game->player_grid[r][c] == 0) {
                int count = 0;
                int candidate = 0;

                for (int val = 1; val <= 9; val++) {
                    if (is_valid(game, r, c, val)) {
                        count++;
                        candidate = val;
                    }
                }

                if (count == 1) {
                    /* --- EXPLICATION THÉORIQUE --- */
                    printf("📖 THÉORIE — Candidat Unique (Naked Single) :\n");
                    printf("   Lorsqu'une case vide ne peut recevoir qu'un seul chiffre possible\n");
                    printf("   (parce que les 8 autres figurent déjà sur la ligne, la colonne ou le bloc),\n");
                    printf("   ce chiffre est obligatoirement la solution de cette case.\n\n");

                    /* --- APPLICATION À LA GRILLE --- */
                    printf("🎯 APPLICATION :\n");
                    printf("   En analysant la case (%d, %d), tous les chiffres de 1 à 9 sont éliminés\n", r + 1, c + 1);
                    printf("   par sa ligne, sa colonne ou son bloc 3x3, SAUF le chiffre %d.\n", candidate);
                    printf("   👉 Placement recommandé : la valeur %d en case (%d, %d).\n", candidate, r + 1, c + 1);
                    return true;
                }
            }
        }
    }
    return false;
}