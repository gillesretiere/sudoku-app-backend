# Compilateur C
CC = gcc

# Options de compilation : activation de GAME_MODE pour ignorer le main() de sudoku_gen.c
CFLAGS = -Wall -Wextra -std=c11 -DGAME_MODE -I. -Isources -Isources/Game -Isources/Generator

# Bibliothèques pour Raylib sur Linux
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Fichiers sources du moteur de jeu
SRCS_GAME = sources/sudoku_gui.c \
            sources/Game/game.c \
            sources/Game/candidates.c \
            sources/Game/bank_loader.c \
            sources/Game/grid_transform.c \
            sources/Game/find_naked_single.c \
            sources/Game/find_hidden_single.c \
            sources/Game/find_naked_pair.c \
            sources/Game/find_hidden_pair.c \
            sources/Game/find_pointing_pairs.c \
            sources/Game/find_box_line_reduction.c \
            sources/Game/find_naked_triple.c \
            sources/Game/find_x_wing.c \
            sources/Game/find_y_wing.c

# Inclut TOUS les fichiers du sous-dossier Generator
SRCS_GEN = $(wildcard sources/Generator/*.c)

# Fusion de l'ensemble des sources
SRCS = $(SRCS_GAME) $(SRCS_GEN)
OBJS = $(SRCS:.c=.o)

# Nom du programme binaire
TARGET = sudoku_gui

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	find sources -name "*.o" -delete

.PHONY: all clean