#include "solver_dlx.h"
#include <stdlib.h>
#include <string.h>

#define NUM_COLUMNS 324
#define MAX_NODES (NUM_COLUMNS + 1 + 729 * 4)

typedef struct DLXNode {
    struct DLXNode *left;
    struct DLXNode *right;
    struct DLXNode *up;
    struct DLXNode *down;
    struct DLXNode *col;
    int size;     /* Taille de la colonne (utilisé pour l'en-tête) */
    int row_id;   /* Identifiant du choix (r * 81 + c * 9 + (d - 1)) */
} DLXNode;

typedef struct {
    DLXNode node_pool[MAX_NODES];
    int node_count;
    DLXNode root;
    DLXNode col_nodes[NUM_COLUMNS];
    int solution_count;
    int max_solutions;
    uint8_t solution_grid[BOARD_SIZE][BOARD_SIZE];
    DLXNode *solution_nodes[81];
} DLXContext;

/* --- Opérations sur les Dancing Links --- */

static void dlx_cover(DLXNode *c) {
    c->right->left = c->left;
    c->left->right = c->right;
    for (DLXNode *i = c->down; i != c; i = i->down) {
        for (DLXNode *j = i->right; j != i; j = j->right) {
            j->down->up = j->up;
            j->up->down = j->down;
            j->col->size--;
        }
    }
}

static void dlx_uncover(DLXNode *c) {
    for (DLXNode *i = c->up; i != c; i = i->up) {
        for (DLXNode *j = i->left; j != i; j = j->left) {
            j->col->size++;
            j->down->up = j;
            j->up->down = j;
        }
    }
    c->right->left = c;
    c->left->right = c;
}

static DLXNode *dlx_choose_column(DLXContext *ctx) {
    int min_size = 999999;
    DLXNode *best_col = NULL;
    for (DLXNode *c = ctx->root.right; c != &ctx->root; c = c->right) {
        if (c->size < min_size) {
            min_size = c->size;
            best_col = c;
        }
    }
    return best_col;
}

static void dlx_search(DLXContext *ctx, int k) {
    if (ctx->root.right == &ctx->root) {
        ctx->solution_count++;
        if (ctx->solution_count == 1) {
            for (int i = 0; i < k; i++) {
                int r_id = ctx->solution_nodes[i]->row_id;
                int val = (r_id % 9) + 1;
                int tmp = r_id / 9;
                int c = tmp % 9;
                int r = tmp / 9;
                ctx->solution_grid[r][c] = val;
            }
        }
        return;
    }

    DLXNode *c = dlx_choose_column(ctx);
    if (!c || c->size == 0) return;

    dlx_cover(c);

    for (DLXNode *r = c->down; r != c; r = r->down) {
        ctx->solution_nodes[k] = r;

        for (DLXNode *j = r->right; j != r; j = j->right) {
            dlx_cover(j->col);
        }

        dlx_search(ctx, k + 1);

        if (ctx->solution_count >= ctx->max_solutions) {
            return; /* Interruption précoce si le quota est atteint */
        }

        for (DLXNode *j = r->left; j != r; j = j->left) {
            dlx_uncover(j->col);
        }
    }

    dlx_uncover(c);
}

static void dlx_build_matrix(DLXContext *ctx, const SudokuBoard *board) {
    memset(ctx, 0, sizeof(DLXContext));

    ctx->root.left = &ctx->root;
    ctx->root.right = &ctx->root;
    ctx->root.up = &ctx->root;
    ctx->root.down = &ctx->root;

    DLXNode *last_col = &ctx->root;

    for (int i = 0; i < NUM_COLUMNS; i++) {
        DLXNode *c = &ctx->col_nodes[i];
        c->size = 0;
        c->row_id = -1;
        c->col = c;
        c->up = c;
        c->down = c;

        c->left = last_col;
        c->right = &ctx->root;
        last_col->right = c;
        ctx->root.left = c;

        last_col = c;
    }

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            uint8_t cell_val = board->cells[r][c].value;

            for (int d = 1; d <= 9; d++) {
                if (cell_val != 0 && cell_val != d) {
                    continue; /* Ne conserver que la valeur posée si la case est renseignée */
                }

                int row_id = r * 81 + c * 9 + (d - 1);
                int constraints[4];
                constraints[0] = r * 9 + c;
                constraints[1] = 81 + r * 9 + (d - 1);
                constraints[2] = 162 + c * 9 + (d - 1);
                constraints[3] = 243 + ((r / 3) * 3 + (c / 3)) * 9 + (d - 1);

                DLXNode *first_node = NULL;
                DLXNode *last_node = NULL;

                for (int i = 0; i < 4; i++) {
                    int col_idx = constraints[i];
                    DLXNode *col_head = &ctx->col_nodes[col_idx];

                    DLXNode *new_node = &ctx->node_pool[ctx->node_count++];
                    new_node->row_id = row_id;
                    new_node->col = col_head;

                    new_node->down = col_head;
                    new_node->up = col_head->up;
                    col_head->up->down = new_node;
                    col_head->up = new_node;

                    col_head->size++;

                    if (!first_node) {
                        first_node = new_node;
                        first_node->left = first_node;
                        first_node->right = first_node;
                    } else {
                        new_node->left = last_node;
                        new_node->right = first_node;
                        last_node->right = new_node;
                        first_node->left = new_node;
                    }
                    last_node = new_node;
                }
            }
        }
    }
}

/* --- Fonctions publiques --- */

int solver_dlx_count_solutions(const SudokuBoard *board, int max_count) {
    if (!board || max_count <= 0) return 0;

    DLXContext ctx;
    dlx_build_matrix(&ctx, board);
    ctx.max_solutions = max_count;
    ctx.solution_count = 0;

    dlx_search(&ctx, 0);

    return ctx.solution_count;
}

bool solver_dlx_has_unique_solution(const SudokuBoard *board, uint8_t solution_out[BOARD_SIZE][BOARD_SIZE]) {
    if (!board) return false;

    DLXContext ctx;
    dlx_build_matrix(&ctx, board);
    ctx.max_solutions = 2;
    ctx.solution_count = 0;

    dlx_search(&ctx, 0);

    if (ctx.solution_count == 1 && solution_out != NULL) {
        memcpy(solution_out, ctx.solution_grid, sizeof(ctx.solution_grid));
    }

    return (ctx.solution_count == 1);
}