#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "core/core_board.h"
#include "solver/solver_dlx.h"
#include "solver/solver_eval.h"
#include "generator/generator_royle.h"

typedef struct {
    const char *name;
    GeneratorTarget target;
} LevelConfig;

int main(void) {
    printf("====================================================================\n");
    printf("   BENCHMARK DU MOTEUR SUDOKU PHASE 2 - 5 NIVEAUX DE DIFFICULTE     \n");
    printf("====================================================================\n\n");

    LevelConfig levels[5] = {
        {
            .name = "Niveau 1 - Debutant",
            .target = {.min_se = 1.0f, .max_se = 1.5f, .max_clues = 40, .max_attempts = 200}
        },
        {
            .name = "Niveau 2 - Intermediaire",
            .target = {.min_se = 2.0f, .max_se = 3.2f, .max_clues = 35, .max_attempts = 200}
        },
        {
            .name = "Niveau 3 - Avance",
            .target = {.min_se = 3.3f, .max_se = 4.5f, .max_clues = 31, .max_attempts = 1000}
        },
        {
            .name = "Niveau 4 - Expert",
            .target = {.min_se = 4.6f, .max_se = 5.5f, .max_clues = 27, .max_attempts = 2000}
        },
        {
            .name = "Niveau 5 - Maitre",
            .target = {.min_se = 5.6f, .max_se = 8.0f, .max_clues = 24, .max_attempts = 5000}
        }
    };

    printf("%-25s | %-8s | %-8s | %-8s | %-12s\n", "Niveau", "Statut", "Indices", "SE Max", "Temps (ms)");
    printf("--------------------------------------------------------------------\n");

    double total_time = 0.0;
    int success_count = 0;

    for (int i = 0; i < 5; i++) {
        SudokuBoard board;
        clock_t start = clock();
        
        bool success = generator_royle_generate_puzzle(&levels[i].target, &board);
        
        clock_t end = clock();
        double elapsed_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
        total_time += elapsed_ms;

        if (success) {
            success_count++;
            int clues = 0;
            for (int r = 0; r < BOARD_SIZE; r++) {
                for (int c = 0; c < BOARD_SIZE; c++) {
                    if (board.cells[r][c].value != 0) clues++;
                }
            }

            SudokuEvaluation eval;
            solver_eval_analyze(&board, &eval);

            printf("%-25s | %-8s | %-8d | %-8.1f | %-12.2f\n", 
                   levels[i].name, "[OK]", clues, eval.max_se_rating, elapsed_ms);
        } else {
            printf("%-25s | %-8s | %-8s | %-8s | %-12.2f\n", 
                   levels[i].name, "[ECHEC]", "-", "-", elapsed_ms);
        }
    }

    printf("--------------------------------------------------------------------\n");
    printf("Bilan : %d/5 niveaux generes avec succes | Temps total : %.2f ms\n", 
           success_count, total_time);
    printf("====================================================================\n");

    return (success_count == 5) ? EXIT_SUCCESS : EXIT_FAILURE;
}