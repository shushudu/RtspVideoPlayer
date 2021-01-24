#pragma once

#include <QString>
#include <QDebug>

#define LOGFN_INFO(s) (qInfo() << QString("|INF|%1: %2").arg(__FUNCTION__).arg(s))
#define LOGFN_WARN(s) (qWarning() << QString("|WRN|%1: %2").arg(__FUNCTION__).arg(s))
#define LOGFN_ERROR(s) (qCritical() << QString("|ERR|%1: %2").arg(__FUNCTION__).arg(s))

