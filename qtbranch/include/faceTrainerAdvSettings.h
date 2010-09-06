#ifndef _INCL_FACE_TRAINER_ADV
#define _INCL_FACE_TRAINER_ADV

#include <string>
#include "ui_faceTrainerAdvSettings.h"
#include "opencvWebcam.h"
#include "detector.h"
#include "verifier.h"

/**
* Face advanced settings class. The QDialog used to configure advanced settings.
*/
class faceTrainerAdvSettings: public QDialog
{
    Q_OBJECT

    private:
        /**
        * UI Object of the Face Trainer Main Window
        */
        Ui::faceTrainerAdvSettings ui;

    private slots:
        /**
        * SLOT - Saves the config values
        */
        void saveClicked();
        
        /**
        * SLOT - Make sure the config is saved upon window closure
        */
        void closeEvent(QCloseEvent* event);
        
        /**
        * SLOT - Restore Defaults to the Config Values
        */
        void restoreDefaults();
        
        /**
        * SLOT - Runs Test Recognition, For user to cross check if the System is working
        */
        void testRecognition();
    
    public slots:
        /**
        * Sets the image of QGraphicsView which Displays the Webcam
        * @param input that needs to be set.
        */
        void setQImageWebcam(QImage *);

    protected:
        /**
        * timerEvent of MainWindow Overload
        */
        void timerEvent(QTimerEvent*);
        
    public:
        /**
        * The Constructor
        * Sets up the UI for the Main Window and connect Signals to Slots
        * @param parent Parent of QWidget
        * @param config Config Directory Path
        * @param wc OpenCVWebcam Pointer
        * @param nd Detector Pointer
        * @param nv Verifier Pointer
        */
        faceTrainerAdvSettings(QWidget* parent = 0, const std::string config = "");
        
        /**
        * The destructor
        */
        virtual ~faceTrainerAdvSettings();
        
        /**
        * Config Directory Path
        */
        std::string configDirectory;
        
        /**
        * Initializes the Ui with Config Data
        */
        void initConfig();
        
        /**
        * Pointer to OpenCV Webcam object
        */
        opencvWebcam* webcam;
        
        /**
        * Pointer to Detector object
        */
        detector* newDetector;
        
        /**
        * Pointer to Verifier object
        */
        verifier* newVerifier;
        
        /**
        * Set Internal Variables of Face Advanced Settings Object
        * @param wc Webcam Object Pointer
        * @param nd Detector Object Pointer
        * @param nv Verifier Object Pointer
        */
        void sT(opencvWebcam* wc, detector* nd, verifier* nv);            

};

#endif // _INCL_FACE_TRAINER_ADV
