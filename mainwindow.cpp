#include "mainwindow.h"

#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
}

#include "logger.h"
#include "rtspvideostreamdecoder.h"
#include "rtspwidget.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);

//    av_register_all();
//    avdevice_register_all();
//    avcodec_register_all();
//    avformat_network_init();

//    createCamera("rtsp://:8554/test");
    createCamera("rtsp://admin:admin@192.168.7.71:80/ch0_0.264");

    return;
}


void MainWindow::createCamera(const QString & rtsp_url)
{
    rtsp_widget = new RtspWidget(this);
    leRtspUrl = new QLineEdit(this);
    leRtspUrl->setText(rtsp_url);
    startButton = new QPushButton("Start");
    stopButton = new QPushButton("Stop");
    stopButton->hide();

    QVBoxLayout * vl = new QVBoxLayout();

    vl->addWidget(leRtspUrl);
    vl->addWidget(rtsp_widget);
    vl->addWidget(startButton);
    vl->addWidget(stopButton);

    bool connected = false;

    connected = connect (startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked, Qt::UniqueConnection);
    assert(connected);
    connected = connect (stopButton, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked, Qt::UniqueConnection);
    assert(connected);

    connected = connect (rtsp_widget, &RtspWidget::started, this, &MainWindow::onRtspStarted, Qt::UniqueConnection);
    assert(connected);
    connected = connect (rtsp_widget, &RtspWidget::stopped, this, &MainWindow::onRtspStopped, Qt::UniqueConnection);
    assert(connected);




    ui->camerasLayout->addLayout(vl);
}

void MainWindow::onStartButtonClicked()
{
    rtsp_widget->start(leRtspUrl->text());
}

void MainWindow::onStopButtonClicked()
{
    rtsp_widget->stop();
}

void MainWindow::onRtspStarted()
{
    stopButton->show();
    startButton->hide();
}

void MainWindow::onRtspStopped()
{
    stopButton->hide();
    startButton->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

