CC = gcc
CFLAGS = -g -Wall -I../cLog

# 目标文件
TARGET = clogtest

# 源文件
SRCS = clogtest.c

# 默认目标
all: $(TARGET)

# 编译目标文件
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# 清理生成的文件
clean:
	$(RM) $(TARGET)

