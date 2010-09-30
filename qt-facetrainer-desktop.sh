#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=0.3
Type=Application
Name=Qt Face Trainer
GenericName=Face Trainer
Comment=Face Trainer for PAM Face Authentication
TryExec=$1/bin/qt-facetrainer
Exec=$1/bin/qt-facetrainer &
Categories=Application;
Icon=$1/share/icons/pfa-logo.png
EOF
