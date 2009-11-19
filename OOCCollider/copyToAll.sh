#!/bin/bash

hosts=("osiris" "horus" "kerner" "maat" );

for host in ${hosts[@]}
do
  scp ~/workspace5/OOCCollider/myhosts $host:~/workspace5/OOCCollider/
  scp ~/workspace5/OOCCollider/startScriptDebug.sh $host:~/workspace5/OOCCollider/
  scp ~/workspace5/OOCCollider/Debug/OOCCollider $host:~/workspace5/OOCCollider/Debug/
  scp ~/workspace5/OOCCollider/shader/* $host:~/workspace5/OOCCollider/shader/
  scp ~/workspace5/OOCFramework/Debug/libOOCFramework.so $host:~/workspace5/OOCFramework/Debug/
  scp ~/workspace5/OOCFormats/Debug/libOOCFormats.so $host:~/workspace5/OOCFormats/Debug/
  scp ~/workspace5/OOCTools/Debug/libOOCTools.so $host:~/workspace5/OOCTools/Debug/

  scp ~/workspace5/OOCCollider/myhosts $host:~/workspace5/OOCCollider/
  scp ~/workspace5/OOCCollider/startScriptRelease.sh $host:~/workspace5/OOCCollider/
  scp ~/workspace5/OOCCollider/Release/OOCCollider $host:~/workspace5/OOCCollider/Release/
  scp ~/workspace5/OOCCollider/shader/* $host:~/workspace5/OOCCollider/shader/
  scp ~/workspace5/OOCFramework/Release/libOOCFramework.so $host:~/workspace5/OOCFramework/Release/
  scp ~/workspace5/OOCFormats/Release/libOOCFormats.so $host:~/workspace5/OOCFormats/Release/
  scp ~/workspace5/OOCTools/Release/libOOCTools.so $host:~/workspace5/OOCTools/Release/

  scp ~/workspace5/OOCCollider/mkRel.sh $host:~/workspace5/OOCCollider/
done
