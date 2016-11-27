# instal libxslt1-dev

CC=gcc
CFLAGS=-std=c99 -Wall -Werror -pedantic -pedantic-errors -Wextra -Wshadow -Wpointer-arith -Wcast-qual \
        -Wstrict-prototypes -Wmissing-prototypes
CFLAGS += $(shell xml2-config --cflags)
LIBS = $(shell xml2-config --libs)
SRC_NAME = get_element.c
PRG_NAME =$(SRC_NAME:.c=.exe)

all: $(PRG_NAME)

$(PRG_NAME): $(SRC_NAME)
	$(CC) $(CFLAGS) $(LIBS) -lxslt $(SRC_NAME) -o $(PRG_NAME)

run: $(PRG_NAME)
	./$(PRG_NAME) cdcatalog.xml title

clean:
	rm -f $(PRG_NAME)
