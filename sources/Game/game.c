#include <stdio.h>
#include "game.h"

/**
 * Initialise une partie avec une grille de test temporaire.
 */
void init_game(SudokuGame *game) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            game->initial_grid[i][j] = 0;
            game->player_grid[i][j] = 0;
            game->solution[i][j] = 0;
        }
    }

    /* Exemple simple pour tester : une case de départ fixe en (0,0) */
    game->initial_grid[0][0] = 5;
    game->player_grid[0][0] = 5;
}

/**
 * Affiche la grille dans le terminal avec les coordonnées (lignes et colonnes).
 */
void print_game_board(const SudokuGame *game) {
    printf("\n    1 2 3   4 5 6   7 8 9\n");
    printf("  +-------+-------+-------+\n");
    
    for (int i = 0; i < 9; i++) {
        printf("%d | ", i + 1);
        for (int j = 0; j < 9; j++) {
            int val = game->player_grid[i][j];
            if (val == 0) {
                printf(". ");
            } else {
                printf("%d ", val);
            }
            if ((j + 1) % 3 == 0) {
                printf("| ");
            }
        }
        printf("\n");
        if ((i + 1) % 3 == 0) {
            printf("  +-------+-------+-------+\n");
        }
    }
    printf("\n");
}

/**
 * Valide et applique le coup du joueur.
 */
bool make_move(SudokuGame *game, int row, int col, int value) {
    if (row < 0 || row > 8 || col < 0 || col > 8) {
        printf("⚠️ Erreur : La ligne et la colonne doivent etre entre 1 et 9.\n");
        return false;
    }

    if (game->initial_grid[row][col] != 0) {
        printf("⚠️ Erreur : La case (%d, %d) est un chiffre de depart fixe !\n", row + 1, col + 1);
        return false;
    }

    if (value < 0 || value > 9) {
        printf("⚠️️ Erreur : Le chiffre doit etre entre 1 et 9 (ou 0 pour effacer).\n");
        return false;
    }

    game->player_grid[row][col] = (char)value;
    return true;
}

/**
 * Vérifie si la grille est entièrement remplie.
 */
bool check_victory(const SudokuGame *game) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (game->player_grid[i][j] == 0) {
                return false; /* Il reste des cases vides */
            }
        }
    }
    return true;
}