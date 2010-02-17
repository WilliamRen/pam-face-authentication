#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=Qt Face Trainer
GenericName=Face Trainer
Comment=Face Trainer for PAM Face Authentication
TryExec=$1/bin/qt-facetrainer
Exec=$1/bin/qt-facetrainer
Categories=System;
Icon=pfa-logo
EOF
