/*
    qt-facetrainer main
    Copyright (C) 2010 Rohan Anil (rohan.anil@gmail.com) -BITS Pilani Goa Campus
    http://www.pam-face-authentication.org

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui>
#include <QApplication>
#include "faceTrainer.h"

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //tab1.setWindowTitle("Face Trainer");
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("qt_facetrainer_" + QLocale::system().name());
    app.installTranslator(&myappTranslator);

    faceTrainer tab1;
    QRect r = tab1.geometry();
    r.moveCenter(QApplication::desktop()->availableGeometry().center());
    tab1.setGeometry(r);
    tab1.show();

    return app.exec();
}

