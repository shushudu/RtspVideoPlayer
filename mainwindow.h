#pragma once

#include "rtspvideostreamdecoder.h"

#include <QWidget>

class QPushButton;
class QLineEdit;
class RtspWidget;
class QHBoxLayout;
class QVBoxLayout;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QHBoxLayout * camerasLayout = nullptr;

    RtspWidget * rtsp_widget = nullptr;
    QLineEdit * leRtspUrl = nullptr;
    QPushButton * btnPlay = nullptr;
    QTimer * tmrReconnect = nullptr;
    QHBoxLayout * controlBox = nullptr;
    QVBoxLayout * rtspLoyout = nullptr;

    void createCamera(const QString & rtsp);

private slots:
    void onTmrReconnectTimeout();
    void onPlayButtonToggled(bool checked);
    void onRtspStarted();
    void onRtspStopped();
    void onRtspClicked();
};
