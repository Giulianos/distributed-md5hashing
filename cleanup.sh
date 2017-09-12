#!/bin/bash

killall -s SIGKILL slave
killall -s SIGKILL application
killall -s SIGKILL md5sum
killall -s SIGKILL view
ipcrm -M 0x0701fa01
ipcrm -S 0x0701fa01
