#include "mainwindow.h"

#include <QPushButton>
#include <QTimer>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
}

#include "logger.h"
#include "rtspvideostreamdecoder.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    //инициализация библиотеки FFMPEG
    av_register_all();
    avdevice_register_all();
    avcodec_register_all();
    avformat_network_init();



    createCamera("rtsp://:8554/test");

    return;
}


void MainWindow::createCamera(const QString & rtsp_url)
{
    lbl = new QLabel(this);
    lbl->setAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    lbl->setWordWrap (true);
    lbl->setStyleSheet("QLabel { background-color : black; color : white; }");
    lbl->setText (rtsp_url);

    RtspVideoStreamDecoder * rtsp = new RtspVideoStreamDecoder(rtsp_url);

    connect(rtsp, &RtspVideoStreamDecoder::finished, this, &MainWindow::onRtspVideoStreamDecoderFinished, Qt::UniqueConnection);
//    connect(tst, &RtspVideoStreamDecoder::finished, tst, &QObject::deleteLater, Qt::UniqueConnection);
    connect(rtsp, &RtspVideoStreamDecoder::newFrame, this, &MainWindow::onRtspNewFrame, Qt::UniqueConnection);
    connect(rtsp, &RtspVideoStreamDecoder::infoChanged, this, &MainWindow::onRtspInfoChanged, Qt::UniqueConnection);
//    rtsp->start();


    checkNewFrameTimer = new QTimer(this);
    connect(checkNewFrameTimer, &QTimer::timeout, this, &MainWindow::onCheckNewFrameTimerTimeout, Qt::UniqueConnection);

    checkNewFrameTimer->setInterval(200);
//    checkNewFrameTimer->start();




    QPushButton * startButton = new QPushButton("Start");
    QPushButton * stopButton = new QPushButton("Stop");
    stopButton->hide();

    QVBoxLayout * vl = new QVBoxLayout();

    vl->addWidget(lbl);
    vl->addWidget(startButton);
    vl->addWidget(stopButton);


    bool connected = false;

    connected = connect (startButton, &QPushButton::clicked, rtsp, &RtspVideoStreamDecoder::start, Qt::UniqueConnection);
    assert(connected);
    connected = connect (stopButton, &QPushButton::clicked, rtsp, &RtspVideoStreamDecoder::stop, Qt::UniqueConnection);
    assert(connected);

    connected = connect (rtsp, &RtspVideoStreamDecoder::finished, startButton, &QPushButton::show, Qt::UniqueConnection);
    assert(connected);
    connected = connect (rtsp, &RtspVideoStreamDecoder::finished, stopButton, &QPushButton::hide, Qt::UniqueConnection);
    assert(connected);

    connected = connect (rtsp, &RtspVideoStreamDecoder::started, startButton, &QPushButton::hide, Qt::UniqueConnection);
    assert(connected);
    connected = connect (rtsp, &RtspVideoStreamDecoder::started, stopButton, &QPushButton::show, Qt::UniqueConnection);
    assert(connected);
    connected = connect (rtsp, &RtspVideoStreamDecoder::started, this, &MainWindow::onThreadStarted, Qt::UniqueConnection);
    assert(connected);


    ui->camerasLayout->addLayout(vl);
}

void MainWindow::onThreadStarted()
{
    checkNewFrameTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

