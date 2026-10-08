#ifndef FIND_HIDDEN_SINGLE_H
#define FIND_HIDDEN_SINGLE_H

#include <stdbool.h>
#include "game.h"

// Cherche un chiffre qui n'a qu'un emplacement possible sur sa ligne.
bool find_hidden_single(const SudokuGame *game);

#endif