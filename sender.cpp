#include "sender.h"
#include <QDataStream>
#include <QDebug>

Sender::Sender(QObject *parent) :
    QObject(parent)
{
}

void Sender::setbits(int r, int d, QMutex* m, QWaitCondition* sc, QWaitCondition* nsc)
{
    pready = r;
    pdata = d;
    mmutex = m;
    sendcond = sc;
    notsendc = nsc;
}

void Sender::ondata(QByteArray data, char type)
{
    QChar stype = (QChar) type;                                 //copnvert type to qchar so it can be serialised

    //serialise the data type
    QByteArray serialtype;
    QDataStream serialize(&serialtype, QIODevice::WriteOnly);
    serialize << stype;

    //convert data into bits
    QBitArray sendtype = bytetobit(serialtype);
    QBitArray senddata = bytetobit(data);

    qDebug() << "sending bits:" << sendtype;

    for (unsigned int i = 0; i<sizeof(QChar)*8; i++)
    {
        mmutex->lock();

        int write = sendtype.at(i) ? 1 : 0;                 //convert bool into int
        digitalWrite(pdata, write);                         //write to data pin
        digitalWrite(pready, 1);                            //set ready pin high
        sendcond->wait(mmutex);                             //wait until send pin is high

        digitalWrite(pready, 0);                            //set ready pin low
        notsendc->wait(mmutex);                             //wait for send pin to go low

        mmutex->unlock();
    }

    qDebug() << "sent type: " << type;
    qDebug() << "Data size:"<<senddata.size();
    qDebug() << "Sending data:"<<senddata;

    //write data same as before
    for (int j = 0; j<senddata.size(); j++)
    {
        mmutex->lock();

        int write = senddata.at(j) ? 1 : 0;
        digitalWrite(pdata, write);
        digitalWrite(pready, 1);
        sendcond->wait(mmutex);

        digitalWrite(pready, 0);
        notsendc->wait(mmutex);

        mmutex->unlock();
    }

    qDebug() << "sender finished";
}

QBitArray Sender::bytetobit(QByteArray convert)                     //convert bytes to bits, code developed from: https://wiki.qt.io/Working_with_Raw_Data
{
    QBitArray bits(convert.count()*8);
    for(int i=0; i<convert.count(); ++i)
    {
        for(int b=0; b<8; ++b)
        {
                bits.setBit(i*8+b, convert.at(i)&(1<<(7-b)));
        }
    }

    return bits;
}
