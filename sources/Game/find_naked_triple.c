#include <stdio.h>
#include <stdbool.h>
#include "find_naked_triple.h"
#include "candidates.h"

static int count_bits(uint16_t mask) {
    int count = 0;
    for (int v = 1; v <= 9; v++) {
        if (mask & (1 << v)) count++;
    }
    return count;
}

bool find_naked_triple(const SudokuGame *game) {
    SudokuGame temp = *game;
    compute_autonote(&temp);

    for (int unit_type = 0; unit_type < 3; unit_type++) {
        for (int u = 0; u < 9; u++) {
            int cells[9][2];
            for (int i = 0; i < 9; i++) {
                if (unit_type == 0)      { cells[i][0] = u; cells[i][1] = i; }
                else if (unit_type == 1) { cells[i][0] = i; cells[i][1] = u; }
                else                     { cells[i][0] = (u / 3) * 3 + i / 3; cells[i][1] = (u % 3) * 3 + i % 3; }
            }

            // Combinaison de 3 cases distinctes (i, j, k)
            for (int i = 0; i < 7; i++) {
                int r1 = cells[i][0], c1 = cells[i][1];
                if (temp.player_grid[r1][c1] != 0) continue;
                uint16_t m1 = temp.candidates[r1][c1];
                if (count_bits(m1) > 3 || count_bits(m1) < 2) continue;

                for (int j = i + 1; j < 8; j++) {
                    int r2 = cells[j][0], c2 = cells[j][1];
                    if (temp.player_grid[r2][c2] != 0) continue;
                    uint16_t m2 = temp.candidates[r2][c2];
                    if (count_bits(m2) > 3 || count_bits(m2) < 2) continue;

                    for (int k = j + 1; k < 9; k++) {
                        int r3 = cells[k][0], c3 = cells[k][1];
                        if (temp.player_grid[r3][c3] != 0) continue;
                        uint16_t m3 = temp.candidates[r3][c3];
                        if (count_bits(m3) > 3 || count_bits(m3) < 2) continue;

                        // Masque combiné des 3 cases
                        uint16_t combined = m1 | m2 | m3;

                        if (count_bits(combined) == 3) {
                            bool can_eliminate = false;
                            for (int idx = 0; idx < 9; idx++) {
                                if (idx == i || idx == j || idx == k) continue;
                                int r = cells[idx][0], c = cells[idx][1];
                                if (temp.player_grid[r][c] == 0 && (temp.candidates[r][c] & combined) != 0) {
                                    can_eliminate = true;
                                    break;
                                }
                            }

                            if (can_eliminate) {
                                const char *unit_names[] = {"la ligne", "la colonne", "le bloc 3x3"};

                                printf("📖 THÉORIE — Triplet Nu (Naked Triple) :\n");
                                printf("   Lorsque 3 cases d'une même unité partagent un total de 3 candidats,\n");
                                printf("   ces 3 chiffres occupent nécessairement ces 3 cases.\n");
                                printf("   Ils peuvent donc être éliminés des autres cases de cette unité.\n\n");

                                printf("🎯 APPLICATION :\n");
                                printf("   Dans %s %d, les cases (%d, %d), (%d, %d) et (%d, %d)\n",
                                       unit_names[unit_type], u + 1, r1 + 1, c1 + 1, r2 + 1, c2 + 1, r3 + 1, c3 + 1);
                                printf("   forment un Triplet Nu.\n");
                                printf("   👉 Action : Éliminez ces candidats des autres cases de %s.\n", unit_names[unit_type]);
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}