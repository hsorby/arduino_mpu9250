#include "mainwindow.h"

#include <QThread>
#include <iostream>

#if defined (_WIN32) || defined( _WIN64)
    // Library effective with Windows
    #include <windows.h>
#else
    // Library effective with Linux
    #include <unistd.h>
#endif


#include "MadgwickAHRS.h"

// Constructor of the main window
// Create window properties, menu etc ...
MainWindow::MainWindow(QWidget *parent,int w, int h)
    : QMainWindow(parent)
{        
    // Set the window size
    this->resize(w,h);
    this->setWindowTitle("Object viewer");

    // Create a layout in the main window
    centralWidget = new QWidget(this);
    gridLayoutWidget = new QWidget(centralWidget);
    gridLayoutWidget->setGeometry(QRect(0, 0, this->width(), this->height()));
    gridLayout = new QGridLayout(gridLayoutWidget);

    // Create the openGL display for the map
    Object_GL = new ObjectOpenGL(gridLayoutWidget);
    Object_GL->setObjectName(QString::fromUtf8("ObjectOpenGL"));
    Object_GL->setGeometry(QRect(0, 0, this->width(), this->height()));

    // Insert the Open Gl display into the layout
    gridLayout->addWidget(Object_GL, 0, 0, 1, 1);
    setCentralWidget(centralWidget);

    // Create the menubar
    QMenu *FileMenu = menuBar()->addMenu("&File");
    FileMenu->addSeparator();
    // FileMenu->addAction("Quit", qApp, SLOT(quit()), QKeySequence(tr("Ctrl+Q")));
    QAction *action = FileMenu->addAction("Quit", QKeySequence(tr("Ctrl+q")));
    QObject::connect(action, &QAction::triggered, this, &QMainWindow::close);

    // Add menu items
    QMenu *ViewMenu = menuBar()->addMenu("&View");
    ViewMenu->addAction("Front view", QKeySequence(tr("Ctrl+f")), Object_GL, &ObjectOpenGL::FrontView);
    ViewMenu->addAction("Rear view", QKeySequence(tr("Ctrl+e")), Object_GL, &ObjectOpenGL::RearView);
    ViewMenu->addAction("Left view", QKeySequence(tr("Ctrl+l")), Object_GL, &ObjectOpenGL::LeftView);
    ViewMenu->addAction("Right view", QKeySequence(tr("Ctrl+r")), Object_GL, &ObjectOpenGL::RightView);
    ViewMenu->addAction("Top view", QKeySequence(tr("Ctrl+t")), Object_GL, &ObjectOpenGL::TopView);
    ViewMenu->addAction("Bottom view", QKeySequence(tr("Ctrl+b")), Object_GL, &ObjectOpenGL::BottomView);
    FileMenu->addSeparator();
    ViewMenu->addAction("Isometric", QKeySequence(tr("Ctrl+i")), Object_GL, &ObjectOpenGL::IsometricView);
    QMenu *AboutMenu = menuBar()->addMenu("?");
    AboutMenu->addAction("About Convert_STL_2_Cube", this, SLOT (handleAbout()));

    // Timer (used for repainting the GL Window every 50 ms)
    QTimer *timerDisplay = new QTimer();
    timerDisplay->connect(timerDisplay, SIGNAL(timeout()),this, SLOT(onTimer_UpdateDisplay()));
    timerDisplay->start(75);


    // Timer for reading raw data (every 10ms)
    QTimer *timerArduino = new QTimer();
    timerArduino->connect(timerArduino, SIGNAL(timeout()),this, SLOT(onTimer_ReadData()));
    timerArduino->start(10);
}




// Desctructor
MainWindow::~MainWindow()
{}




// On resize event, the items in the window are resized
void MainWindow::resizeEvent(QResizeEvent *)
{
    Object_GL->resize(centralWidget->width(),centralWidget->height());
    gridLayoutWidget->setGeometry(QRect(0, 0, centralWidget->width(), centralWidget->height()));
}




// Timer event : repain the Open Gl window
void MainWindow::onTimer_UpdateDisplay()
{
    Object_GL->update();
}





#define         RATIO_ACC       (4./32767.)
#define         RATIO_GYRO      ((1000./32767.)*(M_PI/180.))
//#define         RATIO_GYRO      (1000./32767.)
#define         RATIO_MAG       (48./32767.)

