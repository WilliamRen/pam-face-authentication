/*
    QT Face Manager MAIN
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
#include "cv.h"
#include "highgui.h"
#include "faceTrainer.h"
#include "faceTrainerAdvSettings.h"
#include "pam_face_defines.h"

QImage *QImageIplImageCvt(IplImage *input);
void faceTrainer::setIbarText(char *message)
{
    ui.lbl_ibar->setText(message);

}
void faceTrainer::setQImageWebcam(QImage *input)
{
    if (!input)
        return ;


    double t = (double)cvGetTickCount();

    static QGraphicsScene * scene = new QGraphicsScene(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    ui.webcamPreview->setScene(scene);
    ui.webcamPreview->setBackgroundBrush(*input);

    t = (double)cvGetTickCount() - t;
    //   printf( "detection scene time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );

    ui.webcamPreview->show();

}

faceTrainer::faceTrainer(QWidget *parent)
        : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.stkWg->setCurrentIndex(0);
    connect(ui.pb_next_t2,SIGNAL(clicked()), this, SLOT(showTab3()));
    connect(ui.pb_next_t1,SIGNAL(clicked()), this, SLOT(showTab2()));
    connect(ui.pb_back_t2,SIGNAL(clicked()), this, SLOT(showTab1()));


}

void faceTrainer::timerEvent( QTimerEvent * )
{
    IplImage * queryImage = webcam.queryFrame();
    double t = (double)cvGetTickCount();
    newDetector.runDetector(queryImage);
    t = (double)cvGetTickCount() - t;
   printf( " total time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );


   //double t = (double)cvGetTickCount();
    QImage * qm=QImageIplImageCvt(queryImage);
    //t = (double)cvGetTickCount() - t;
 //printf( "detection convert time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );

    setQImageWebcam(qm);
    setIbarText(newDetector.queryMessage());
    cvWaitKey(1);

    delete qm;
}

void faceTrainer::showTab2()
{
    ui.stkWg->setCurrentIndex(1);
    startTimer( 100 );

}

void faceTrainer::showTab1()
{
    ui.stkWg->setCurrentIndex(0);
}
void faceTrainer::showTab3()
{
    ui.stkWg->setCurrentIndex(2);
}

faceTrainerAdvSettings::faceTrainerAdvSettings(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    faceTrainer tab1;
    tab1.show();
    return app.exec();
}

