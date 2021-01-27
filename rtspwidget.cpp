#include "rtspwidget.h"

#include <QLabel>

RtspWidget::RtspWidget(QWidget *parent) : QLabel(parent)
{
    QLabel::setAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    QLabel::setWordWrap (true);
    QLabel::setStyleSheet("QLabel { background-color : black; color : white; }");
    QLabel::setText ("ready to start");
    QLabel::setMinimumSize(QSize(32,32));

    checkNewFrameTimer = new QTimer(this);
    connect(checkNewFrameTimer, &QTimer::timeout, this, &RtspWidget::onCheckNewFrameTimerTimeout, Qt::UniqueConnection);
    checkNewFrameTimer->setInterval(500);

}
