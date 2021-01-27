#pragma once

#include "rtspvideostreamdecoder.h"
#include "ui_mainwindow.h"

class QPushButton;

#include <QLabel>
#include <QMutex>
#include <QTimer>

class RtspWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;


    RtspWidget * rtsp_widget = nullptr;
    QLineEdit * leRtspUrl = nullptr;
    QPushButton * startButton = nullptr;
    QPushButton * stopButton = nullptr;

    void createCamera(const QString & rtsp);

private slots:
    void onStartButtonClicked();
    void onStopButtonClicked();
    void onRtspStarted();
    void onRtspStopped();
};
