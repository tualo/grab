#!/bin/sh
mkdir /opt/grab

cp grab /opt/grab/grab
cp customer.txt /opt/grab/customer.txt
cp grab-start.sh /opt/grab/grab-start.sh

chmod +x /opt/grab/grab
chmod +x /opt/grab/grab/grab-start.sh

mv grab.service /etc/systemd/system/grab.service
systemctl daemon-reload
