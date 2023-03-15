# Makefile
TARGET=p2.out
$(TARGET): main.o plus.o minus.o
	gcc -o $(TARGET) $^
main.o: main.c
	gcc -c main.c
plus.o: plus.c
	gcc -c plus.c
minus.o: minus.c
	gcc -c minus.c
clean:
	rm main.o plus.o minus.o $(TARGET)
