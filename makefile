CC = gcc
TARGET = hw3
OBJS = hw3.c

$(TARGET): $(OBJS)
	$(CC) -g $(OBJS) -o $(TARGET)

clean:
	rm $(TARGET)
