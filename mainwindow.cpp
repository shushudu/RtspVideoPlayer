#include "mainwindow.h"

#include <QLineEdit>
#include <QPushButton>

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

    rtsp_widget->setUpdateInterval(50);

    ui->camerasLayout->addLayout(vl);

}

void MainWindow::onPlayButtonToggled(bool checked)
{
    if (checked)
    {
        rtsp_widget->start(leRtspUrl->text());
    }
    else
    {
        rtsp_widget->stop();
    }
}

void MainWindow::onRtspStarted()
{
    leRtspUrl->setDisabled(true);
}

void MainWindow::onRtspStopped()
{
    leRtspUrl->setDisabled(false);

    if (btnPlay->isChecked())
    {
        rtsp_widget->start(leRtspUrl->text());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

