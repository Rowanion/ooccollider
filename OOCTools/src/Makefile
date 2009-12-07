CC=mpic++
LD=$(CC)
HOMEDIR = /home-pc2/user/ava
CPPFLAGS = -Wall -Wno-deprecated -O3 -DCLUSTER -fPIC -ffast-math -funroll-loops -msse2 -I$(HOMEDIR)/Diplom/include -I$(HOME)/Diplom/usr/include -I/usr/include -I../include 

LFLAGS = -fPIC -L$(HOMEDIR)/Diplom/usr/lib64 -L$(HOMEDIR)/Diplom/usr/lib -L$(HOMEDIR)/Diplom/lib  -L/usr/lib64 -L/usr/X11R6/lib -lGL -lGLU -lGLEW -lCg -lCgGL -lglut -lboost_system-gcc41-mt-1_38 -lboost_filesystem-gcc41-mt -lpthread 

HEADERFILES = AbstractVertex.h BoundingBox.h Camera.h CgToolkit.h ColorTable.h declarations.h ExternalFunctions.h Face.h FboFactory.h Fbo.h GlToolkit.h MetaGroup.h Model.h Octree.h OctreeNode.h OOCTools.h StructDefs.h TriBoxTest.h V3b.h V3f.h V3ub.h V4f.h Vbo.h VboManager.h VertexArray.h

OBJS = AbstractVertex.o BoundingBox.o Camera.o CgToolkit.o ColorTable.o Face.o Fbo.o FboFactory.o GlToolkit.o MetaGroup.o Model.o Octree.o OctreeNode.o TriBoxTest.o V3b.o V3f.o V3ub.o V4f.o Vbo.o VboManager.o

SRCS = AbstractVertex.cpp BoundingBox.cpp Camera.cpp CgToolkit.cpp ColorTable.cpp Face.cpp Fbo.cpp FboFactory.cpp GlToolkit.cpp MetaGroup.cpp Model.cpp Octree.cpp OctreeNode.cpp TriBoxTest.cpp V3b.cpp V3f.cpp V3ub.cpp V4f.cpp Vbo.cpp VboManager.cpp 

all : libooctools.so 
	chmod g+rw *.o

clean :
	rm -rf libooctools.so *.o

libooctools.so : ${OBJS}
	$(LD) -shared -o $@ ${OBJS} ${LFLAGS}

depend :
	makedepend ${SRCS}

install :
	cp libooctools.so $(HOMEDIR)/Diplom/usr/lib
	cp ../include/*.h $(HOMEDIR)/Diplom/usr/include/ooctools

