#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bank_loader.h"
#include "grid_transform.h"
#include "candidates.h"

#define MAX_BANK_ENTRIES 500
#define LINE_BUF_SIZE 256

bool load_puzzle_from_bank(SudokuGame *game, int target_level) {
    FILE *file = fopen("puzzles_bank.txt", "r");
    if (!file) {
        printf("❌ Erreur : Impossible d'ouvrir le fichier puzzles_bank.txt.\n");
        return false;
    }

    char matching_lines[MAX_BANK_ENTRIES][LINE_BUF_SIZE];
    int matching_count = 0;
    char buffer[LINE_BUF_SIZE];

    /* 1. Filtration des grilles correspondant au niveau demandé */
    while (fgets(buffer, sizeof(buffer), file) && matching_count < MAX_BANK_ENTRIES) {
        int level = -1;
        if (sscanf(buffer, "%d;", &level) == 1 && level == target_level) {
            strncpy(matching_lines[matching_count], buffer, LINE_BUF_SIZE - 1);
            matching_lines[matching_count][LINE_BUF_SIZE - 1] = '\0';
            matching_count++;
        }
    }
    fclose(file);

    if (matching_count == 0) {
        printf("⚠️ Aucune grille de niveau %d trouvée dans la banque.\n", target_level);
        return false;
    }

    /* 2. Sélection aléatoire d'une grille */
    int selected_idx = rand() % matching_count;
    char *selected_line = matching_lines[selected_idx];

    int level, clue_count;
    char puzzle_str[82];
    char solution_str[82];

    if (sscanf(selected_line, "%d;%d;%81s;%81s", &level, &clue_count, puzzle_str, solution_str) != 4) {
        printf("❌ Erreur de format dans la ligne de banque sélectionnée.\n");
        return false;
    }

    /* 3. Conversion des chaînes de 81 caractères en tableaux 2D 9x9 */
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int idx = r * 9 + c;
            game->initial_grid[r][c] = puzzle_str[idx] - '0';
            game->solution[r][c] = solution_str[idx] - '0';
            game->player_grid[r][c] = game->initial_grid[r][c];
        }
    }

    /* 4. Application des transformations isométriques (variations infinies) */
    transform_grid(game->initial_grid, game->solution);

    /* Copie de la grille transformée dans le plateau du joueur */
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            game->player_grid[r][c] = game->initial_grid[r][c];
        }
    }

    compute_autonote(game);
    printf("Grille [Niveau %d] chargée et transformée avec succès (%d indices d'origine).\n", target_level, clue_count);
    return true;
}