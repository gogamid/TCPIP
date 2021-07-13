#!/bin/bash
gcc  smbbroker.c -o smbbroker
sudo cp smbbroker /usr/local/bin
gcc smbpublish.c -o smbpublish
sudo cp smbpublish /usr/local/bin
gcc smbsubscribe.c -o smbsubscribe
sudo cp smbsubscribe /usr/local/bin
smbbroker              