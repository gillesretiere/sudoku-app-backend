# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O3 -fPIC -I. -Isources -Isources/core -Isources/solver -Isources/generator

# Fichiers sources du moteur C11
SRCS = sources/core/core_board.c \
       sources/solver/solver_dlx.c \
       sources/solver/solver_eval.c \
       sources/generator/generator_royle.c \
       sources/sudoku_api.c

OBJS = $(SRCS:.c=.o)

# Fichier dynamique cible pour macOS
DYLIB = libsudoku.dylib

all: $(DYLIB) test_benchmark

# Règle pour générer la bibliothèque dynamique macOS (.dylib)
$(DYLIB): $(OBJS)
	$(CC) -dynamiclib -o $@ $^ -lm

# Règle pour l'exécutable de benchmark
test_benchmark: sources/test_benchmark.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(DYLIB) test_benchmark

.PHONY: all clean
