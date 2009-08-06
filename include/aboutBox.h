/** @file */

#include "ui_aboutDialog.h"
/**
* About Dialog Class. The aboutBox QDialog
*/
class aboutBox: public QDialog
{
    Q_OBJECT
private:
    /**
    *UI Object of the Face Trainer Main Window
    */
    Ui::aboutDialog ui;

public:
    /**
    *The Constructor
    *Sets up the UI for the Main Window and connect Signals to Slots
    */
    aboutBox(QWidget *parent = 0);
};
