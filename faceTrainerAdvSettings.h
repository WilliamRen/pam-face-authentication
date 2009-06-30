 #include "ui_faceTrainerAdvSettings.h"
 class faceTrainerAdvSettings: public QDialog
    {
        Q_OBJECT

    public:
        faceTrainerAdvSettings(QWidget *parent = 0,char *config=0);
        char *configDirectory;

    private:
        Ui::faceTrainerAdvSettings ui;

    private slots:
        void saveClicked();
        void restoreDefaults();



    };
