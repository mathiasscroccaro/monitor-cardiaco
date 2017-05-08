#include "mainwindow.h"
#include "uart.h"
#include <QThread>
#include <QApplication>

bool play = true;
QVector <char> rxUart;
QMutex mutex;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<QVector<unsigned char>>("QVector<unsigned char>");
    qRegisterMetaType<QVector<float>>("QVector<float>");

    MainWindow w;
    w.show();



    //Uart * com = new Uart(250000,QString("COM7"));
    //QThread * tUart = new QThread();

    //QObject::connect(tUart, SIGNAL(started()), com, SLOT(run()) );

    //com->moveToThread(tUart);

    //tUart->start(QThread::HighestPriority);

    return a.exec();
}
