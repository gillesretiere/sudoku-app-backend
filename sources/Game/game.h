#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* En mode GUI, la macro GUI_MODE neutralise uniquement les printf de la version graphique */
#ifdef GUI_MODE
    #define printf(...) ((void)0)
#endif

typedef struct {
    char initial_grid[9][9];
    char player_grid[9][9];
    char solution[9][9];
    uint16_t candidates[9][9];
    
    /* Masques de surbrillance pour l'affichage */
    uint16_t highlight_red[9][9];
    uint16_t highlight_cyan[9][9];
    uint16_t highlight_magenta[9][9];
    uint16_t highlight_green[9][9];

    /* Masque mémoire des candidats éliminés par les indices */
    uint16_t eliminated_mask[9][9];
    /* Tampon mémoire pour transmettre la description de l'indice à la GUI (inactif en CLI) */
    char hint_description[512];
    
} SudokuGame;

void init_game(SudokuGame *game, int difficulty);
void print_game_board(const SudokuGame *game);
bool make_move(SudokuGame *game, int row, int col, int value);
bool check_victory(const SudokuGame *game);
void provide_hint(const SudokuGame *game);
void print_candidates_grid(const SudokuGame *game);

/* Applique l'indice courant en supprimant définitivement les candidats rouges */
bool apply_current_hint(SudokuGame *game);

void compute_autonote(SudokuGame *game);
bool has_candidate(const SudokuGame *game, int r, int c, int val);
int count_candidates(const SudokuGame *game, int r, int c);
void clear_highlights(SudokuGame *game);
// À ajouter dans game.h pour rendre la fonction visible à l'ensemble du projet :
int c_test_init_game(int level);
void c_get_grid(int level, int *out_grid);
void c_get_game_data(int level, int *out_initial, int *out_player);
int c_make_move(int row, int col, int value, int *out_player);
int c_check_completed_zone(int row, int col);
#endif