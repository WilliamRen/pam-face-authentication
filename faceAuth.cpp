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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

void resetFlags();
void ipcStart();
char *shared;
key_t ipckey;
int shmid;

// COMMUNICATION SHARED MEMORY
int *commAuth;
key_t ipckeyCommAuth;
int shmidCommAuth;

void resetFlags()
{
    *commAuth=0;
}

void ipcStart()
{
    /*   IPC   */
    ipckey =  IPC_KEY_IMAGE;
    shmid = shmget(ipckey, IMAGE_SIZE, IPC_CREAT | 0666);
    shared = shmat(shmid, NULL, 0);

    ipckeyCommAuth = IPC_KEY_STATUS;
    shmidCommAuth = shmget(ipckeyCommAuth, sizeof(int), IPC_CREAT | 0666);
    commAuth = shmat(shmidCommAuth, NULL, 0);

    *commAuth=0;
    /*   IPC END  */
}
void faceAuth::timerEvent( QTimerEvent * )
{
}
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
    ipcStart();
    resetFlags();
    startTimer( 100 );
    faceAuthWindow.exec();
//return app.exec();
}

