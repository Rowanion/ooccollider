CC=mpic++
LD=$(CC)
#HOMEDIR=/home-pc2/user/ava
HOMEDIR=/scratch/tsuess/ava
CPPFLAGS = -Wall -Wno-deprecated -O3 -fPIC -ffast-math -funroll-loops -msse2 -DPC2 -I../events -I$(HOMEDIR)/Diplom/usr/include -I../../OOCTools/include/ -I$(HOMEDIR)/Diplom/include -I../../OOCFormats/include/ -I../../OOCFramework/include/ -I./
# `sdl-config --cflags`
LFLAGS = -fPIC -L$(HOMEDIR)/Diplom/usr/lib64 -L$(HOMEDIR)/Diplom/usr/lib -L$(HOMEDIR)/Diplom/lib  -L/usr/lib64 -L/usr/X11R6/lib -lmpi -lglfw -lGL -lGLU -lGLEW -lCg -lCgGL -lglut -lboost_system-gcc41-mt-1_38 -lboost_filesystem-gcc41-mt -lpthread -looctools -loocformats -loocframework

HEADERFILES = ResultsEvent.h KeyReleasedEvent.h RenderCoreGlFrame.h SceneCompletionEvent.h VboDistributionEvent.h MemTools.h AccumulatedRendertimeEvent.h ClearCacheEvent.h DepthBufferRequestEvent.h InfoRequestEvent.h KillApplicationEvent.h MouseButtonStateChangeEvent.h MouseWheelEvent.h OcclusionResultsEvent.h WindowClosedEvent.h CameraMovedEvent.h ColorBufferEvent.h EndOfFrameEvent.h JobDoneEvent.h ModelViewMatrixEvent.h MouseDraggedEvent.h NodeRequestEvent.h RandomSeedEvent.h WindowResizedEvent.h ChangeTileDimensionsEvent.h DepthBufferEvent.h EndTransmissionEvent.h KeyPressedEvent.h MouseButtonEvent.h MouseMovedEvent.h OcclusionRequestEvent.h VboEvent.h AbstractEventHandler.h AbstractGlFrame.h DataCoreGlFrame.h DataCore.h declarations.h error.h OOCCamera.h OOCWindow.h RenderCore.h RenderMasterCore.h SimpleGlFrame.h Splittree.h Structs.h TestEventHandler.h

OBJS = ../events/ResultsEvent.o ../events/KeyReleasedEvent.o RenderCoreGlFrame.o ../events/SceneCompletionEvent.o ../events/VboDistributionEvent.o MemTools.o AbstractGlFrame.o DataCore.o DataCoreGlFrame.o OOCCamera.o OOCCollider.o OOCWindow.o RenderCore.o RenderMasterCore.o SimpleGlFrame.o Splittree.o Structs.o TestEventHandler.o ../events/AccumulatedRendertimeEvent.o ../events/ClearCacheEvent.o ../events/DepthBufferRequestEvent.o ../events/InfoRequestEvent.o ../events/KillApplicationEvent.o ../events/MouseButtonStateChangeEvent.o ../events/MouseWheelEvent.o ../events/OcclusionResultsEvent.o ../events/WindowClosedEvent.o ../events/CameraMovedEvent.o ../events/ColorBufferEvent.o ../events/EndOfFrameEvent.o ../events/JobDoneEvent.o ../events/ModelViewMatrixEvent.o ../events/MouseDraggedEvent.o ../events/NodeRequestEvent.o ../events/RandomSeedEvent.o ../events/WindowResizedEvent.o ../events/ChangeTileDimensionsEvent.o ../events/DepthBufferEvent.o ../events/EndTransmissionEvent.o ../events/KeyPressedEvent.o ../events/MouseButtonEvent.o ../events/MouseMovedEvent.o ../events/OcclusionRequestEvent.o ../events/VboEvent.o

SRCS = ../events/ResultsEvent.cpp ../events/KeyReleasedEvent.cpp RenderCoreGlFrame.cpp ../events/SceneCompletionEvent.cpp ../events/VboDistributionEvent.cpp MemTools.cpp AbstractGlFrame.cpp DataCore.cpp DataCoreGlFrame.cpp OOCCamera.cpp OOCCollider.cpp OOCWindow.cpp RenderCore.cpp RenderMasterCore.cpp SimpleGlFrame.cpp Splittree.cpp Structs.cpp ../events/TestEventHandler.cpp ../events/AccumulatedRendertimeEvent.cpp ../events/ClearCacheEvent.cpp ../events/DepthBufferRequestEvent.cpp ../events/InfoRequestEvent.cpp ../events/KillApplicationEvent.cpp ../events/MouseButtonStateChangeEvent.cpp ../events/MouseWheelEvent.cpp ../events/OcclusionResultsEvent.cpp ../events/WindowClosedEvent.cpp ../events/CameraMovedEvent.cpp ../events/ColorBufferEvent.cpp ../events/EndOfFrameEvent.cpp ../events/JobDoneEvent.cpp ../events/ModelViewMatrixEvent.cpp ../events/MouseDraggedEvent.cpp ../events/NodeRequestEvent.cpp ../events/RandomSeedEvent.cpp ../events/WindowResizedEvent.cpp ../events/ChangeTileDimensionsEvent.cpp ../events/DepthBufferEvent.cpp ../events/EndTransmissionEvent.cpp ../events/KeyPressedEvent.cpp ../events/MouseButtonEvent.cpp ../events/MouseMovedEvent.cpp ../events/OcclusionRequestEvent.cpp ../events/VboEvent.cpp

all : ooccollider
	chmod g+rw *.o

clean :
	rm -rf ooccollider *.o
	rm -rf ../events/*.o

ooccollider : ${OBJS}
	$(LD) -o $@ ${OBJS} ${LFLAGS}

depend :
	makedepend ${SRCS}

install :
	cp ooccollider $(HOMEDIR)/Diplom/usr/bin
	mkdir $(HOMEDIR)/Diplom/usr/shader
	cp ../shader/*.cg $(HOMEDIR)/Diplom/usr/shader/
	cp ../camerapositions.data $(HOMEDIR)/Diplom/usr/
