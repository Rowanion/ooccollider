#!/bin/sh
COMMAND = "killall -9 OOCCollider"

$COMMAND
ssh osiris $COMMAND
ssh horus $COMMAND
