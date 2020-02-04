CC=g++
CCFLAGS=-Wall -Wextra -Werror -pedantic -march=x86-64 -lsfml-window -lsfml-audio -lsfml-system -lsfml-graphics

main: bubblewrap.cpp
	$(CC) $(CCFLAGS) -g bubblewrap.cpp -o bubblewrap.bin

clean:
	rm -f *.o
	echo "Pulizia Completa"

