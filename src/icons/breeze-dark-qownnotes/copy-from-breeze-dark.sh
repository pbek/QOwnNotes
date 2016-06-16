#!/usr/bin/env bash

breezeDarkIconPath="/usr/share/icons/breeze-dark/actions/16/"

cd ../breeze-qownnotes/16x16
find *.svg | xargs -I %% cp ${breezeDarkIconPath}%% ../../breeze-dark-qownnotes/16x16
