#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QThread>
#include <wiringPi.h>

class interface : public QObject
{
    Q_OBJECT
public:
    explicit interface(QObject *parent = 0);
    void data(QWaitCondition* c, QWaitCondition* cn, int b);
    void process();                                                 //process that the object runs - polling its respective pin

signals:

public slots:

private:
    QWaitCondition* waitcond;                                       //when pin is high
    QWaitCondition* low;                                            //when pin is low
    int bit;                                                        //pin number that the object polls
};

#endif // INTERFACE_H
