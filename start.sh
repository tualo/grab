export ERP_URL=http://localhost/wawi/index.php

export ERP_CLIENT=macc
export ERP_LOGIN=sorter
export ERP_PASSWORD=test

export OCR_DB_NAME=sorter
export OCR_DB_USER=sorter
export OCR_DB_PASSWORD=sorter
export OCR_DB_HOST=localhost
export OCR_IMAGE_WIDTH_SCALE=1.2

#source /opt/pylon4/bin/pylon-setup-env.sh /opt/pylon4/
export PYLON_ROOT=/opt/pylon4
export GENICAM_ROOT_V2_3=$PYLON_ROOT/genicam
export GENICAM_CACHE_v2_3=${HOME}/genicam_xml_cache
gclibdir=$GENICAM_ROOT_V2_3/bin/Linux64_x64
pylonlibdir=$PYLON_ROOT/lib64
export LD_LIBRARY_PATH=$pylonlibdir:$gclibdir:$gclibdir/GenApi/Generic:$LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH


nohup /home/tualo/genicam/grab < /dev/null > /var/log/grab.log 2>&1 &
nohup erp-dispatcher service $ERP_URL < /dev/null > /var/log/erp-dispatcher.log 2>&1 &
nohup ocrservice bcocr --quick --processlist /usr/local/lib/node_modules/ocrservice/process_list.json /tmp/ < /dev/null > /var/log/ocrservice.log 2>&1 &


#nohup ocrservice-cwatch --processlist /usr/local/lib/node_modules/ocrservice/process_list.json /tmp/ < /dev/null > /var/log/ocrservice.log 2>&1 &
