CC = gcc
CFLAGS = -Wall -Wextra -fPIC -g 
LDFLAGS = -shared

SRC = loglite.c cJSON.c  # 添加了cjson.c
OBJ = $(SRC:.c=.o)
LIB = libloglite.so

all: $(LIB)

$(LIB): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(LIB)

