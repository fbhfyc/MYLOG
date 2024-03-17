CC = gcc
CFLAGS = -g -Wall -I../cLog
LDFLAGS = -L../cLog -lloglite 

# 目标文件
TARGET = test

# 源文件
SRCS = clogtest.c

# 默认目标
all: $(TARGET)

# 编译目标文件
$(TARGET): $(SRCS:.c=.o)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 编译源文件
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# 清理生成的文件
clean:
	$(RM) $(TARGET) $(SRCS:.c=.o)

