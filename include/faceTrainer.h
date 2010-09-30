/** @file */

#include "ui_faceTrainer.h"
#include "faceTrainerAdvSettings.h"
//#include "opencvWebcamThread.h"
#include "opencvWebcam.h"
#include "detector.h"
#include "verifier.h"
/**
* Face Trainer Class. The Main window used to train the Face.
*/
class faceTrainer: public QMainWindow
{
    Q_OBJECT

public:
    /**
    *The Constructor
    *Sets up the UI for the Main Window and connect Signals to Slots
    */
    faceTrainer(QWidget *parent = 0);

    /**
    *OpenCV Webcam Object
    */
    opencvWebcam webcam;
    /**
    *Detector Object
    */
    detector newDetector;
    /**
    *Verifier Object
    */
    verifier newVerifier;
    /**
    *Populates the QList with Face Image Sets in the $HOME/.pam-face-authentication/faces/
    */
    void populateQList();
    /**
    *Crappy Function i use to test against a set of Images, Ignore it
    */
    void verify();

private:

    int timerId;
    /**
    *UI Object of the Face Trainer Main Window
    */
    Ui::faceTrainer ui;
    //  	opencvWebcamThread * thread;
protected:
    /**
    *timerEvent of MainWindow Overload
    */
    void timerEvent( QTimerEvent* );

private slots:
    /**
    *Slot - Capture Button Click
    */
    void captureClick();
    /**
    *Slot - Shows the Last Tab (Thank You Tab)
    */
    void showTab3();
    /**
    *Slot - Shows the Second Tab (Training Tab)
    */
    void showTab2();

    /**
    *Slot - Shows the First Tab (Instructions Tab)
    */
    void showTab1();

    /**
    *Slot - Shows the About Dialog
    */
    void about();
    /**
    *Slot - Removes the Selected Set from QList , removes the files on disk as well
    */
    void removeSelected();
    //   void verify();

    /**
    *Slot - Shows the Advanced Settings Dialog
    */
    void showAdvDialog();
//        void butClick();


public slots:
    /**
    *Sets the image of QGraphicsView which Displays the Webcam
    *@param input that needs to be set.
    */

    void setQImageWebcam(QImage *);
    /**
    *Sets the information bar with Message of the current state
    *@param message The Message that needs to be Set.
    */
    void setIbarText(QString message);

};
