
 #include "ui_faceAuth.h"

 class faceAuth: public QDialog
    {
        Q_OBJECT

    public:
        faceAuth(QWidget *parent = 0);


    private:
        Ui::faceAuth ui;

    private slots:
        void authClicked();
        void cancelClicked();



    };
