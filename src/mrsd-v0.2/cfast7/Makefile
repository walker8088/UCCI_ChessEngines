CC = gcc
OPT1 = -O3 -march=athlon-xp -fomit-frame-pointer -pipe -maccumulate-outgoing-args
OPT2 = -ffast-math -fforce-mem -mpreferred-stack-boundary=2 -foptimize-sibling-calls
OPT4 = -s -static
CFLAGS =  $(OPT1) $(OPT2) $(OPTP) -I../kegg  
#CFLAGS =  -fprofile-arcs  -ftest-coverage -I../kegg  
LIBOBJS =  $(SOBJS)  mboard.o score.o mvgen.o  bd.o\
           automove.o mvgen2.o mvgen7.o hvalue.o
CCHOBJS = cch.o $(OBJS) 
GCCHOBJS = gcch.o $(OBJS) $(GTKOBJS)
LIBS =  -lcfast -lkegg -lstdc++
all: libcfast.a 1mv

cfast: cfast.o  libcfast.o ../kegg/libkegg.a
	$(CC) $(LDFLAGS) -o cfast cfast.o  $(LIBS)
1mv: 1mv.o  libcfast.a ../kegg/libkegg.a
	$(CC) $(CFLAGS) -L../kegg -L. -o 1mv 1mv.o $(LIBS)

ev: ev.o  libcfast.a ../kegg/libkegg.a
	$(CC) $(CFLAGS) -L../kegg -L. -o ev ev.o $(LIBS)
spider: spider.o  libcfast.a ../kegg/libkegg.a
	$(CC) $(CFLAGS) -L../kegg -L. -o spider spider.o $(LIBS)
	

libcfast.a: $(LIBOBJS)
	rm -f libcfast.a
	ar q libcfast.a $(LIBOBJS)
	ranlib libcfast.a

mboard.o: mboard.cc mboard.h ../kegg/board.h
	$(CC) $(CFLAGS)  -c  mboard.cc
1mv.o: 1mv.cc mboard.h ../kegg/board.h
	$(CC) $(CFLAGS)  -c  1mv.cc
ev.o: ev.cc mboard.h ../kegg/board.h
	$(CC) $(CFLAGS)  -c  ev.cc
spider.o: spider.cc mboard.h ../kegg/board.h
	$(CC) $(CFLAGS)  -c  spider.cc
score.o: score.cc mboard.h ../kegg/board.h
	$(CC)  $(CFLAGS) -c score.cc
hvalue.o: hvalue.cc mboard.h ../kegg/board.h
	$(CC)  $(CFLAGS) -c hvalue.cc
#old type,and some common part .
mvgen.o: mvgen.cc mboard.h ../kegg/board.h
	$(CC)  $(CFLAGS)  -c mvgen.cc
#neg fast,easy
mvgen1.o: mvgen1.cc mboard.h ../kegg/board.h
	$(CC)  $(CFLAGS)  -c mvgen1.cc
#capture first,easy  (null step)
mvgen2.o: mvgen2.cc mboard.h ../kegg/board.h
	$(CC)  $(CFLAGS) -c  mvgen2.cc
#null step,easy
mvgen3.o: mvgen3.cc mboard.h ../kegg/board.h
	$(CC)  $(CFLAGS)  -c mvgen3.cc
#move-move (null step) easy capture first,
mvgen4.o: mvgen4.cc mboard.h ../kegg/board.h
	$(CC)  $(CFLAGS)  -c mvgen4.cc
#(null step) easy capture first,eat good first
mvgen5.o: mvgen5.cc mboard.h ../kegg/board.h
	$(CC)  $(CFLAGS)  -c mvgen5.cc
#(null step) easy capture first,eat good first,hash table
mvgen7.o: mvgen7.cc mboard.h ../kegg/board.h bd.h
	$(CC)  $(CFLAGS)  -c mvgen7.cc
bd.o: bd.cc mboard.h ../kegg/board.h bd.h
	$(CC)  $(CFLAGS)  -c bd.cc
automove.o: automove.cc mboard.h ../kegg/board.h
	$(CC)  $(CFLAGS)  -c automove.cc
clean:
	rm -f *~ *.o *.a
