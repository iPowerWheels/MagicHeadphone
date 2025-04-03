CC = gcc

CFLAGS = -Wall -Wextra -Werror -ggdb -O2 -std=gnu99 \
         -fno-omit-frame-pointer -D_FORTIFY_SOURCE=2 -fstack-protector-all \
         -fPIE -pie -fsanitize=address,undefined

LIBS = -lsndfile -lm -lglfw -lpthread

SRCS = main.c core.c
OBJS = $(SRCS:.c=.o)
TARGET = MagicHeadphone

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) core *.core *.out

.PHONY: all clean run
