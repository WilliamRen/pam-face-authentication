/* TRANSLATOR std::faceTrainerAdvSettings */

/*
    faceTrainerAdvSettings class
    Copyright (C) 2010 Rohan Anil (rohan.anil@gmail.com) -BITS Pilani Goa Campus
    http://www.pam-face-authentication.org

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>
#include "cv.h"
#include "highgui.h"
#include "qtUtils.h"
#include "utils.h"
#include "webcamImagePaint.h"
#include "faceTrainerAdvSettings.h"

using std::string;

//------------------------------------------------------------------------------
faceTrainerAdvSettings::faceTrainerAdvSettings(QWidget* parent, 
    const string configDir)
    : QDialog(parent), configDirectory(configDir)
{
    ui.setupUi(this);

    connect(ui.pb_save,SIGNAL(clicked()), this, SLOT(saveClicked()));
    connect(ui.pb_restore,SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    connect(ui.pb_tr,SIGNAL(clicked()), this, SLOT(testRecognition()));

    initConfig();
}

//------------------------------------------------------------------------------
faceTrainerAdvSettings::~faceTrainerAdvSettings()
{
}

//------------------------------------------------------------------------------
void faceTrainerAdvSettings::sT(opencvWebcam* wc, detector* nd, verifier* nv)
{
    webcam = wc;
    newDetector = nd;
    newVerifier = nv;
    startTimer(20);
}

//------------------------------------------------------------------------------
void faceTrainerAdvSettings::setQImageWebcam(QImage *input)
{
    if(!input) return;

    static QGraphicsScene* scene = new QGraphicsScene(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    ui.webcamPreview->setScene(scene);
    ui.webcamPreview->setBackgroundBrush(*input);

    ui.webcamPreview->show();
}

//------------------------------------------------------------------------------
void faceTrainerAdvSettings::initConfig()
{
    config* newConfig = getConfig((char*)configDirectory.c_str());
    ui.percentage->setValue(int(newConfig->percentage*100));
    // ui.sb_face->setValue(newConfig->filterMaceFacePSLR);
    // ui.sb_eye->setValue(newConfig->filterMaceEyePSLR);
    // ui.sb_insideFace->setValue(newConfig->filterMaceInsideFacePSLR);
}

//------------------------------------------------------------------------------
void faceTrainerAdvSettings::saveClicked()
{
    config newConfig;
    newConfig.percentage =((double)ui.percentage->value() / 100);
    // newConfig.filterMaceFacePSLR = ui.sb_face->value();
    // newConfig.filterMaceInsideFacePSLR = ui.sb_insideFace->value();
    setConfig(&newConfig, (char*)configDirectory.c_str());
}

//------------------------------------------------------------------------------
void faceTrainerAdvSettings::closeEvent(QCloseEvent* event)
{
    saveClicked();
}

//------------------------------------------------------------------------------
void faceTrainerAdvSettings::restoreDefaults()
{
    initConfig();
    ui.percentage->setValue(80);
    // ui.sb_face->setValue(MACE_FACE_DEFAULT);
    // ui.sb_eye->setValue(MACE_EYE_DEFAULT);
    // ui.sb_insideFace->setValue(MACE_INSIDE_FACE_DEFAULT);
    saveClicked();
    newVerifier->createBiometricModels(NULL);
}

//------------------------------------------------------------------------------
void faceTrainerAdvSettings::testRecognition()
{
    IplImage* queryImage = webcam->queryFrame();
    IplImage* test = newDetector->clipFace(queryImage);
        
    if(newVerifier->verifyFace(test) == true)
        ui.result->setText(QString(tr("Yes")));
    else
        ui.result->setText(QString(tr("No")));
        
    cvReleaseImage(&queryImage);
    cvReleaseImage(&test);
}

//------------------------------------------------------------------------------
void faceTrainerAdvSettings::timerEvent(QTimerEvent*)
{
    static webcamImagePaint newWebcamImagePaint;
    
    IplImage* queryImage = webcam->queryFrame();
    newDetector->runDetector(queryImage);
    
    // if(newDetector->checkEyeDetected() == true)
    // newVerifier->verifyFace(newDetector->clipFace(queryImage));
    // this works captureClick();
    // double t = (double)cvGetTickCount();
    
    newWebcamImagePaint.paintCyclops(queryImage, 
        newDetector->eyesInformation.LE, newDetector->eyesInformation.RE);
    newWebcamImagePaint.paintEllipse(queryImage, 
        newDetector->eyesInformation.LE, newDetector->eyesInformation.RE);

    /* cvLine(queryImage, 
        newDetector->eyesInformation.LE, newDetector->eyesInformation.RE, 
        cvScalar(0,255,0), 4);*/
    // newVerifier->verifyFace(newDetector->clipFace(queryImage));
    
    QImage* qm = QImageIplImageCvt(queryImage);
    setQImageWebcam(qm);
    cvWaitKey(1);
    cvReleaseImage(&queryImage);
    delete qm;
}

