#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include "graphicsscene.h"

namespace Ui {
class Window;
}

class Window : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Window(QWidget *parent = 0);
    ~Window();
    GraphicsScene * canvas;                     //sender canvas
    GraphicsScene * creceive;                   //receiver graphicsscene
    void changecolour(QColor colour);           //function to change colours

    
private slots:
    //GUI button click events
    void on_btnclear_released();
    void on_pensize_valueChanged(int arg1);
    void on_penblack_released();
    void on_penred_released();
    void on_penblue_released();
    void on_pengreen_clicked();
    void on_penrubber_released();
    void on_pencustom_released();

    void on_bgcolour_released();

signals:
    //send data to sender
    void sendsize(QByteArray ssize, char type);
    void sendcolour(QByteArray scolor, char type);
    void sendclear(QByteArray clear, char type);

private:
    Ui::Window *ui;
};

#endif // WINDOW_H
