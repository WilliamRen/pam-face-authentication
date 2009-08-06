/** @file */

#include "ui_faceTrainerAdvSettings.h"
#include "opencvWebcam.h"
#include "detector.h"
#include "verifier.h"
/**
* Face Advanced Settings Class. The QDialog used to Configure Advanced Settings.
*/
class faceTrainerAdvSettings: public QDialog
{
    Q_OBJECT

public:
    /**
     *The Constructor
     *Sets up the UI for the Main Window and connect Signals to Slots
     *@param parent Parent of QWidget
     *@param config Config Directory Path
     *@param wc OpenCVWebcam Pointer
     *@param nd Detector Pointer
     *@param nv Verifier Pointer
     */
    faceTrainerAdvSettings(QWidget *parent = 0,char *config=0);
    /**
    *Config Directory Path
    */
    char *configDirectory;
    /**
    *Initializes the Ui with Config Data
    */
    void initConfig();
    /**
    *Pointer to OpenCV Webcam object
    */
    opencvWebcam *webcam;
    /**
    *Pointer to Detector object
    */
    detector *newDetector;
    /**
    *Pointer to Verifier object
    */
    verifier *newVerifier;
    /**
    *Set Internal Variables of Face Advanced Settings Object
    *@param wc Webcam Object Pointer
    *@param nd Detector Object Pointer
    *@param nv Verifier Object Pointer
    */
    void sT(opencvWebcam *wc,detector *nd,verifier *nv);
private:
    /**
    *UI Object of the Face Trainer Main Window
    */
    Ui::faceTrainerAdvSettings ui;

private slots:

    /**
    *SLOT -Saves the Config Values
    */
    void saveClicked();
    /**
    *SLOT - Restore Defaults to the Config Values
    */
    void restoreDefaults();
    /**
    *SLOT - Runs Test Recognition, For user to cross check if the System is working
    */
    void testRecognition();
public slots:
    /**
    *Sets the image of QGraphicsView which Displays the Webcam
    *@param input that needs to be set.
    */

    void setQImageWebcam(QImage *);
protected:
    /**
    *timerEvent of MainWindow Overload
    */
    void timerEvent( QTimerEvent * );

};
