#include "candidates.h"

// Helper interne : vérifie les règles de validité classiques du Sudoku
static bool is_valid_placement(const SudokuGame *game, int r, int c, int val) {
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

// Calcule et met à jour automatiquement tous les candidats de la grille
void compute_autonote(SudokuGame *game) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            // Réinitialise le masque de la case
            game->candidates[r][c] = 0;

            // Si la case est déjà remplie, elle n'a aucun candidat
            if (game->player_grid[r][c] != 0) {
                continue;
            }

            // Teste chaque chiffre de 1 à 9 et active le bit correspondant
            for (int val = 1; val <= 9; val++) {
                if (is_valid_placement(game, r, c, val)) {
                    game->candidates[r][c] |= (1 << val); // Active le bit 'val'
                }
            }
        }
    }
}

// Vérifie si la valeur 'val' est un candidat actif dans la case (r, c)
bool has_candidate(const SudokuGame *game, int r, int c, int val) {
    if (val < 1 || val > 9) return false;
    return (game->candidates[r][c] & (1 << val)) != 0;
}

// Ajoute un candidat manuellement
void add_candidate(SudokuGame *game, int r, int c, int val) {
    if (val >= 1 && val <= 9) {
        game->candidates[r][c] |= (1 << val);
    }
}

// Retire un candidat manuellement
void remove_candidate(SudokuGame *game, int r, int c, int val) {
    if (val >= 1 && val <= 9) {
        game->candidates[r][c] &= ~(1 << val);
    }
}

// Compte le nombre de candidats restants dans une case
int count_candidates(const SudokuGame *game, int r, int c) {
    int count = 0;
    for (int val = 1; val <= 9; val++) {
        if (has_candidate(game, r, c, val)) {
            count++;
        }
    }
    return count;
}