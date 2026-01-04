#include <QTime>

#include "Utils.h"

QString Utils::msecToDuration(const int &durationInMsec)
{
    QTime time = QTime::fromMSecsSinceStartOfDay(durationInMsec);
    if (time.hour() > 0)
        return time.toString("h:mm:ss");
    return time.toString("mm:ss");
}
