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
#include <dirent.h>

#include <QtGui>
#include <QApplication>
#include "cv.h"
#include "highgui.h"
#include "faceTrainer.h"
#include "pam_face_defines.h"
#include <iostream>
#include <list>
#include <string>
#include <cctype>
using namespace std;
QImage *QImageIplImageCvt(IplImage *input);
typedef struct{
int filterMaceFacePSLR;
int filterMaceEyePSLR;
int filterMaceInsideFacePSLR;
}config;

void setConfig(config *configuration,char * configDirectory);
config * getConfig(char *configDirectory);


void faceTrainer::setIbarText(char *message)
{
    ui.lbl_ibar->setText(message);

}

void faceTrainer:: populateQList()
{
    ui.lv_thumbnails->clear();
    setFace* faceSetStruct =newVerifier.getFaceSet();
    int i=0;
    for (i=0;i<faceSetStruct->count;i++)
    {
        char setName[100];
        sprintf(setName,"Set %d",i+1);
        //ui.lv_thumbnails->setIconSize(QSize(60, 60));
        QListWidgetItem *item = new QListWidgetItem(setName,ui.lv_thumbnails);
        item->setIcon(QIcon(faceSetStruct->setFilePathThumbnails[i]));
        QString qstring(faceSetStruct->setName[i]);
        //   printf("%s \n",faceSetStruct->setName[i]);
        item->setData(Qt::UserRole,qstring);


    }


}
void faceTrainer::setQImageWebcam(QImage *input)
{
    if (!input)
        return ;

    static QGraphicsScene * scene = new QGraphicsScene(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    ui.webcamPreview->setScene(scene);
    ui.webcamPreview->setBackgroundBrush(*input);

    ui.webcamPreview->show();

}
void faceTrainerAdvSettings::setQImageWebcam(QImage *input)
{
    if (!input)
        return ;

    static QGraphicsScene * scene = new QGraphicsScene(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    ui.webcamPreview->setScene(scene);
    ui.webcamPreview->setBackgroundBrush(*input);

    ui.webcamPreview->show();

}
void faceTrainerAdvSettings::saveClicked()
{
    config newConfig;
    newConfig.filterMaceEyePSLR= ui.sb_eye->value();
    newConfig.filterMaceFacePSLR= ui.sb_face->value();
    newConfig.filterMaceInsideFacePSLR= ui.sb_insideFace->value();
    setConfig(&newConfig,configDirectory);
}

void faceTrainerAdvSettings::restoreDefaults()
{
    ui.sb_face->setValue(MACE_FACE_DEFAULT);
    ui.sb_eye->setValue(MACE_EYE_DEFAULT);
    ui.sb_insideFace->setValue(MACE_INSIDE_FACE_DEFAULT);
    saveClicked();

}

void faceTrainerAdvSettings::sT(opencvWebcam *wc,detector *nd,verifier *nv)
{
     webcam=wc;
        newDetector=nd;
       newVerifier=nv;

        startTimer( 100 );
}

void faceTrainerAdvSettings:: testRecognition()
{
   IplImage * queryImage = webcam->queryFrame();
    if(newVerifier->verifyFace(newDetector->clipFace(queryImage))==1)
    ui.result->setText(QString("Yes"));
    else
    ui.result->setText(QString("No"));
}

faceTrainerAdvSettings::faceTrainerAdvSettings(QWidget *parent, char* configDir,opencvWebcam *wc,detector *nd,verifier *nv)
        : QDialog(parent)
{
    webcam=wc;
        newDetector=nd;
       newVerifier=nv;
    configDirectory=configDir;
    ui.setupUi(this);
    connect(ui.pb_save,SIGNAL(clicked()), this, SLOT(saveClicked()));
    connect(ui.pb_restore,SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    connect(ui.pb_tr,SIGNAL(clicked()), this, SLOT(testRecognition()));

    config* newConfig;
    newConfig=getConfig(configDirectory);
    ui.sb_face->setValue(newConfig->filterMaceFacePSLR);
    ui.sb_eye->setValue(newConfig->filterMaceEyePSLR);
    ui.sb_insideFace->setValue(newConfig->filterMaceInsideFacePSLR);


}

faceTrainer::faceTrainer(QWidget *parent)
        : QMainWindow(parent)
{
    newDialog= new faceTrainerAdvSettings(this,newVerifier.configDirectory,&webcam,&newDetector,&newVerifier);
    ui.setupUi(this);
    ui.stkWg->setCurrentIndex(0);
    connect(ui.pb_capture,SIGNAL(clicked()), this, SLOT(captureClick()));
    connect(ui.pb_next_t2,SIGNAL(clicked()), this, SLOT(showTab3()));
    connect(ui.pb_next_t1,SIGNAL(clicked()), this, SLOT(showTab2()));
    connect(ui.pb_back_t2,SIGNAL(clicked()), this, SLOT(showTab1()));
    connect(ui.pb_ds,SIGNAL(clicked()), this, SLOT(removeSelected()));
    connect(ui.pb_adv,SIGNAL(clicked()), this, SLOT(showAdvDialog()));

    //connect(ui.button1,SIGNAL(clicked()), this, SLOT(verify()));
    //connect(ui.but,SIGNAL(clicked()), this, SLOT(butClick()));


}
void faceTrainer::showAdvDialog()
{
    newDialog->sT(&webcam,&newDetector,&newVerifier);
    newDialog->exec();

}


void faceTrainer::verify()
{

    struct dirent *de=NULL;
    DIR *d=NULL;
    d=opendir("/home/rohan/train/");
    while (de = readdir(d))
    {
        if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0)))
        {
            char fullPath[300];
            sprintf(fullPath,"%s/%s","/home/rohan/train",de->d_name);
            IplImage *temp=cvLoadImage(fullPath,1);
           // printf("%s \n",fullPath);
            if(newVerifier.verifyFace(temp)==1)
            {
            }

        }
    }

//IplImage * queryImage = webcam.queryFrame();
//newVerifier.verifyFace(queryImage);

}
/*
void faceTrainer::butClick()
{
    IplImage * queryImage = webcam.queryFrame();
    newVerifier.verifyFace(newDetector.clipFace(queryImage));
}
*/
void faceTrainer::removeSelected()
{


    QList<QListWidgetItem *>  list=ui.lv_thumbnails->selectedItems();
    QList<QListWidgetItem *>::iterator i;

    for (i = list.begin(); i != list.end(); ++i)
    {
        QListWidgetItem *item = *i;
        QString dat=item->data(Qt::UserRole).toString();
        char *ptr =  dat.toAscii().data();
//      printf("%s \n",ptr);
        newVerifier.removeFaceSet(ptr);

    }
    ui.lv_thumbnails->clear();

    populateQList();

verify();

}
void faceTrainer::captureClick()
{

    static int latch=0;
    if (latch==0)
    {
        ui.pb_capture->setText("Cancel");
        latch=1;
        newDetector.startClipFace(13);
    }
    else
    {
        ui.pb_capture->setText("Capture");
        latch=0;
        newDetector.stopClipFace();
    }



}
void faceTrainer::timerEvent( QTimerEvent * )
{

    IplImage * queryImage = webcam.queryFrame();
    newDetector.runDetector(queryImage);
    //if(newDetector.checkEyeDetected()==1)
    //newVerifier.verifyFace(newDetector.clipFace(queryImage));
    //this works captureClick();
    //double t = (double)cvGetTickCount();
    cvLine(queryImage, newDetector.eyesInformation.LE, newDetector.eyesInformation.RE, cvScalar(0,255,0), 4);
//newVerifier.verifyFace(newDetector.clipFace(queryImage));
   QImage * qm=QImageIplImageCvt(queryImage);
    if (newDetector.finishedClipFace()==1)
    {
        newVerifier.addFaceSet(newDetector.returnClipedFace(),13);
        captureClick();
        populateQList();

    }

    setQImageWebcam(qm);


    setIbarText(newDetector.queryMessage());


    cvWaitKey(1);

    delete qm;
    cvReleaseImage(&queryImage);

}


