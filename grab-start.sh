#!/bin/sh

export GRAB_USER=jondoe
export GRAB_PATH=/tmp/
export GRAB_EXPOSURE=800
export GRAB_HEIGHT=32
export GRAB_START_AVG=18
export GRAB_STOP_AVG=8
export GRAB_PID=/var/run/grab.pid
export PYLON_ROOT=/opt/pylon5
export GENICAM_ROOT_V2_3=$PYLON_ROOT/genicam
export GENICAM_CACHE_v2_3=${HOME}/genicam_xml_cache
gclibdir=$GENICAM_ROOT_V2_3/bin/Linux64_x64
pylonlibdir=$PYLON_ROOT/lib64
export LD_LIBRARY_PATH=$pylonlibdir:$gclibdir:$gclibdir/GenApi/Generic:$LD_LIBRARY_PATH
sleep 4
nice -n -19 /opt/grab/grab
sleep 10
