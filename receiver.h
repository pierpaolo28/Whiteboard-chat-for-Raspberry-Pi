#ifndef RECEIVER_H
#define RECEIVER_H
#include <QObject>
#include <QBitArray>
#include <QChar>
#include <QMutex>
#include <QWaitCondition>
#include <wiringPi.h>

//code developed from: https://wiki.qt.io/QThreads_general_usage
//and http://doc.qt.io/qt-5/threads-technologies.html

class Receiver : public QObject
{
    Q_OBJECT

public:
    explicit Receiver(QObject *parent = 0);
    void setbits(int d, int s, QMutex* m, QWaitCondition* rc, QWaitCondition* nrc);
    QByteArray bittobyte(QBitArray convert);    //convert received data back to bytes

public slots:
    void onsenderready();                       //process to read data sent from sender

signals:
    void rpos(QByteArray);                      //received data is position
    void rsize(QByteArray);                     //received data is size
    void rcolour(QByteArray);                   //received data is colour
    void rtoggle(QByteArray);                   //received toggle
    void rbackground(QByteArray);               //received background colour
    void rclear();                              //received clear
    void done();                                //when receiver has finished reading the data packet


private:
    QMutex* mmutex;
    QWaitCondition* readycond;                  //when ready bit is high
    QWaitCondition* notreadyc;                  //when ready bit is low

    int pdata;                                  //data pin number
    int psent;                                  //sent pin number
};

#endif // RECEIVER_H
