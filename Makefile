DESTDIR?=/usr/local

ifneq ($V,1)
Q ?= @
endif

CC	= gcc
CFLAGS	= $(DEBUG) -Wall -Wextra $(INCLUDE) -Winline -pipe 

LDFLAGS	= -L$(DESTDIR)/lib
LIBS    = -lpthread -lrt -lm -lcrypt

SRC	=	src/pcar.c src/comm.c src/thread.c src/relay.c src/input.c

OBJ	=	$(SRC:.c=.o)

all:	pcar

pcar:	$(OBJ)
	$Q echo [Link]
	$Q $(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)

.c.o:
	$Q echo [Compile] $<
	$Q $(CC) -c $(CFLAGS) $< -o $@

.PHONY:	clean
clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJ) pcar *~ core tags *.bak

.PHONY:	install
install: pcar
ifneq ($(shell id -u),0)
	$Q echo "Must be root! (sudo make install)"
	$Q exit 1
endif
	$Q echo "[Install]"
	$Q install -D -m 4755 -o root pcar $(DESTDIR)/bin
	$Q install -D -m 4755 -o root stream_deck/start_stream_deck $(DESTDIR)/bin
	$Q mkdir -p $(DESTDIR)/share/pcar
	$Q cp -r stream_deck/Assets $(DESTDIR)/share/pcar/Assets
	$Q install -D -m 4755 -o root pcar_startup $(DESTDIR)/bin


.PHONY:	uninstall
uninstall:
	$Q echo "[UnInstall]"
	$Q rm -f $(DESTDIR)/bin/pcar
	$Q rm -f $(DESTDIR)/man/man1/pcar.1
