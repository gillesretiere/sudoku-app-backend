# Compilateur
CC = gcc

# Options de compilation C11
CFLAGS = -Wall -Wextra -std=c11 -O2 -I. -Isources -Isources/core -Isources/solver -Isources/generator

# Fichiers sources des 4 modules de la Phase 2
SRCS_PHASE2 = sources/core/core_board.c \
              sources/solver/solver_dlx.c \
              sources/solver/solver_eval.c \
              sources/generator/generator_royle.c

# Objets compilés
OBJS_PHASE2 = $(SRCS_PHASE2:.c=.o)

# Binaires cibles
TARGET_TEST = test_phase2
TARGET_BENCHMARK = test_benchmark

# Règle par défaut
all: $(TARGET_TEST) $(TARGET_BENCHMARK)

# Compilation du binaire de test unitaire
$(TARGET_TEST): $(OBJS_PHASE2) sources/test_phase2.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

# Compilation du binaire de benchmark
$(TARGET_BENCHMARK): $(OBJS_PHASE2) sources/test_benchmark.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

# Règle générique de compilation .c -> .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJS_PHASE2) sources/test_phase2.o sources/test_benchmark.o $(TARGET_TEST) $(TARGET_BENCHMARK)

.PHONY: all clean