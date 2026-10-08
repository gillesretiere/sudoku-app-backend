#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

/* Structure représentant l'état complet d'une partie */
typedef struct {
    char initial_grid[9][9]; /* Grille initiale (chiffres fixes) */
    char player_grid[9][9];  /* Grille actuelle jouée par l'utilisateur */
    char solution[9][9];     /* Solution complète pour la vérification */
} SudokuGame;
// Strategy functions
typedef int (*f_ptr_t)(void);
extern f_ptr_t *strat_all[];
/* Prototypes des fonctions du jeu */
void init_game(SudokuGame *game);
void print_game_board(const SudokuGame *game);
bool make_move(SudokuGame *game, int row, int col, int value);
bool check_victory(const SudokuGame *game);
// Analyse la grille du joueur et affiche un indice logique
void provide_hint(const SudokuGame *game);

#endif /* GAME_H */