#pragma once

#include <QtWidgets/QMainWindow>
#include <QGridLayout>
#include <QMenuBar>
#include <QMessageBox>


#include "rOc_serial.h"
#include "objectgl.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor and destructor
    MainWindow(QWidget *parent = 0, int w=600, int h=400);
    ~MainWindow();


    bool                    connect();

protected slots:
    // Redraw the scene
    void                    onTimer_UpdateDisplay();

    // Get raw data from Arduini
    void                    onTimer_ReadData();

    // Open the about dialog box
    void                    handleAbout();

protected:

    // Overload of the resize event
    void                    resizeEvent(QResizeEvent *);

private:

    // Layout of the window
    QGridLayout             *gridLayout;
    QWidget                 *gridLayoutWidget;

    // Central widget (where the openGL window is drawn)
    QWidget                 *centralWidget;

    // OpenGL object
    ObjectOpenGL            *Object_GL;

    // Serial device for communicating with the Arduino
    rOc_serial mpu9250;

};

