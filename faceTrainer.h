 #include "ui_faceTrainer.h"
//#include "opencvWebcamThread.h"
#include "opencvWebcam.h"
#include "detector.h"
 class faceTrainer: public QMainWindow
    {
        Q_OBJECT

    public:
        faceTrainer(QWidget *parent = 0);
        opencvWebcam webcam;
        detector newDetector;


    private:
        Ui::faceTrainer ui;
  //  	opencvWebcamThread * thread;
protected:
    void        timerEvent( QTimerEvent * );

  private slots:
        void showTab3();
        void showTab2();
        void showTab1();
  public slots:
        void setQImageWebcam(QImage *);
        void setIbarText(char *message);

    };
