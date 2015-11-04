#!/bin/sh
mkdir -p ./bin/astyle
cd ./bin/astyle
$(wget -O astyle_2.05.1_linux.tar.gz http://downloads.sourceforge.net/project/astyle/astyle/astyle%202.05.1/astyle_2.05.1_linux.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fastyle%2Ffiles%2Fastyle%2Fastyle%25202.05.1%2F&ts=1446294940&use_mirror=skylink)
tar zxf astyle_2.05.1_linux.tar.gz
cd astyle/build/gcc/
make release -j 4
sudo make install
cd ../../../../..
