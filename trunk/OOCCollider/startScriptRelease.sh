#!/bin/sh
echo "$*"
echo "$#"
echo "startet auf "$(hostname);
if [ "$(/bin/hostname)" != 'cluster-seth' ]; then
        echo "Not the main Host" ;
        export DISPLAY=:13.0 ;
        X :13 &
	export LD_LIBRARY_PATH=/opt/OpenMPI64/lib:/home/ava/Diplom/usr/lib:/home/ava/Diplom/usr/lib64:/home/ava/workspace5/OOCFormats/Release:/home/ava/workspace5/OOCTools/Release:/home/ava/workspace5/OOCFramework/Release:/usr/lib
else
        echo "main Host";
fi
echo "sleep 5"
sleep 5
echo  "start program"
/home/ava/workspace5/OOCCollider/Release/OOCCollider $*
