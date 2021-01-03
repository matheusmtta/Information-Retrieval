CC=g++
CFLAGS=-Wall -Wextra
EXEC=./main
LIBS=-lchilkat-9.5.0 -lresolv -lpthread
CHILKAT=../chilkat-9.5.0-x86_64-linux-gcc#link for chilkat lib

$(EXEC): main.cpp $(BIN)webpage.o
	export LD_LIBRARY_PATH=$(CHILKAT)/lib:$$LD_LIBRARY_PATH; \
	$(CC) $(CFLAGS) main.cpp $(BIN)webpage.o -I $(CHILKAT)/include -L $(CHILKAT)/lib $(LIBS) -o $(EXEC)

$(BIN)/webpage.o: webpage.cpp
	$(CC) $)(CFLAGS) -c webpage.cpp -o $(BIN)webpage.o$ 


run:
	export LD_LIBRARY_PATH=$(CHILKAT)/lib:$$LD_LIBRARY_PATH; \
	./main
