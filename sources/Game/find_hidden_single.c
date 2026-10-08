#include <stdio.h>
#include <stdbool.h>
#include "find_hidden_single.h"

// Helper : vérifie si un chiffre 'val' peut être placé en position (r, c)
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

// Affiche l'explication théorique du Chiffre Caché
static void print_theory(void) {
    printf("📖 THÉORIE — Chiffre Caché (Hidden Single) :\n");
    printf("   Même si une case possède plusieurs candidats possibles, si un chiffre donné\n");
    printf("   ne peut aller nulle part ailleurs dans sa ligne, sa colonne ou son bloc,\n");
    printf("   alors ce chiffre doit obligatoirement être placé dans cette case.\n\n");
}

bool find_hidden_single(const SudokuGame *game) {
    // 1. Recherche par Lignes
    for (int r = 0; r < 9; r++) {
        for (int val = 1; val <= 9; val++) {
            int count = 0;
            int target_c = -1;

            for (int c = 0; c < 9; c++) {
                if (game->player_grid[r][c] == 0 && is_valid(game, r, c, val)) {
                    count++;
                    target_c = c;
                }
            }

            if (count == 1) {
                print_theory();
                printf("🎯 APPLICATION :\n");
                printf("   Sur la ligne %d, le chiffre %d ne peut aller qu'en colonne %d.\n", 
                       r + 1, val, target_c + 1);
                printf("   👉 Placement recommandé : la valeur %d en case (%d, %d).\n", 
                       val, r + 1, target_c + 1);
                return true;
            }
        }
    }

    // 2. Recherche par Colonnes
    for (int c = 0; c < 9; c++) {
        for (int val = 1; val <= 9; val++) {
            int count = 0;
            int target_r = -1;

            for (int r = 0; r < 9; r++) {
                if (game->player_grid[r][c] == 0 && is_valid(game, r, c, val)) {
                    count++;
                    target_r = r;
                }
            }

            if (count == 1) {
                print_theory();
                printf("🎯 APPLICATION :\n");
                printf("   Dans la colonne %d, le chiffre %d ne peut aller qu'en ligne %d.\n", 
                       c + 1, val, target_r + 1);
                printf("   👉 Placement recommandé : la valeur %d en case (%d, %d).\n", 
                       val, target_r + 1, c + 1);
                return true;
            }
        }
    }

    // 3. Recherche par Blocs 3x3
    for (int box = 0; box < 9; box++) {
        int start_r = (box / 3) * 3;
        int start_c = (box % 3) * 3;

        for (int val = 1; val <= 9; val++) {
            int count = 0;
            int target_r = -1;
            int target_c = -1;

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int r = start_r + i;
                    int c = start_c + j;

                    if (game->player_grid[r][c] == 0 && is_valid(game, r, c, val)) {
                        count++;
                        target_r = r;
                        target_c = c;
                    }
                }
            }

            if (count == 1) {
                print_theory();
                printf("🎯 APPLICATION :\n");
                printf("   Dans le bloc 3x3 n°%d, le chiffre %d ne peut être placé qu'en case (%d, %d).\n", 
                       box + 1, val, target_r + 1, target_c + 1);
                printf("   👉 Placement recommandé : la valeur %d en case (%d, %d).\n", 
                       val, target_r + 1, target_c + 1);
                return true;
            }
        }
    }

    return false;
}