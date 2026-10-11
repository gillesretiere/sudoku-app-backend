#include <stdlib.h>
#include "grid_transform.h"

/* Applique une table de permutation de chiffres à une grille 9x9 */
static void apply_digit_map(char grid[9][9], const char map[10]) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (grid[r][c] >= 1 && grid[r][c] <= 9) {
                grid[r][c] = map[(int)grid[r][c]];
            }
        }
    }
}

/* Applique une rotation de 90 degrés dans le sens horaire */
static void rotate_90(char grid[9][9]) {
    char temp[9][9];
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            temp[c][8 - r] = grid[r][c];
        }
    }
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            grid[r][c] = temp[r][c];
        }
    }
}

/* Applique une symétrie miroir horizontale */
static void flip_horizontal(char grid[9][9]) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 4; c++) {
            char temp = grid[r][c];
            grid[r][c] = grid[r][8 - c];
            grid[r][8 - c] = temp;
        }
    }
}

void transform_grid(char puzzle[9][9], char solution[9][9]) {
    /* 1. Génération d'UNE SEULE table de permutation commune */
    char map[10];
    for (int i = 0; i <= 9; i++) map[i] = (char)i;

    /* Mélange de Fisher-Yates sur les chiffres 1 à 9 */
    for (int i = 9; i > 1; i--) {
        int j = 1 + rand() % i;
        char temp = map[i];
        map[i] = map[j];
        map[j] = temp;
    }

    /* Application de la MÊME table aux deux grilles */
    apply_digit_map(puzzle, map);
    apply_digit_map(solution, map);

    /* 2. Rotations aléatoires identiques */
    int rotations = rand() % 4;
    for (int i = 0; i < rotations; i++) {
        rotate_90(puzzle);
        rotate_90(solution);
    }

    /* 3. Miroir horizontal identique */
    if (rand() % 2 == 1) {
        flip_horizontal(puzzle);
        flip_horizontal(solution);
    }
}