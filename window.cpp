#include "window.h"
#include "ui_window.h"
#include "graphicsscene.h"

#include <QGraphicsView>
#include <QPainter>
#include <QColorDialog>

QPalette pal;

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
    canvas = new GraphicsScene();                                     //sender canvas
    creceive = new GraphicsScene();                                   //receiver canvas
    ui->setupUi(this);

    //make graphics view display its respective graphics scene
    ui->view->setScene(canvas);
    ui->receiveview->setScene(creceive);

    //set canvas size
    ui->receiveview->setSceneRect(-600,-300,300,300);
    ui->view->setSceneRect(-600,-300,300,300);

    canvas->setBackgroundBrush(Qt::white);

    this->setWindowTitle("Whiteboard Chat for RPi");                  //set window title

    //shows user the pen colour
    pal.setColor(QPalette::Window, Qt::black);
    ui->currentcolour->setPalette(pal);
}

Window::~Window()
{
    delete ui;
}

void Window::on_btnclear_released()
{
    ui->view->scene()->clear();                                         //clears the sender graphicsscene

    //serialise data and send to sender
    QByteArray buffer;
    QDataStream serial(&buffer, QIODevice::WriteOnly);
    bool clear=true;
    serial << clear;
    emit(sendclear(buffer, 'w'));
}

void Window::on_pensize_valueChanged(int size)
{
    canvas->pen.setWidth(size);                                         //sets the penwidth of the sender to the value that has been selected
    QByteArray buffer;
    QDataStream serial(&buffer, QIODevice::WriteOnly);                  //data stream writes to buffer
    serial << size;                                                     //converts the colour into bytes and stores in the array
    emit(sendsize(buffer, 's'));                                        //emits size change signal

}

void Window::changecolour(QColor colour)
{
    canvas->pen.setColor(colour);                                       //changes the sender pen to the chosen colour
    pal.setColor(QPalette::Window, colour);                             //changes the colour of the palette to the chosen colour
    ui->currentcolour->setPalette(pal);                                 //displays the current chosen colour by changing the bvackground of a label via the palette
    QByteArray buffer;
    QDataStream serial(&buffer, QIODevice::WriteOnly);                  //data stream writes to buffer
    serial << colour;                                                   //converts the colour into bytes and stores in the array
    emit(sendcolour(buffer, 'c'));                                      //emits the colour change signal
}

void Window::on_penblack_released()
{
    changecolour(Qt::black);
}

void Window::on_penred_released()
{
    changecolour(Qt::red);
}

void Window::on_penblue_released()
{
    changecolour(Qt::blue);
}

void Window::on_pengreen_clicked()
{
    changecolour(Qt::green);
}

void Window::on_penrubber_released()
{
    changecolour(canvas->backgroundBrush().color());
}

void Window::on_pencustom_released()
{
    QColor customcolor = QColorDialog::getColor(canvas->pen.color());     //Displays a colour wheel dialog which allows the user to choose any colour and once it is selected,
                                                                         //it is saved in the variable, the initial colour selected on the colour dialog is
                                                                         //the most recently chosen colour which allows quick adjustments to be made
    changecolour(customcolor);
}

void Window::on_bgcolour_released()
{
    QColor bgcolour = QColorDialog::getColor(canvas->backgroundBrush().color());    //select background colour

    canvas->setBackgroundBrush(bgcolour);                               //set background colour on sender

    QByteArray buffer;
    QDataStream serial(&buffer, QIODevice::WriteOnly);                  //data stream writes to buffer
    serial << bgcolour;                                                 //converts the colour into bytes and stores in the array
    emit(sendcolour(buffer, 'b'));                                      //emits the background colour change signal
}
