#Makefile

TARGET=p14
OBJECT1=p14.c
OBJECT2=p14.o

$(TARGET): $(OBJECT2)
	gcc -o $@ $^ -pthread
$(OBJECT): $(OBJECT1) 
	gcc $^

.PHONY: clean
clean:
	rm *.o $(TARGET)
