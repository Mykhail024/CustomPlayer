#include <QString>
#include <QFile>
#include <iostream>

namespace Convert
{
	namespace Seconds {
		static QString toMinutes(int seconds)
		{
			QString secondsString = (seconds % 60 < 10) ? QString("0%1").arg(seconds % 60)
														: QString::number(seconds % 60);

			return QString("%1:%2").arg(seconds / 60).arg(secondsString);
		}

	}
	namespace File {
		static QString toQString(QString filePath)
		{
			QFile file(filePath);
			if(file.open(QFile::ReadOnly | QFile::Text))
			{
				return QString::fromUtf8(file.readAll());
			}
			else {
				std::cerr << "Error reading: " << filePath.toStdString() << std::endl;
				return "";
			}
		}
	}
}
