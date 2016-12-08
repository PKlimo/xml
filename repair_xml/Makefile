# instal libxslt1-dev

CC=gcc
CFLAGS=-std=c99 -Wall -Werror -pedantic -pedantic-errors -Wextra -Wshadow -Wpointer-arith -Wcast-qual \
        -Wstrict-prototypes -Wmissing-prototypes
SRC_NAME = repair_xml.c
PRG_NAME =$(SRC_NAME:.c=.exe)

all: $(PRG_NAME)

$(PRG_NAME): $(SRC_NAME)
	$(CC) $(CFLAGS) $(SRC_NAME) -o $(PRG_NAME)

run: $(PRG_NAME)
	dd if=cdcatalog.xml of=pok.xml bs=1K count=2
	xmllint pok.xml > /dev/null || 	./$(PRG_NAME) pok.xml "</cd>" "</catalog>"
	echo "xml file repaired"
	xmllint pok.xml > /dev/null

clean:
	rm -f $(PRG_NAME)
	rm -f pok.xml
