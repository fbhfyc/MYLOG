CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC -I../cjson
LDFLAGS = -shared

SRC = loglite.c cjson/cJSON.c  # 添加了cjson.c
OBJ = $(SRC:.c=.o)
LIB = libloglite.so

all: $(LIB)

$(LIB): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ -I../cjson

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<  -I../cjson

clean:
	rm -f $(OBJ) $(LIB)

