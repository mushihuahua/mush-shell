CC = g++

output: main.o builtins.o
	$(CC) main.o builtins.o -o output

main.o: src/main.cpp
	$(CC) -c src/main.cpp

builtins.o: src/builtins.cpp
	$(CC) -c src/builtins.cpp

clean:
	rm *.o output
