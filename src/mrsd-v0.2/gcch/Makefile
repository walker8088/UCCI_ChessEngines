CC = gcc
OPT1 = -O3 -march=athlon-xp -fomit-frame-pointer -pipe
OPT2 = -ffast-math -fforce-mem -mpreferred-stack-boundary=2
# you may want to remove -mwindows option from fltk-config if you are using
# cygwin
CFLAGS =   $(OPT1) $(OPT2) -I../kegg  `fltk-config --cxxflags` 
#	   -I/usr/src/Mird-1.0.7/src
LDFLAGS2=  $(OPT1) -L../kegg -lkegg `fltk-config --use-images --ldflags`
LDFLAGS=  $(OPT1) -L../kegg -lkegg `fltk-config --use-images --ldflags` -lstdc++
#	  -L/usr/src/Mird-1.0.7/src -lmird -lstdc++
OBJS = flt2.o client_board.o edit_board.o  
all: xgcch
xgcch: $(OBJS) control.h ../kegg/libkegg.a ../cfast7/libcfast.a
	$(CC) $(CFLAGS) -o xgcch  $(OBJS) ../cfast7/libcfast.a $(LDFLAGS) 
flt2.o: flt2.cc control.h ../kegg/board.h ../kegg/cbfile.h
	$(CC) $(CFLAGS) -DXGCCH -c   flt2.cc
client_board.o: client_board.cc control.h ../kegg/board.h ../kegg/cbfile.h
	$(CC) $(CFLAGS) -c   client_board.cc
edit_board.o: edit_board.cc control.h ../kegg/board.h ../kegg/cbfile.h
	$(CC) $(CFLAGS) -c   edit_board.cc
../kegg/libkegg.a:
	cd ../kegg;make
../cfast7/libcfast.a:
	cd ../cfast7;make
clean:
	rm -f *~ *.o
