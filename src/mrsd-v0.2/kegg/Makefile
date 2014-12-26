CC = gcc
OPT1 =  -O3 -march=athlon-xp -fomit-frame-pointer -pipe
OPT2 = -ffast-math -fforce-mem -mpreferred-stack-boundary=2
#OPT3 = -I/usr/src/Mird-1.0.7/src
OPT4 = -s -static
CFLAGS = $(OPT1)  $(OPT2) $(OPT3)
#CFLAGS =  -fprofile-arcs -ftest-coverage  $(OPT3)
OBJS =  board1.o board2.o board3.o cbfile.o cbdb.o mydb.o  
all: libkegg.a
libmird.a: 
	ln -s /usr/src/Mird-1.0.7/src/libmird.a libmird.a
libkegg.a: $(OBJS)
	rm -f libkegg.a
	ar q libkegg.a $(OBJS)
	ranlib libkegg.a
board1.o: board1.cc board.h cbdb.h cbfile.h
	  $(CC) $(CFLAGS) -c board1.cc
board2.o: board2.cc board.h
	$(CC)  $(CFLAGS)   -c board2.cc
board3.o: board3.cc board.h
	$(CC)  $(CFLAGS)   -c board3.cc
cbfile.o: cbfile.cc board.h cbfile.h
	$(CC)  $(CFLAGS) $(DEFINES) -c cbfile.cc
cbdb.o: cbdb.cc cbdb.h board.h cbfile.h mydb.h
	$(CC)  $(CFLAGS) $(DEFINES) -c cbdb.cc
2mv: 2mv.cc libkegg.a 
	gcc $(CFLAGS) -o 2mv 2mv.cc libkegg.a -lstdc++
strip_cb: strip_cb.cc libkegg.a
	gcc $(CFLAGS) -o strip_cb strip_cb.cc libkegg.a -lstdc++
clean:
	rm -f *~ *.o *.a
