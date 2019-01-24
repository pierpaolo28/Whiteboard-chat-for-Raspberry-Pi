#include "interface.h"

interface::interface(QObject *parent) :
    QObject(parent)
{
}

void interface::data(QWaitCondition *c, QWaitCondition *cn, int b)
{
    waitcond = c;
    low = cn;
    bit = b;
}

void interface::process()
{
    int read;
    forever
    {
        read = digitalRead(bit);        //reads the state of the pin
        if (read==1)                    //pin is high
        {
            waitcond->wakeAll();
        }
        else                            //pin is low
        {
            low->wakeAll();
        }
    }
}
