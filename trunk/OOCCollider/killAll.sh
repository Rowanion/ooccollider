#!/bin/bash

hosts=("osiris" "horus" "kerner" "maat" "sobek" "ammit" "hapi" "sokar");

killall -9 OOCCollider

for host in ${hosts[@]}
do
  ssh $host "killall -9 OOCCollider"
done
