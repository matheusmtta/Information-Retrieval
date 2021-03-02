CC=g++
CFLAGS=-Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer -g
EXEC=./main
LIBS=`pkg-config --cflags --libs gumbo`
BIN = bin

$(EXEC): src/main.cpp $(BIN)/parser.o $(BIN)/invertedlist.o $(BIN)/dictionary.o
	$(CC) $(CFLAGS) src/main.cpp $(BIN)/parser.o $(BIN)/invertedlist.o $(BIN)/dictionary.o $(LIBS) -o $(EXEC)

$(BIN)/parser.o: src/parser.cpp
	$(CC) $(CFLAGS) -c src/parser.cpp $(LIBS) -o $(BIN)/parser.o$ 

$(BIN)/invertedlist.o: src/invertedlist.cpp
	$(CC) $(CFLAGS) -c src/invertedlist.cpp $(LIBS) -o $(BIN)/invertedlist.o$ 

$(BIN)/dictionary.o: src/dictionary.cpp
	$(CC) $(CFLAGS) -c src/dictionary.cpp $(LIBS) -o $(BIN)/dictionary.o$ 

clean:
	rm -rf $(BIN)/parser.o $(BIN)/invertedlist.o $(BIN)/dictionary.o main 