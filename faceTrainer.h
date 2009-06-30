 #include "ui_faceTrainer.h"
 #include "faceTrainerAdvSettings.h"
//#include "opencvWebcamThread.h"
#include "opencvWebcam.h"
#include "detector.h"
#include "verifier.h"

 class faceTrainer: public QMainWindow
    {
        Q_OBJECT

    public:
        faceTrainer(QWidget *parent = 0);
        faceTrainerAdvSettings* newDialog;
        opencvWebcam webcam;
        detector newDetector;
        verifier newVerifier;
        void populateQList();

    private:
        Ui::faceTrainer ui;
  //  	opencvWebcamThread * thread;
protected:
    void        timerEvent( QTimerEvent * );

  private slots:
        void captureClick();
        void showTab3();
        void showTab2();
        void showTab1();
        void removeSelected();
     //   void verify();
        void showAdvDialog();
//        void butClick();


  public slots:
        void setQImageWebcam(QImage *);
        void setIbarText(char *message);

    };
