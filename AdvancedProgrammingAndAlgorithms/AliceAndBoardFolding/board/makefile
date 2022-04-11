CFLAGS ?= -std=c++98 -Wall -Wextra -Iinclude

ALL = Board

all: $(ALL)

clean:
	rm -f Board

Board: BoardFolding.cpp
	g++ $(CFLAGS) -o Board BoardFolding.cpp
