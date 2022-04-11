CFLAGS ?= -std=c++98 -Wall -Wextra -Iinclude

ALL = FATRW

all: $(ALL)

clean:
	rm -f obj/* bin/*

obj/rw.o: src/rw.cpp
	g++ $(CLFAGS) -c -o obj/rw.o src/rw.cpp

FATRW: obj/rw.o
	g++ $(CFLAGS) -o FATRW obj/rw.o
