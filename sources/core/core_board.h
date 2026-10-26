#ifndef CORE_BOARD_H
#define CORE_BOARD_H

#include <stdint.h>
#include <stdbool.h>

#define BOARD_SIZE 9
#define SUBGRID_SIZE 3

/* Masque binaire avec les bits 1 à 9 positionnés à 1 (0x03FE = 0b0000 0011 1111 1110) */
#define FULL_CANDIDATES_MASK 0x03FE

/**
 * @brief Représente une case individuelle du Sudoku.
 */
typedef struct {
    uint8_t value;         /* Chiffre posé (1-9) ou 0 si la case est vide */
    uint16_t candidates;   /* Masque binaire des candidats actifs (bits 1 à 9) */
} BoardCell;

/**
 * @brief Représente l'état complet du plateau de jeu 9x9.
 */
typedef struct {
    BoardCell cells[BOARD_SIZE][BOARD_SIZE];
} SudokuBoard;

/* --- Initialisation et chargement --- */
void board_init(SudokuBoard *board);
void board_load_grid(SudokuBoard *board, const uint8_t grid[BOARD_SIZE][BOARD_SIZE]);

/* --- Manipulation bitwise des candidats --- */
void board_set_candidate(SudokuBoard *board, int row, int col, uint8_t val);
void board_remove_candidate(SudokuBoard *board, int row, int col, uint8_t val);
bool board_has_candidate(const SudokuBoard *board, int row, int col, uint8_t val);
uint8_t board_count_candidates(const SudokuBoard *board, int row, int col);
uint8_t board_get_single_candidate(const SudokuBoard *board, int row, int col);

/* --- Mises à jour de la grille --- */
void board_set_value(SudokuBoard *board, int row, int col, uint8_t val);
void board_clear_cell(SudokuBoard *board, int row, int col);
void board_recompute_all_candidates(SudokuBoard *board);

#endif /* CORE_BOARD_H */