#include <QtGui>

class opencvWebcamThread : public QThread
{
    Q_OBJECT

public:
    QObject  *parent;
    opencvWebcamThread(QObject *parent);
    void run();
};


