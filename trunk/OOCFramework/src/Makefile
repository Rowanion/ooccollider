CC=mpic++
LD=$(CC)
HOMEDIR=/home-pc2/user/ava
#HOMEDIR=/scratch/tsuess/ava
CPPFLAGS = -Wall -Wno-deprecated -D__X86_64__ -O3 -fPIC -ffast-math -funroll-loops -msse2 -I../include -I$(HOMEDIR)/Diplom/usr/include -I../../OOCTools/include/ -I/home-pc2/user/ava/Diplom/include -I../../OOCFormats/include/
# `sdl-config --cflags`
LFLAGS = -fPIC -L$(HOMEDIR)/Diplom/usr/lib64 -L$(HOMEDIR)/Diplom/usr/lib -L$(HOMEDIR)/Diplom/lib -L/usr/lib64 -L/usr/X11R6/lib -lmpi -lGL -lGLU -lGLEW -lCg -lCgGL -lglut -lboost_system-gcc41-mt-1_38 -lboost_filesystem-gcc41-mt -lpthread -looctools -loocformats

HEADERFILES = AbstractEventListener.h CCollisionProtocol.h ClassId.h declarations.h EventManager.h IEvent.h Message.h MpiControl.h VirtualNode.h VirtualRequest.h VboFactory.h

OBJS = CCollisionProtocol.o ClassId.o EventManager.o IEvent.o Message.o MpiControl.o VirtualNode.o VirtualRequest.o VboFactory.o

SRCS = CCollisionProtocol.cpp ClassId.cpp EventManager.cpp IEvent.cpp Message.cpp MpiControl.cpp VirtualNode.cpp VirtualRequest.cpp VboFactory.cpp

all : liboocframework.so 
	chmod g+rw *.o

clean :
	rm -rf liboocframework.so *.o

liboocframework.so : ${OBJS}
	$(LD) -shared -o $@ ${OBJS} ${LFLAGS}

depend :
	makedepend ${SRCS}

install :
	cp liboocframework.so $(HOMEDIR)/Diplom/usr/lib
	cp ../include/*.h $(HOMEDIR)/Diplom/usr/include/oocframework

