#!/bin/bash

hosts=("osiris" "horus" "kerner");

for host in ${hosts[@]}
do
  ssh $host "killall -9 OOCCollider"
done

killall -9 OOCCollider