// Timer event : get raw data from Arduino
void MainWindow::onTimer_ReadData()
{
    /*
     * Check if there is any data in the RX buffer.
     */
    if(mpu9250.peekReceiver())
    {
        // Read data from MPU-9250
        char buffer[200];

        mpu9250.readString(buffer, '\n', 200, 10);
        // std::cout << "buffer: " << buffer << std::endl;
        //mpu9250.readStringNoTimeOut(buffer,'\n',200);


        // Parse raw data
        int  it;
        float ax,ay,az;
        float gx,gy,gz;
        float mx,my,mz;
        float time;
        sscanf (buffer,"%d %f %f %f %f %f %f %f %f %f %f",&it,&ax,&ay,&az,&gx,&gy,&gz,&mx,&my,&mz, &time);


        // Display raw data
        // std::cout << "reading: " << it << "\t";
        // std::cout << ax << "\t" << ay << "\t" << az << "\t";
        // std::cout << gx << "\t" << gy << "\t" << gz << "\t";
        // std::cout << mx << "\t" << my << "\t" << mz << "\t";
        // std::cout << time << std::endl;


        // ax=iax*RATIO_ACC;
        // ay=iay*RATIO_ACC;
        // az=iaz*RATIO_ACC;

        // gx=(igx-48.4827)*RATIO_GYRO;
        // gy=(igy+76.3552)*RATIO_GYRO;
        // gz=(igz+64.3234)*RATIO_GYRO;

        // mx=imx*RATIO_MAG;
        // my=imy*RATIO_MAG;
        // mz=imz*RATIO_MAG;

        Object_GL->setAcceleromter(ax,ay,az);
        Object_GL->setGyroscope(gx,gy,gz);
        Object_GL->setMagnetometer(mx,my,mz);

        MadgwickAHRSupdate(gx,gy,gz,ax,ay,az,mx,my,mz);
        //        MadgwickAHRSupdateIMU(gx,gy,gz,ax,ay,az);
        std::cout << "Madgwick AHRS update: " << q0 << " \t" << q1 << " \t" << q2 << " \t" << q3 << std::endl;

        double R11 = 2.*q0*q0 -1 +2.*q1*q1;
        double R21 = 2.*(q1*q2 - q0*q3);
        double R31 = 2.*(q1*q3 + q0*q2);
        double R32 = 2.*(q2*q3 - q0*q1);
        double R33 = 2.*q0*q0 -1 +2.*q3*q3;

        double phi = atan2(R32, R33);
        double theta = -atan(R31 / sqrt(1-R31*R31));
        double psi = atan2(R21, R11);



        std::cout << R31 << "\t" << phi*180./M_PI << "\t" << theta*180./M_PI << "\t" << psi*180./M_PI << std::endl;
        Object_GL->setAngles(phi*180./M_PI , theta*180./M_PI , psi*180./M_PI );

        /*
        std::cout << it/1000. << "\t";
        std::cout << ax << "\t" << ay << "\t" << az << "\t";
        std::cout << gx << "\t" << gy << "\t" << gz << "\t";
        std::cout << mx << "\t" << my << "\t" << mz << "\t";
        std::cout << std::endl;
        */
    }
    else
    {
        usleep(10);
    }
}



// Open the 'about' dialog box
void MainWindow::handleAbout()
{
    QMessageBox::information(this,"About OpenGL Frame","<H2>OpenGL Frame</H2>2011<BR>Supported by the Cart-O-Matic project (ANR CAROTTE)<BR>University of Angers (France,Europe)<BR>Designed by Philippe Lucidarme <BR> Contact: philippe.lucidarme@univ-angers.fr. ");
}


// Connect to the serial device (Arduino)
bool MainWindow::connect()
{
    // Connect to serial port
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    std::cout << "Attempting to open: " << env.value("MPU9250_DEVICE_NAME", "/dev/null").toStdString() << " ...";
    if (mpu9250.openDevice(env.value("MPU9250_DEVICE_NAME", "/dev/null").toStdString().c_str(), env.value("MPU9250_BAUD_RATE", "115200").toUInt()) != 1)
    {
        std::cout << " Failure" << std::endl;
        std::cerr << "Error while opening serial device" << std::endl;
        return false;
    }

    // Flush receiver of previously received data
    std::cout << " Success" << std::endl;
    usleep(100);
    mpu9250.flushReceiver();
    return true;
}
