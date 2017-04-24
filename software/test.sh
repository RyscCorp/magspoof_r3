#! /bin/bash

TRACK_DATA="%B4444444444444444^ABE/LINCOLN^291110100000931?;4444444444444444=29111010000093100000?+4444444444444444=29111010000093100000?"

echo "1. Connect magstripe reader to PC"
echo "2. Turn on magspoof"
echo "3. Hold magspoof close to reader head"
echo "4. Push button on magspoof"
read TRACK
if [ "$TRACK" == $TRACK_DATA ]; then
  echo PASS
  exit 0
fi

echo FAIL
exit 1
