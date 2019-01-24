#include "graphicsscene.h"
#include <QPainterPath>
#include <QDebug>
#include <QGraphicsPathItem>
#include <QDataStream>

GraphicsScene::GraphicsScene(QObject *parent) :                             //constructor
    QGraphicsScene(parent)
{
    pen = QPen(Qt::black,5,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);        //defines the pen that will be used to draw on the canvas
    drawing = false;                                                        //initialises the drawing variable
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{

    QPoint pos = mouseEvent->scenePos().toPoint();                          //Gets the position of the mouse where it has been clicked

    QByteArray spos = serializept(pos);                                     //serialises the position into a byte array
    emit senddata(spos, 'p');                                               //sends the serialised data to the sender

    lastpoint = pos;                                                        //saves the position so that a line can be drawn if the mouse is moved
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    //send signal notifying to the receiver that the canvas is being drawn on
    QByteArray buffer;
    bool toggle = true;
    QDataStream serial(&buffer, QIODevice::WriteOnly);
    serial << toggle;
    emit(senddata(buffer, 't'));

    QPoint pos = mouseEvent->scenePos().toPoint();                             //position of the mouse on the canvas
    this->addLine(lastpoint.x(),lastpoint.y(),pos.x(),pos.y(),pen);            //draws line from last mouse position to current position

    //send position to sender
    QByteArray spos = serializept(pos);
    emit senddata(spos, 'p');

    lastpoint=pos;                                                             //saves position so line can be drawn if mouse is moved again
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    //notifies receiver that drawing has stopped
    QByteArray buffer;
    bool toggle = false;
    QDataStream serial(&buffer, QIODevice::WriteOnly);
    serial << toggle;
    emit(senddata(buffer, 't'));
}

void GraphicsScene::onsendpos(QByteArray spos)
{
    //deserialise position
    QDataStream deserial(spos);
    QPoint rpoint;
    deserial >> rpoint;

    if(this->drawing==true)
    {
        this->addLine(lastpoint.x(),lastpoint.y(),rpoint.x(),rpoint.y(),pen);   //draws line from last position to received position
    }
    lastpoint=rpoint;                                                           //saves the received position as the lastpoint so next line can be drawn
}

void GraphicsScene::onsizechange(QByteArray ssize)
{
    //deserialise
    QDataStream deserial(ssize);
    int rsize;
    deserial >> rsize;
    qDebug() << "Received size";

    this->pen.setWidth(rsize);                                              //changes pen width to received size
}

void GraphicsScene::oncolourchange(QByteArray scolour)
{
    //deserialise
    QDataStream deserial(scolour);
    QColor rcolour;
    deserial >> rcolour;
    qDebug() << "Received pen colour" << rcolour;

    this->pen.setColor(rcolour);                                            //changes pen colour to received colour
}

void GraphicsScene::onbackground(QByteArray sbg)
{
    //deserialise
    QDataStream deserial(sbg);
    QColor rbg;
    deserial >> rbg;
    qDebug() << "Received background colour" << rbg;

    this->setBackgroundBrush(rbg);
}

void GraphicsScene::onclear()
{
    qDebug() << "Received clear";
    this->clear();                                                          //clears canvas
}

void GraphicsScene::ontoggledraw(QByteArray stoggle)
{
    //deserialise
    QDataStream deserial(stoggle);
    bool rtoggle;
    deserial >> rtoggle;
    qDebug() << "Received toggle: " << rtoggle;

    drawing = rtoggle;                                                      //set drawing boolean to received value
}

QByteArray GraphicsScene::serializept(QPoint point)                         //code developed from http://www.java2s.com/Code/Cpp/Qt/SerializationwithQDataStream.htm
{
    QByteArray buffer;                                                      //array of bytes
    QDataStream serial(&buffer, QIODevice::WriteOnly);                      //data stream writes to buffer array
    serial << point;                                                        //converts the point into bytes and stores in the array
    return buffer;
}