void faceTrainerAdvSettings::timerEvent( QTimerEvent * )
{
    IplImage * queryImage = webcam->queryFrame();
    newDetector->runDetector(queryImage);
    //if(newDetector->checkEyeDetected()==1)
    //newVerifier->verifyFace(newDetector->clipFace(queryImage));
    //this works captureClick();
    //double t = (double)cvGetTickCount();
    cvLine(queryImage, newDetector->eyesInformation.LE, newDetector->eyesInformation.RE, cvScalar(0,255,0), 4);
//newVerifier->verifyFace(newDetector->clipFace(queryImage));
    QImage * qm=QImageIplImageCvt(queryImage);
    setQImageWebcam(qm);
    cvWaitKey(1);

    delete qm;
}
void faceTrainer::showTab2()
{
    if(webcam.startCamera()==1)
    {
    ui.stkWg->setCurrentIndex(1);
    populateQList();
    startTimer( 100 );
    }
    else
    {
    QMessageBox msgBox1;
            msgBox1.setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
    msgBox1.setWindowTitle("Face Trainer");
    msgBox1.setText("<strong>Error</strong>");
    msgBox1.setInformativeText(
        "Camera Not Found. <br /> "
        "Plugin Your Camera and Try Again.");
    msgBox1.setStandardButtons(QMessageBox::Ok);
    msgBox1.setIconPixmap(QPixmap(":/cnc.png"));
    msgBox1.exec();
    }
}

void faceTrainer::showTab1()
{

    ui.stkWg->setCurrentIndex(0);
}
void faceTrainer::showTab3()
{
    ui.stkWg->setCurrentIndex(2);
}



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    faceTrainer tab1;
    tab1.setWindowTitle("Face Trainer");

     QRect r = tab1.geometry();
     r.moveCenter(QApplication::desktop()->availableGeometry().center());
     tab1.setGeometry(r);


    tab1.show();
    return app.exec();
}

