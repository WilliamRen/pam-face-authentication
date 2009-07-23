 #include "ui_aboutDialog.h"

 class aboutBox: public QDialog
    {
        Q_OBJECT
    private:
        Ui::aboutDialog ui;

    public:
        aboutBox(QWidget *parent = 0);
    };
