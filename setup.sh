#!/bin/bash

dir=`dirname "$0"`
cd $dir

echo "This script will install dependencies, compile and install swarmcon on your system."
echo
echo "swarmcon: https://github.com/gestom/CosPhi/tree/master/Localization"
echo
echo "It will install the following packages from apt:"
echo
echo "$(cat swarmcon.txt)"
echo
echo "This requires root access"

while true; do
    read -p "Proceed? [y/n]: " yn
    case $yn in
        [Yy]* ) echo
                break;;
        [Nn]* ) exit 1
                break;;
        * )     echo "Answer [y]es or [n]o";;
    esac
done

for line in $(cat swarmcon.txt)
do
    sudo apt install $line -y
done

cd scr

make