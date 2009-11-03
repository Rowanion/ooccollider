#!/bin/bash

hosts=("osiris" "horus" "kerner");

for host in ${hosts[@]}
do
  scp ~/workspace5/OOCCollider/startScript.sh $host:~/workspace5/OOCCollider/
  scp ~/workspace5/OOCCollider/Debug/OOCCollider $host:~/workspace5/OOCCollider/Debug/
  scp ~/workspace5/OOCCollider/shader/* $host:~/workspace5/OOCCollider/shader/
  scp ~/workspace5/OOCFramework/Debug/libOOCFramework.so $host:~/workspace5/OOCFramework/Debug/
  scp ~/workspace5/OOCFormats/Debug/libOOCFormats.so $host:~/workspace5/OOCFormats/Debug/
  scp ~/workspace5/OOCTools/Debug/libOOCTools.so $host:~/workspace5/OOCTools/Debug/
done
