#!/bin/sh

killall -9 OOCCollider
ssh horus "killall -9 OOCCollider"
ssh osiris "killall -9 OOCCollider"
