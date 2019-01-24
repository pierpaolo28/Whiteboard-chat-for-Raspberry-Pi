#ifndef SENDER_H
#define SENDER_H
#include <QObject>
#include <QBitArray>
#include <QChar>
#include <QMutex>
#include <QWaitCondition>
#include <wiringPi.h>

//code developed from: https://wiki.qt.io/QThreads_general_usage
//and http://doc.qt.io/qt-5/threads-technologies.html

class Sender : public QObject {
    Q_OBJECT

public:
    explicit Sender(QObject *parent = 0);
    void setbits(int r, int d, QMutex* m, QWaitCondition* sc, QWaitCondition* nsc);
    QBitArray bytetobit(QByteArray convert);                        //convert data from bytes into bits

public slots:
    void ondata(QByteArray data, char type);                        //type: p=point, s=size, c=colour, t=toggle, w=clear, b=background

signals:

private:
    QMutex* mmutex;
    QWaitCondition* sendcond;                                       //when send pin is high
    QWaitCondition* notsendc;                                       //when send pin is low

    int pready;                                                     //ready pin
    int pdata;                                                      //data pin
};

#endif // SENDER_H
