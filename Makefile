CC=gcc
OPTS=-O2 -Wall
LIMELIB=../../utils/limelib.so

all:
	$(CC) $(OPTS) whitespace_vm.c $(LIMELIB) -o lwsvm && $(CC) $(OPTS) wsa.c $(LIMELIB) -o lwsa

wsa:
	./lwsa
