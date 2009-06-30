/*
    QT Face Auth MAIN
    Copyright (C) 2009 Rohan Anil (rohan.anil@gmail.com) -BITS Pilani Goa Campus
    http://code.google.com/p/pam-face-authentication/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui>
#include <QApplication>
#include <QDialog>
#include "faceAuth.h"
#include "pam_face_defines.h"
void faceAuth::authClicked()
{
    close();
}

void faceAuth::cancelClicked()
{
    close();
}

faceAuth::faceAuth(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
Qt::WindowFlags flags;
Qt::WindowModality modality =Qt::ApplicationModal;
flags =Qt::SplashScreen | Qt::FramelessWindowHint;

setWindowFlags( flags );
setWindowModality(modality);
    connect(ui.pbAuth,SIGNAL(clicked()), this, SLOT(authClicked()));
    connect(ui.pbCancel,SIGNAL(clicked()), this, SLOT(cancelClicked()));

}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    faceAuth faceAuthWindow;
    faceAuthWindow.exec();
//return app.exec();
}

