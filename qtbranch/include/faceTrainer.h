#ifndef _INCL_FACE_TRAINER
#define _INCL_FACE_TRAINER

#include "ui_faceTrainer.h"
#include "opencvWebcam.h"
#include "detector.h"
#include "verifier.h"

/**
* Face Trainer class. The main window used to train the face.
*/
class faceTrainer: public QMainWindow
{
    Q_OBJECT
    
    private:
      int timerId;
        
      /**
      * UI object of the face trainer main window
      */
      Ui::faceTrainer ui;
      
      // opencvWebcamThread* thread;
      
    private slots:
      /**
      * Slot - Shows the first tab (Instructions tab)
      */
      void showTab1();
      
      /**
      * Slot - Shows the second tab (Training tab)
      */
      void showTab2();
      
      /**
      * Slot - Shows the last tab (Thank you tab)
      */
      void showTab3();
   
      /**
      * Slot - Capture button click
      */
      void captureClick();

      /**
      * Slot - Shows the about dialog
      */
      void about();
      
      /**
      * Slot - Removes the selected set from QList, 
      * and removes the files on disk as well
      */
      void removeSelected();
      
      /**
      * Slot - Shows the advanced settings dialog
      */
      void showAdvDialog();
        
      // void butClick();
      
    protected:
      /**
      * timerEvent of MainWindow overload
      */
      void timerEvent(QTimerEvent*);      

    public:
      /**
      * The constructor
      * Sets up the UI for the main window and connect signals to slots
      */
      faceTrainer(QWidget* parent = 0);
      
      /**
      * The destructor
      */
      virtual ~faceTrainer();

      /**
      * OpenCV webcam object
      */
      opencvWebcam webcam;
      
      /**
      * Detector object
      */
      detector newDetector;
      
      /**
      * Verifier object
      */
      verifier newVerifier;
      
      /**
      * Populates the QList with Face Image Sets in $HOME/.pam-face-authentication/faces/
      */
      void populateQList();
      
      /**
      * Crappy function i use to test against a set of Images, ignore it
      */
      void verify();
      
      /**
      * Translating messages from detector class
      * @param int for Message index
      */
      QString getQString(int messageIndex);

    public slots:
      /**
      * Sets the image of QGraphicsView which displays the Webcam
      * @param input that needs to be set.
      */
      void setQImageWebcam(QImage*);
    
      /**
      * Sets the information bar with message of the current state
      * @param message The Message that needs to be set.
      */
      void setIbarText(QString message);

};

#endif // _INCL_FACE_TRAINER
