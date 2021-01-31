#include "mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

#include "logger.h"
#include "rtspwidget.h"


MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    camerasLayout = new QHBoxLayout();
    camerasLayout->setContentsMargins(6, 6, 6, 6);

    this->setLayout(camerasLayout);
    this->resize(320, 240);

//    createCamera("rtsp://mpv.cdn3.bigCDN.com:554/bigCDN/mp4:bigbuckbunnyiphone_400.mp4");
//    createCamera("rtsp://freja.hiof.no:1935/rtplive/definst/hessdalen03.stream");
//    createCamera("rtsp://:8554/test");
//    createCamera("rtsp://admin:admin@192.168.7.71:80/ch0_0.264");
//    createCamera("rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov");
    createCamera("http://158.58.130.148:80/mjpg/video.mjpg");

    return;
}


void MainWindow::createCamera(const QString & rtsp_url)
{
    tmrReconnect = new QTimer(this);
    tmrReconnect->setInterval(2000);
    tmrReconnect->setSingleShot(true);

    leRtspUrl = new QLineEdit();
    leRtspUrl->setText(rtsp_url);

    btnPlay = new QPushButton("Play");
    btnPlay->setCheckable(true);

    controlBox = new QHBoxLayout();
    controlBox->addWidget(leRtspUrl);
    controlBox->addWidget(btnPlay);

    rtsp_widget = new RtspWidget();
    rtsp_widget->setUpdateInterval(50);

    rtspLoyout = new QVBoxLayout();
    rtspLoyout->addLayout(controlBox);
    rtspLoyout->addWidget(rtsp_widget);


    bool connected = false;
    connected = connect (btnPlay, &QPushButton::toggled, this, &MainWindow::onPlayButtonToggled, Qt::UniqueConnection);
    assert(connected);
    connected = connect (rtsp_widget, &RtspWidget::started, this, &MainWindow::onRtspStarted, Qt::UniqueConnection);
    assert(connected);
    connected = connect (rtsp_widget, &RtspWidget::stopped, this, &MainWindow::onRtspStopped, Qt::UniqueConnection);
    assert(connected);
    connected = connect (rtsp_widget, &RtspWidget::clicked, this, &MainWindow::onRtspClicked, Qt::UniqueConnection);
    assert(connected);
    connected = connect (tmrReconnect, &QTimer::timeout, this, &MainWindow::onTmrReconnectTimeout, Qt::UniqueConnection);
    assert(connected);


    camerasLayout->addLayout(rtspLoyout);
}


void MainWindow::onTmrReconnectTimeout()
{
    if (btnPlay->isChecked())
    {
        rtsp_widget->start(leRtspUrl->text());
    }
}


void MainWindow::onPlayButtonToggled(bool checked)
{
    if (checked)
    {
        leRtspUrl->setDisabled(true);
        rtsp_widget->start(leRtspUrl->text());
    }
    else
    {
        leRtspUrl->setDisabled(false);
        rtsp_widget->stop();
    }
}


void MainWindow::onRtspStarted()
{
}


void MainWindow::onRtspClicked()
{
    static QSize sizeBefore;
    static QPoint posBefore;

    if (2 == rtspLoyout->count())
    {
        sizeBefore = this->size();
        posBefore = this->pos();

        QRect rtsp_widget_geom = rtsp_widget->geometry();
        QPoint rtsp_widget_pos = rtsp_widget->mapToGlobal( QPoint(0, 0));

        hide();
        rtspLoyout->takeAt(0);
        camerasLayout->setContentsMargins(0, 0, 0, 0);
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
        this->move(rtsp_widget_pos.x() + 7, rtsp_widget_pos.y());
        this->resize(rtsp_widget_geom.width(), rtsp_widget_geom.height());
        show();
    }
    else
    {
        hide();
        rtspLoyout->addLayout(controlBox);
        rtspLoyout->addWidget(rtsp_widget);
        camerasLayout->setContentsMargins(6, 6, 6, 6);
        setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
        this -> resize(sizeBefore);
        this -> move(posBefore);
        show();
    }
}


void MainWindow::onRtspStopped()
{
    if (btnPlay->isChecked())
    {
        tmrReconnect->start();
    }
}


MainWindow::~MainWindow()
{
}
