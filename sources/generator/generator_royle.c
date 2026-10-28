#include "../solver/solver_dlx.h"
#include "../solver/solver_eval.h"
#include "generator_royle.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int r1, c1;
    int r2, c2;
} SymmetricPair;

static void shuffle_pairs(SymmetricPair *pairs, int count) {
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        SymmetricPair temp = pairs[i];
        pairs[i] = pairs[j];
        pairs[j] = temp;
    }
}

static void shuffle_digits(uint8_t *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        uint8_t temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

static bool fill_board_random(SudokuBoard *board, int row, int col) {
    if (row == BOARD_SIZE) return true;
    if (col == BOARD_SIZE) return fill_board_random(board, row + 1, 0);

    uint8_t digits[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    shuffle_digits(digits, 9);

    for (int i = 0; i < 9; i++) {
        uint8_t val = digits[i];
        bool valid = true;

        for (int k = 0; k < BOARD_SIZE; k++) {
            if (board->cells[row][k].value == val || board->cells[k][col].value == val) {
                valid = false;
                break;
            }
        }

        if (valid) {
            int box_r = (row / 3) * 3;
            int box_c = (col / 3) * 3;
            for (int br = 0; br < 3 && valid; br++) {
                for (int bc = 0; bc < 3; bc++) {
                    if (board->cells[box_r + br][box_c + bc].value == val) {
                        valid = false;
                        break;
                    }
                }
            }
        }

        if (valid) {
            board_set_value(board, row, col, val);
            if (fill_board_random(board, row, col + 1)) return true;
            board_set_value(board, row, col, 0);
        }
    }
    return false;
}

void generator_royle_generate_full_grid(SudokuBoard *board) {
    board_init(board);
    fill_board_random(board, 0, 0);
}

bool generator_royle_generate_puzzle(const GeneratorTarget *target, SudokuBoard *out_board) {
    if (!target || !out_board) return false;

    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    int attempts = 0;
    int max_attempts = (target->max_attempts > 0) ? target->max_attempts : 1000;

    while (attempts < max_attempts) {
        attempts++;

        SudokuBoard full_board;
        generator_royle_generate_full_grid(&full_board);
        SudokuBoard work_board = full_board;

        SymmetricPair pairs[41];
        int pair_count = 0;

        pairs[pair_count++] = (SymmetricPair){4, 4, 4, 4};

        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                int sym_r = 8 - r;
                int sym_c = 8 - c;
                if (r < sym_r || (r == sym_r && c < sym_c)) {
                    pairs[pair_count++] = (SymmetricPair){r, c, sym_r, sym_c};
                }
            }
        }

        shuffle_pairs(pairs, pair_count);
        int current_clues = 81;

        for (int i = 0; i < pair_count; i++) {
            SymmetricPair p = pairs[i];
            uint8_t backup1 = work_board.cells[p.r1][p.c1].value;
            uint8_t backup2 = work_board.cells[p.r2][p.c2].value;

            board_set_value(&work_board, p.r1, p.c1, 0);
            if (p.r1 != p.r2 || p.c1 != p.c2) {
                board_set_value(&work_board, p.r2, p.c2, 0);
            }

            if (solver_dlx_has_unique_solution(&work_board, NULL)) {
                current_clues -= (p.r1 == p.r2 && p.c1 == p.c2) ? 1 : 2;
            } else {
                board_set_value(&work_board, p.r1, p.c1, backup1);
                if (p.r1 != p.r2 || p.c1 != p.c2) {
                    board_set_value(&work_board, p.r2, p.c2, backup2);
                }
            }
        }

        SudokuEvaluation eval;
        solver_eval_analyze(&work_board, &eval);

        if (eval.is_solvable && 
            eval.max_se_rating >= target->min_se && 
            eval.max_se_rating <= target->max_se && 
            current_clues <= target->max_clues) {
            
            *out_board = work_board;
            return true;
        }
    }

    return false;
}