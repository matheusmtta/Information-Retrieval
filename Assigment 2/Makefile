CC=g++
CFLAGS=-Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer -g
EXEC=./main
LIBS=-lchilkat-9.5.0 -lresolv -lpthread
CHILKAT=../chilkat-9.5.0-x86_64-linux-gcc#link for chilkat lib
BIN = bin

$(EXEC): src/main.cpp $(BIN)/taskurl.o $(BIN)/webpage.o
	export LD_LIBRARY_PATH=$(CHILKAT)/lib:$$LD_LIBRARY_PATH; \
	$(CC) $(CFLAGS) src/main.cpp $(BIN)/taskurl.o $(BIN)/webpage.o -I $(CHILKAT)/include -L $(CHILKAT)/lib $(LIBS) -o $(EXEC)

$(BIN)/taskurl.o: src/taskurl.cpp
	$(CC) $(CFLAGS) -c src/taskurl.cpp -o $(BIN)/taskurl.o$ 

$(BIN)/webpage.o: src/webpage.cpp
	$(CC) $(CFLAGS) -c src/webpage.cpp -o $(BIN)/webpage.o$ 


run:
	export LD_LIBRARY_PATH=$(CHILKAT)/lib:$$LD_LIBRARY_PATH; \
	./main

clean:
	rm -rf $(BIN)/webpage.o $(BIN)/taskurl.o main