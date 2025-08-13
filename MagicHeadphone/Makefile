CC = gcc

CFLAGS = -Wall -Wextra -Werror -ggdb -O2 -std=gnu99 \
         -fno-omit-frame-pointer -fstack-protector-all \
         -fPIE -fsanitize=address,undefined

LIBS = -lm -lpthread -lsndfile

# Source and executable for MagicHeadphone
SRCS = main.c core.c
OBJS = $(SRCS:.c=.o)
TARGET = MagicHeadphone

# Source and executable for MagicMix
MIX_SRC = MagicMix.c
MIX_TARGET = MagicMix

# Source and executable for MagicSplit
SPLIT_SRC = MagicSplit.c
SPLIT_TARGET = MagicSplit

all: $(TARGET) $(MIX_TARGET) $(SPLIT_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(MIX_TARGET): $(MIX_SRC)
	$(CC) $(CFLAGS) -o $(MIX_TARGET) $(MIX_SRC) $(LIBS)

$(SPLIT_TARGET): $(SPLIT_SRC)
	$(CC) $(CFLAGS) -o $(SPLIT_TARGET) $(SPLIT_SRC) $(LIBS)

run: $(TARGET) $(MIX_TARGET) $(SPLIT_TARGET)
	./$(SPLIT_TARGET) input.wav
	./$(TARGET)
	./$(MIX_TARGET) output-final.wav output-1.wav output-2.wav output-3.wav output-4.wav output-5.wav output-6.wav


clean:
	rm -f $(OBJS) $(TARGET) $(MIX_TARGET) $(SPLIT_TARGET) core *.core *.out

.PHONY: all clean run
