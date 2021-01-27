#pragma once

#include "rtspvideostreamdecoder.h"
#include "ui_mainwindow.h"

class QPushButton;
class QLineEdit;
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
    QPushButton * btnPlay = nullptr;

    void createCamera(const QString & rtsp);

private slots:
    void onPlayButtonToggled(bool checked);
    void onRtspStarted();
    void onRtspStopped();
};
