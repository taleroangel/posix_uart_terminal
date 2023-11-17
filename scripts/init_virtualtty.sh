#!/usr/bin/bash

sudo mkdir -p /dev/virt
sudo chown root:dialout -R /dev/virt
sudo chmod 770 /dev/virt

echo -e "\nDefault baud rate is B38400"
echo -e "/dev/virt/vtty0; /dev/virt/vtty1\n"
socat -d -d -v pty,raw,echo=0,link=/dev/virt/vtty0 pty,raw,echo=0,link=/dev/virt/vtty1