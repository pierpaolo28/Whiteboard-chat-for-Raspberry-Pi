#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

//developed from http://doc.qt.io/qt-5/qtwidgets-widgets-scribble-example.html

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GraphicsScene(QObject *parent = 0);                                    //constructor

    //mouse events - used for drawing
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

    QByteArray serializept(QPoint point);                                           //function to serialise points

    QPen pen;                                                                       //pen to draw on canvas
    QPoint lastpoint;                                                               //the last point that the mouse was at
    bool drawing;                                                                   //on when the canvas is being drawn on

public slots:
    void onsendpos(QByteArray spos);                                                 //slot for when a line is recieved from sender
    void onsizechange(QByteArray ssize);                                             //slot for when the pen size is changed on the sender
    void oncolourchange(QByteArray scolour);                                         //slot for when pen colour is changed on sender
    void onbackground(QByteArray sbg);
    void onclear();                                                                  //slot for when canvas is cleared
    void ontoggledraw(QByteArray stoggle);                                           //slot to set the draw boolean

signals:
    void senddata(QByteArray data, char type);                                       //sends data to sender thread
};

#endif // GRAPHICSSCENE_H
