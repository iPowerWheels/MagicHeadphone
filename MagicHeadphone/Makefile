CC = gcc

BASE_FLAGS = -Wall -Wextra -Werror -std=gnu99

DEBUG_FLAGS = -ggdb -O0 \
              -fno-omit-frame-pointer -fstack-protector-all -fPIE \
              -fsanitize=address,undefined

RELEASE_FLAGS = -Wall -Wextra -Werror -ggdb -O3 -std=gnu99 \
		-march=native \
		-fstack-protector-strong \
		-fPIE \
		-D_FORTIFY_SOURCE=2

LIBS = -lm -lpthread -lsndfile

SRCS = main.c core.c
OBJS = $(SRCS:.c=.o)
TARGET = MagicHeadphone

MIX_SRC = MagicMix.c
MIX_TARGET = MagicMix

SPLIT_SRC = MagicSplit.c
SPLIT_TARGET = MagicSplit

all: release

# ===== BUILD RELEASE =====
release: CFLAGS = $(BASE_FLAGS) $(RELEASE_FLAGS)
release: $(TARGET) $(MIX_TARGET) $(SPLIT_TARGET)

# ===== BUILD DEBUG =====
debug: CFLAGS = $(BASE_FLAGS) $(DEBUG_FLAGS)
debug: $(TARGET) $(MIX_TARGET) $(SPLIT_TARGET)

# ===== RULES =====
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(MIX_TARGET): $(MIX_SRC)
	$(CC) $(CFLAGS) -o $(MIX_TARGET) $(MIX_SRC) $(LIBS)

$(SPLIT_TARGET): $(SPLIT_SRC)
	$(CC) $(CFLAGS) -o $(SPLIT_TARGET) $(SPLIT_SRC) $(LIBS)

# ===== RUN =====
run: release
	./$(SPLIT_TARGET) input.wav
	./$(TARGET)
	./$(MIX_TARGET) output-final.wav output-1.wav output-2.wav output-3.wav output-4.wav output-5.wav output-6.wav

# ===== CLEAN =====
clean:
	rm -f $(OBJS) $(TARGET) $(MIX_TARGET) $(SPLIT_TARGET) core *.core *.out

.PHONY: all clean run debug release
