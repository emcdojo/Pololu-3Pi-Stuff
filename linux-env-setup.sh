#!/bin/sh

# install avr libraries andtoolchain
sudo apt-get install avrdude avrprog gcc-avr avr-libc

# download and install libpololu-avr
wget http://www.pololu.com/file/download/libpololu-avr-121115.zip?file_id=0J594
unzip libpololu-avr-121115.zip?file_id=0J594
cd libpololu-avr
make install

