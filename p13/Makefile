CC = gcc
TARGET = p13
CCFLAGS = -Wall -Wextra -Werror

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) $< -lpthread -lm -o $@

$(TARGET).o: $(TARGET).c
	$(CC) $(CCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(TARGET).o