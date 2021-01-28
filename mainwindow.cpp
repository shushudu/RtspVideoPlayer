#include "mainwindow.h"

#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

#include "logger.h"
#include "rtspwidget.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    createCamera("rtsp://:8554/test");
//    createCamera("rtsp://admin:admin@192.168.7.71:80/ch0_0.264");

    return;
}


void MainWindow::createCamera(const QString & rtsp_url)
{
    tmrReconnect = new QTimer(this);
    tmrReconnect->setInterval(2000);
    tmrReconnect->setSingleShot(true);


    rtsp_widget = new RtspWidget(this);
    leRtspUrl = new QLineEdit(this);
    leRtspUrl->setText(rtsp_url);

    btnPlay = new QPushButton("Play", this);
    btnPlay->setCheckable(true);

    QVBoxLayout * vl = new QVBoxLayout();

    QHBoxLayout * hl = new QHBoxLayout();


    hl->addWidget(leRtspUrl);
    hl->addWidget(btnPlay);

    vl->addLayout(hl);
    vl->addWidget(rtsp_widget);

    bool connected = false;
    connected = connect (rtsp_widget, &RtspWidget::started, this, &MainWindow::onRtspStarted, Qt::UniqueConnection);
    assert(connected);
    connected = connect (rtsp_widget, &RtspWidget::stopped, this, &MainWindow::onRtspStopped, Qt::UniqueConnection);
    assert(connected);
    connected = connect (btnPlay, &QPushButton::toggled, this, &MainWindow::onPlayButtonToggled, Qt::UniqueConnection);
    assert(connected);
    connected = connect (tmrReconnect, &QTimer::timeout, this, &MainWindow::onTmrReconnectTimeout, Qt::UniqueConnection);
    assert(connected);

    rtsp_widget->setUpdateInterval(50);

    ui->camerasLayout->addLayout(vl);

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

void MainWindow::onRtspStopped()
{
    if (btnPlay->isChecked())
    {
        tmrReconnect->start();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

