CFLAGS ?= -std=c++98 -Wall -Wextra -Iinclude

ALL = Alice

all: $(ALL)

clean:
	rm -f Alice

#obj/AliceGame.o: AliceGame.cpp
#	g++ $(CFLAGS) -c -o AliceGame.o Alicegame.cpp

Alice: AliceGame.cpp
	g++ $(CFLAGS) -o Alice AliceGame.cpp
