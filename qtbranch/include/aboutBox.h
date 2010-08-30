#ifndef _INCL_FACE_TRAINER_ABOUT
#define _INCL_FACE_TRAINER_ABOUT

#include "ui_aboutDialog.h"

/**
* About Dialog Class. The aboutBox QDialog
*/
class aboutBox: public QDialog
{
    Q_OBJECT

    private:
        /**
        * UI object of the face trainer main window
        */
        Ui::aboutDialog ui;

    public:
        /**
        * The constructor
        * Sets up the UI for the Main window and connect signals to slots
        */
        aboutBox(QWidget *parent = 0);
};

#endif // _INCL_FACE_TRAINER_ABOUT
