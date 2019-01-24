#include "receiver.h"
#include <QDataStream>
#include <QDebug>
#include <QColor>
#include <QPoint>

Receiver::Receiver(QObject *parent) :
    QObject(parent)
{
}

void Receiver::setbits(int d, int s, QMutex* m, QWaitCondition* rc, QWaitCondition* nrc)
{
    pdata = d;
    psent = s;
    mmutex = m;
    readycond = rc;
    notreadyc = nrc;
}

void Receiver::onsenderready()
{
    QBitArray readchar(sizeof(QChar)*8);                            //bit to write the data type to - char

    for (int m=0; m<readchar.size(); m++)                           //read data up to the size of char
    {
        mmutex->lock();

        readycond->wait(mmutex);                                    //wait for ready bit to go high

        int read = digitalRead(pdata);                              //read data bit
        bool bit = (read==1) ? true : false;                        //convert read from int to bool
        readchar.setBit(m, bit);                                    //writes value to array
        digitalWrite(psent, 1);                                     //set sent pin high
        notreadyc->wait(mmutex);                                    //wait for ready pin to go low

        digitalWrite(psent, 0);                                     //set sent pin low

        mmutex->unlock();
    }

    qDebug() << "received:" << readchar;

    QByteArray dtype= bittobyte(readchar);                          //convert received data into bits

    //deserialise data
    QDataStream deserialize(dtype);
    QChar rtype;
    deserialize >> rtype;
    qDebug() << "received type:" << rtype;

    QBitArray readdata;

    //resize data array depending on what type the data is
    if(rtype== 'p')
    {
        readdata.resize(64);                                //size of qpoint
    }
    else if(rtype=='s')
    {
        readdata.resize(sizeof(int)*8);                     //size of int
    }
    else if(rtype=='c')
    {
        readdata.resize(88);                                //size of qcolor
    }
    else if(rtype=='t')
    {
        readdata.resize(8);                                 //size of bool
    }
    else if(rtype=='w')
    {
        readdata.resize(8);                                 //size of bool
    }
    else if(rtype=='b')
    {
        readdata.resize(88);                                //size of qcolor
    }


    //read data same as before
    for (int n=0; n<readdata.size(); n++)
    {
        mmutex->lock();

        readycond->wait(mmutex);

        int read = digitalRead(pdata);
        bool bit = (read==1) ? true : false;
        readdata.setBit(n, bit);

        digitalWrite(psent, 1);
        notreadyc->wait(mmutex);

        digitalWrite(psent, 0);

        mmutex->unlock();
    }
    qDebug() << "received:" << readdata;

    //convert received data back into bytes
    QByteArray rdata = bittobyte(readdata);

    //send signal according to type of data received
    if(rtype== 'p')
    {
        emit(rpos(rdata));
    }
    else if(rtype=='s')
    {
        emit(rsize(rdata));
    }
    else if(rtype=='c')
    {
        emit(rcolour(rdata));
    }
    else if(rtype=='t')
    {
        emit(rtoggle(rdata));
    }
    else if(rtype=='w')
    {
        emit(rclear());
    }
    else if(rtype=='b')
    {
        emit(rbackground(rdata));
    }

    qDebug() << "receiver finished";
    emit done();
}

QByteArray Receiver::bittobyte(QBitArray convert)                           //code developed from: https://wiki.qt.io/Working_with_Raw_Data
{
    QByteArray bytes;
    bytes.resize(convert.count()/8+1);
    bytes.fill(0);

    for(int b=0; b<convert.count(); ++b)
    {
        bytes[b/8] = ( bytes.at(b/8) | ((convert[b]?1:0)<<(7-(b%8))));
    }
    return bytes;
}
