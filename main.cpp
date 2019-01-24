#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <pthread.h>
#include <wiringPi.h>

#include "window.h"
#include "graphicsscene.h"
#include "sender.h"
#include "receiver.h"
#include "interface.h"

int main(int argc, char *argv[])
{

    // setup GPIO interface - uncomment when needed
    // needs to run with root via sudo in terminal.
    wiringPiSetup();

    //sender
    pinMode (0, OUTPUT);                                                        //ready bit
    pinMode (1, OUTPUT);                                                        //data bit
    pinMode (3, INPUT);                                                         //sent bit

    //receiver
    pinMode (4, INPUT);                                                         //ready
    pinMode (5, INPUT);                                                         //data
    pinMode (7, OUTPUT);                                                        //sent

    digitalWrite(0,0);
    digitalWrite(1,0);
    digitalWrite(7,0);

    QApplication a(argc, argv);
    Window whiteboard;
    whiteboard.show();                                                            //initialise window


    QMutex mutex;

    //wait conditions allow threads to wait until a certain condition is met, in this case, when a signal pin is high or low
    QWaitCondition cready;
    QWaitCondition cnready;
    QWaitCondition csent;
    QWaitCondition cnsent;

    Sender* sender = new Sender();                                              //sends serialised data
    sender->setbits(0, 1, &mutex, &csent, &cnsent);
    interface* sentinterface = new interface();                                 //reads input to sender
    sentinterface->data(&csent, &cnsent, 3);


    Receiver* receiver = new Receiver();                                        //receives serialised data
    receiver->setbits(5, 7, &mutex, &cready, &cnready);
    interface* readinterface = new interface();                                 //reads input to receiver
    readinterface->data(&cready, &cnready, 4);

    //threads for each object so they can run in parallel
    QThread* sendthread = new QThread;
    QThread* receivethread = new QThread;
    QThread* sinterfacethread = new QThread;
    QThread* rinterfacethread = new QThread;

    //move each object to its respective thread
    sender->moveToThread(sendthread);
    receiver->moveToThread(receivethread);
    readinterface->moveToThread(rinterfacethread);
    sentinterface->moveToThread(sinterfacethread);

    //connect local signals to sender so that they can be sent to receiver
    QObject::connect(whiteboard.canvas, &GraphicsScene::senddata, sender, &Sender::ondata);
    QObject::connect(&whiteboard, &Window::sendcolour, sender, &Sender::ondata);
    QObject::connect(&whiteboard, &Window::sendsize, sender, &Sender::ondata);
    QObject::connect(&whiteboard, &Window::sendclear, sender, &Sender::ondata);

    QObject::connect(rinterfacethread, &QThread::started, readinterface, &interface::process);
    QObject::connect(sinterfacethread, &QThread::started, sentinterface, &interface::process);
    QObject::connect(receivethread, &QThread::started, receiver, &Receiver::onsenderready);

     //once receiver has finished receiving, prepares to receive the next packet
    QObject::connect(receiver, &Receiver::done, receiver, &Receiver::onsenderready);

    //connect deserialised signals from receiver to receiver canvas
    QObject::connect(receiver, &Receiver::rpos, whiteboard.creceive, &GraphicsScene::onsendpos);
    QObject::connect(receiver, &Receiver::rsize, whiteboard.creceive, &GraphicsScene::onsizechange);
    QObject::connect(receiver, &Receiver::rcolour, whiteboard.creceive, &GraphicsScene::oncolourchange);
    QObject::connect(receiver, &Receiver::rbackground, whiteboard.creceive, &GraphicsScene::onbackground);
    QObject::connect(receiver, &Receiver::rtoggle, whiteboard.creceive, &GraphicsScene::ontoggledraw);
    QObject::connect(receiver, &Receiver::rclear, whiteboard.creceive, &GraphicsScene::onclear);

    //start threads
    rinterfacethread->start();
    sinterfacethread->start();
    sendthread->start();
    receivethread->start();


    // start window event loop
    qDebug() << "Starting event loop...";
    int ret = a.exec();
    qDebug() << "Event loop stopped.";

    sendthread->quit();
    sendthread->terminate();
    receivethread->quit();
    receivethread->terminate();
    rinterfacethread->quit();
    rinterfacethread->terminate();
    sinterfacethread->quit();
    sinterfacethread->terminate();

    sentinterface->~interface();
    readinterface->~interface();
    sender->~Sender();
    receiver->~Receiver();

    mutex.unlock();
    qDebug() << "here1";
    mutex.unlock();
    cnready.wakeAll();
    cnready.~QWaitCondition();
    qDebug() << "here2";
    mutex.unlock();
    cnsent.wakeAll();
    cnsent.~QWaitCondition();
    qDebug() << "here3";
    mutex.unlock();
    cready.wakeAll();
    cready.~QWaitCondition();
    qDebug() << "here4";
    mutex.unlock();
    csent.wakeAll();
    csent.~QWaitCondition();
    qDebug() << "here5";

    //mutex.unlock();
    mutex.~QMutex();

    digitalWrite(0,0);
    digitalWrite(1,0);
    digitalWrite(7,0);

    return ret;
}

