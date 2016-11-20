# instal libxslt1-dev

CC=gcc
CFLAGS=-std=c99 -Wall -Werror -pedantic -pedantic-errors -Wextra -Wshadow -Wpointer-arith -Wcast-qual \
        -Wstrict-prototypes -Wmissing-prototypes
PRG_NAME = xml_repair.exe
SRC_NAME = xml_repair.c

all: $(PRG_NAME)

$(PRG_NAME): $(SRC_NAME)
	$(CC) $(CFLAGS) $(SRC_NAME) -o $(PRG_NAME)

run: $(PRG_NAME)
	./$(PRG_NAME) pok.xml "</cd>" "</catalog>"

clean:
	rm -f $(PRG_NAME)
