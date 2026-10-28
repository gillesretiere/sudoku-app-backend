# Compilateur
CC = gcc

# Options de compilation C11 avec tous les avertissements activés
CFLAGS = -Wall -Wextra -std=c11 -O2 -I. -Isources -Isources/core -Isources/solver -Isources/generator

# Fichiers sources des 4 modules de la Phase 2
SRCS_PHASE2 = sources/core/core_board.c \
              sources/solver/solver_dlx.c \
              sources/solver/solver_eval.c \
              sources/generator/generator_royle.c

# Objets pour la Phase 2
OBJS_PHASE2 = $(SRCS_PHASE2:.c=.o)

# Cibles d'exécution
TARGET_TEST = test_phase2

# Règle par défaut
all: $(TARGET_TEST)

# Compilation du binaire de test
$(TARGET_TEST): $(OBJS_PHASE2) sources/test_phase2.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

# Règle générique de compilation des fichiers .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJS_PHASE2) sources/test_phase2.o $(TARGET_TEST)

.PHONY: all clean