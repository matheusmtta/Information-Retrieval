CC=g++
CFLAGS=-Wall -Wextra
EXEC=./main
LIBS=-lchilkat-9.5.0 -lresolv -lpthread
CHILKAT=../chilkat-9.5.0-x86_64-linux-gcc#link for chilkat lib

$(EXEC): main.cpp
	export LD_LIBRARY_PATH=$(CHILKAT)/lib:$$LD_LIBRARY_PATH; \
	$(CC) $(CFLAGS) main.cpp -I $(CHILKAT)/include -L $(CHILKAT)/lib $(LIBS) -o $(EXEC)

run:
	export LD_LIBRARY_PATH=$(CHILKAT)/lib:$$LD_LIBRARY_PATH; \
	./main
