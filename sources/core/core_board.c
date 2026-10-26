#include "core_board.h"
#include <string.h>

/**
 * @brief Initialise un plateau vide. Toutes les cases sont à 0 et possèdent tous les candidats.
 */
void board_init(SudokuBoard *board) {
    if (!board) return;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            board->cells[r][c].value = 0;
            board->cells[r][c].candidates = FULL_CANDIDATES_MASK;
        }
    }
}

/**
 * @brief Charge une grille numérique et recompile automatiquement la matrice des candidats.
 */
void board_load_grid(SudokuBoard *board, const uint8_t grid[BOARD_SIZE][BOARD_SIZE]) {
    if (!board || !grid) return;
    
    board_init(board);
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (grid[r][c] >= 1 && grid[r][c] <= 9) {
                board->cells[r][c].value = grid[r][c];
            }
        }
    }
    board_recompute_all_candidates(board);
}

/**
 * @brief Active le candidat 'val' pour une case donnée.
 */
void board_set_candidate(SudokuBoard *board, int row, int col, uint8_t val) {
    if (!board || row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return;
    if (val < 1 || val > 9) return;
    board->cells[row][col].candidates |= (1U << val);
}

/**
 * @brief Désactive le candidat 'val' pour une case donnée.
 */
void board_remove_candidate(SudokuBoard *board, int row, int col, uint8_t val) {
    if (!board || row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return;
    if (val < 1 || val > 9) return;
    board->cells[row][col].candidates &= ~(1U << val);
}

/**
 * @brief Vérifie si le candidat 'val' est présent dans la case.
 */
bool board_has_candidate(const SudokuBoard *board, int row, int col, uint8_t val) {
    if (!board || row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return false;
    if (val < 1 || val > 9) return false;
    return (board->cells[row][col].candidates & (1U << val)) != 0;
}

/**
 * @brief Retourne le nombre de candidats restants dans une case.
 */
uint8_t board_count_candidates(const SudokuBoard *board, int row, int col) {
    if (!board || row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return 0;
    return (uint8_t)__builtin_popcount(board->cells[row][col].candidates);
}

/**
 * @brief Si la case possède un unique candidat, retourne sa valeur. Sinon, retourne 0.
 */
uint8_t board_get_single_candidate(const SudokuBoard *board, int row, int col) {
    if (!board || row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return 0;
    uint16_t mask = board->cells[row][col].candidates;
    if (__builtin_popcount(mask) != 1) return 0;
    return (uint8_t)__builtin_ctz(mask);
}

/**
 * @brief Fixe la valeur d'une case et remet son masque de candidats à zéro.
 */
void board_set_value(SudokuBoard *board, int row, int col, uint8_t val) {
    if (!board || row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return;
    board->cells[row][col].value = val;
    if (val != 0) {
        board->cells[row][col].candidates = 0;
    }
}

/**
 * @brief Efface une case et recalcule l'état logique global du plateau.
 */
void board_clear_cell(SudokuBoard *board, int row, int col) {
    if (!board || row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return;
    board->cells[row][col].value = 0;
    board_recompute_all_candidates(board);
}

/**
 * @brief Recalcule les candidats de toutes les cases vides selon les contraintes du Sudoku.
 */
void board_recompute_all_candidates(SudokuBoard *board) {
    if (!board) return;

    /* 1. Réinitialisation des masques de candidats */
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (board->cells[r][c].value == 0) {
                board->cells[r][c].candidates = FULL_CANDIDATES_MASK;
            } else {
                board->cells[r][c].candidates = 0;
            }
        }
    }

    /* 2. Élimination basée sur les chiffres posés */
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            uint8_t val = board->cells[r][c].value;
            if (val == 0) continue;

            /* Retrait du candidat dans la ligne et la colonne */
            for (int i = 0; i < BOARD_SIZE; i++) {
                board_remove_candidate(board, r, i, val);
                board_remove_candidate(board, i, c, val);
            }

            /* Retrait du candidat dans le bloc 3x3 */
            int box_r = (r / SUBGRID_SIZE) * SUBGRID_SIZE;
            int box_c = (c / SUBGRID_SIZE) * SUBGRID_SIZE;
            for (int br = 0; br < SUBGRID_SIZE; br++) {
                for (int bc = 0; bc < SUBGRID_SIZE; bc++) {
                    board_remove_candidate(board, box_r + br, box_c + bc, val);
                }
            }
        }
    }
}