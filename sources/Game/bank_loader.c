#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bank_loader.h"
#include "grid_transform.h"
#include "candidates.h"

#define MAX_BANK_ENTRIES 500
#define LINE_BUF_SIZE 256

typedef struct {
    char puzzle[82];
    char solution[82];
    int level;
    int clues;
} BankEntry;

// Fonction utilitaire pour trouver et ouvrir le fichier de la banque de grilles
static FILE* open_puzzle_bank_file(void) {
    // 1. Essai depuis le dossier courant (ex: si on est dans sources/Game/)
    FILE *file = fopen("puzzles_bank.txt", "r");
    if (file) return file;

    // 2. Essai depuis la racine du projet (ex: quand on lance ./sources/Game/sudoku_game)
    file = fopen("sources/Game/puzzles_bank.txt", "r");
    if (file) return file;

    // 3. Essai depuis un sous-dossier parent si nécessaire
    file = fopen("../sources/Game/puzzles_bank.txt", "r");
    if (file) return file;

    return NULL; // Aucun fichier trouvé
}

bool load_puzzle_from_bank(SudokuGame *game, int target_level) {
    FILE *file = open_puzzle_bank_file();
    if (!file) {
        printf("⚠️ Impossible d'ouvrir puzzles_bank.txt.\n");
        return false; // Utilisation de la génération par défaut en cas d'échec
    }

    BankEntry matching_puzzles[100];
    int count = 0;

    char line[LINE_BUF_SIZE];
    while (fgets(line, sizeof(line), file)) {
        int level, clues;
        char puzzle[82], solution[82];

        /* Format : niveau;nb_indices;grille_puzzle;grille_solution */
        if (sscanf(line, "%d;%d;%81s;%81s", &level, &clues, puzzle, solution) == 4) {
            if (level == target_level && count < 100) {
                matching_puzzles[count].level = level;
                matching_puzzles[count].clues = clues;
                strncpy(matching_puzzles[count].puzzle, puzzle, 82);
                strncpy(matching_puzzles[count].solution, solution, 82);
                count++;
            }
        }
    }
    fclose(file);

    if (count == 0) {
        printf("⚠️ Aucune grille trouvée pour le niveau %d dans la banque.\n", target_level);
        return false;
    }

    /* Tirage aléatoire parmi les grilles correspondantes */
    int random_idx = rand() % count;
    BankEntry selected = matching_puzzles[random_idx];

    /* Chargement dans le moteur de jeu */
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            char p_char = selected.puzzle[r * 9 + c];
            char s_char = selected.solution[r * 9 + c];

            int p_val = (p_char >= '1' && p_char <= '9') ? (p_char - '0') : 0;
            int s_val = (s_char >= '1' && s_char <= '9') ? (s_char - '0') : 0;

            game->initial_grid[r][c] = (char)p_val;
            game->player_grid[r][c] = (char)p_val;
            game->solution[r][c] = (char)s_val;
        }
    }

    return true;
}