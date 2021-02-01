CC=g++
CFLAGS=-Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer -g
EXEC=./main
LIBS=`pkg-config --cflags --libs gumbo`
BIN = bin

$(EXEC): src/main.cpp $(BIN)/parser.o $(BIN)/invertedlist.o $(BIN)/display.o
	$(CC) $(CFLAGS) src/main.cpp $(BIN)/parser.o $(BIN)/invertedlist.o $(BIN)/display.o $(LIBS) -o $(EXEC)

$(BIN)/parser.o: src/parser.cpp
	$(CC) $(CFLAGS) -c src/parser.cpp $(LIBS) -o $(BIN)/parser.o$ 

$(BIN)/invertedlist.o: src/invertedlist.cpp
	$(CC) $(CFLAGS) -c src/invertedlist.cpp $(LIBS) -o $(BIN)/invertedlist.o$ 

$(BIN)/display.o: src/display.cpp
	$(CC) $(CFLAGS) -c src/display.cpp $(BIN)/invertedlist.o $(LIBS) -o $(BIN)/display.o$ 

clean:
	rm -rf $(BIN)/parser.o $(BIN)/invertedlist.o $(BIN)/display.o main 