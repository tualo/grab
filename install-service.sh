#!/bin/sh
mkdir /opt/grab

ln -s grab /opt/grab/grab
cp customer.txt /opt/grab/customer.txt
cp grab-start.sh /opt/grab/grab-start.sh


chmod +x /opt/grab/grab
chmod +x /opt/grab/grab-start.sh

cp systemd.service /etc/systemd/system/grab.service
systemctl daemon-reload
systemctl enable grab
