#!/usr/bin/env bash

#breezeDarkIconPath="/usr/share/icons/breeze-dark/actions/16/"
breezeDarkIconPath="/run/current-system/sw/share/icons/breeze-dark/actions/16/"

cd ../breeze-qownnotes/16x16 || exit
find *.svg | xargs -I %% cp ${breezeDarkIconPath}%% ../../breeze-dark-qownnotes/16x16
