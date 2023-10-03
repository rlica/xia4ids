#!/bin/bash

./Clear.sh

echo ""
echo "Install: xia4ids"
echo ""
cmake .
echo ""
make install

if [ ! -f "./bin/xia4ids" ]; then
  echo ""
  echo " Compilation failed!"
else
  rm -rf xia4ids_dev
  echo ""
  echo " Please include the following lines in your .bashrc:"
  echo "    PATH=\$PATH:$PWD/bin"
#  echo "    export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$PWD/"
  echo ""
  echo " Done!"
fi
