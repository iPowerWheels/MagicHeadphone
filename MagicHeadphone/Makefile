CC = gcc

CFLAGS = -Wall -Wextra -Werror -ggdb -O2 -std=gnu99 \
         -fno-omit-frame-pointer -D_FORTIFY_SOURCE=2 -fstack-protector-all \
         -fPIE -pie -fsanitize=address,undefined

LIBS = -lm -lpthread -lsndfile

# Fuentes y binario para MagicHeadphone
SRCS = main.c core.c
OBJS = $(SRCS:.c=.o)
TARGET = MagicHeadphone

# Fuente y ejecutable para mix_wav con -lsndfile
MIX_SRC = mix_wav.c
MIX_TARGET = mix_wav

all: $(TARGET) $(MIX_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(MIX_TARGET): $(MIX_SRC)
	$(CC) $(CFLAGS) -o $(MIX_TARGET) $(MIX_SRC) -lsndfile

run: $(TARGET) $(MIX_TARGET)
	./$(TARGET)
	./$(MIX_TARGET) output-final.wav output-1.wav output-2.wav output-3.wav output-4.wav output-5.wav output-6.wav

clean:
	rm -f $(OBJS) $(TARGET) $(MIX_TARGET) core *.core *.out

.PHONY: all clean run

