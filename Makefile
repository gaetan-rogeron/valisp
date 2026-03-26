.PHONY: debug

CC = gcc
OPTIONS = -Wall -ansi -pedantic
BIBLIO=-lreadline
DEBUG_OPTIONS =
EXECUTABLE = valisp
# Ne pas modifier ci-dessous


SOURCE = $(wildcard *.c)
OBJETS = $(SOURCE:.c=.o) parseur.o

all: $(EXECUTABLE)

debug: DEBUG_OPTIONS = -g
debug: $(EXECUTABLE)


$(EXECUTABLE): $(OBJETS)
	$(CC)  $(DEBUG_OPTIONS) $(OPTIONS) $(OBJETS) $(BIBLIO) -o $(EXECUTABLE)



%.o: %.c
	$(CC)  $(DEBUG_OPTIONS) $(OPTIONS) -MMD -c $<

clean:
	@if [ -f "parseur.c" ];    \
	then                        \
	    rm -vf *.o *.d;          \
	else                          \
	    cp parseur.o parseur.sauv; \
	    rm -vf *.o *.d;             \
	    mv parseur.sauv parseur.o;   \
	fi;

-include *.d
