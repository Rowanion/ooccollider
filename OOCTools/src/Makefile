CC=mpic++
LD=$(CC)
HOMEDIR = /home-pc2/user/ava
CPPFLAGS = -Wall -Wno-deprecated -O3 -DCLUSTER -D__X86_64__ -fPIC -ffast-math -funroll-loops -msse2 -I$(HOMEDIR)/Diplom/include -I$(HOME)/Diplom/usr/include -I/usr/include -I../include -I../../OOCFormats/include

LFLAGS = -fPIC -L$(HOMEDIR)/Diplom/usr/lib64 -L$(HOMEDIR)/Diplom/usr/lib -L$(HOMEDIR)/Diplom/lib  -L/usr/lib64 -L/usr/X11R6/lib -lmpi -lGL -lGLU -lGLEW -lCg -lCgGL -lglut -lboost_system-gcc41-mt-1_38 -lboost_filesystem-gcc41-mt -lpthread 

HEADERFILES = ../include/BoundingBox.h ../include/declarations.h ../include/Fbo.h ../include/GlToolkit.h ../include/MetaGroup.h ../include/StructDefs.h ../include/V3f_class.h ../include/Vbo.h ../include/Camera.h ../include/ExternalFunctions.h ../include/GeometricOps.h ../include/IndexedVbo.h ../include/Model.h ../include/Triangle.h ../include/V3f.h ../include/VboManager.h ../include/CgToolkit.h ../include/Face.h ../include/glCamera.h ../include/IndexedVertexArray.h ../include/OOCTools.h ../include/TriBoxTest.h ../include/V3ub.h ../include/VertexArray.h ../include/ColorTable.h ../include/FboFactory.h ../include/glQuaternion.h ../include/MersenneTwister.h ../include/Quaternion.h ../include/V3b.h ../include/V4f.h .. /include/OctreeNode.h

OBJS = ColorTable.o TriBoxTest.o V3ub.o BoundingBox.o GeometricOps.o MetaGroup.o Face.o GlToolkit.o V3b.o V4f.o Camera.o Model.o Quaternion.o Fbo.o IndexedVbo.o StructDefs.o Vbo.o CgToolkit.o FboFactory.o IndexedVertexArray.o Triangle.o V3f.o VboManager.o

SRCS = ColorTable.cpp TriBoxTest.cpp V3ub.cpp BoundingBox.cpp GeometricOps.cpp MetaGroup.cpp Face.cpp GlToolkit.cpp V3b.cpp V4f.cpp Camera.cpp Model.cpp Quaternion.cpp Fbo.cpp IndexedVbo.cpp StructDefs.cpp Vbo.cpp CgToolkit.cpp FboFactory.cpp IndexedVertexArray.cpp Triangle.cpp V3f.cpp VboManager.cpp

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

