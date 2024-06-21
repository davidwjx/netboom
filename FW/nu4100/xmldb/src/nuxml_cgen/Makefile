CC =		cc
CFLAGS =	-O -I/usr/local/include -std=c99 -Wall -Wno-format-security -Wpointer-arith -Wshadow -Wcast-align -Wstrict-prototypes -Wredundant-decls -Wno-long-long
LDFLAGS =	-s -L/usr/local/lib
LIBS =		

all:		nuxml_cgen

nuxml_cgen:		nuxml_cgen.c sxmlc.c
	$(CC) $(CFLAGS) nuxml_cgen.c sxmlc.c $(LDFLAGS) $(LIBS) -o nuxml_cgen


clean:
	rm -f nuxml_cgen *.o core core.* *.core
