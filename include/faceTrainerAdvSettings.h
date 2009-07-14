 #include "ui_faceTrainerAdvSettings.h"
 #include "opencvWebcam.h"
#include "detector.h"
#include "verifier.h"
 class faceTrainerAdvSettings: public QDialog
    {
        Q_OBJECT

    public:
        faceTrainerAdvSettings(QWidget *parent = 0,char *config=0,opencvWebcam *wc=0,detector *nd=0,verifier *nv=0);
        char *configDirectory;
      opencvWebcam *webcam;
        detector *newDetector;
        verifier *newVerifier;
        void sT(opencvWebcam *wc,detector *nd,verifier *nv);
    private:
        Ui::faceTrainerAdvSettings ui;

    private slots:
        void saveClicked();
        void restoreDefaults();
         void testRecognition();
  public slots:
        void setQImageWebcam(QImage *);
protected:
    void        timerEvent( QTimerEvent * );

    };
