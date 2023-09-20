#pragma once

#include <QSlider>
#include <QPropertyAnimation>

namespace Widgets {
	class TimeSlider : public QSlider{
		Q_OBJECT
		public:
			TimeSlider(QWidget* parent = nullptr);

		public slots:
			void onSetValueDontMove(int value);

		signals:
			void setValueDontMove(int value);

		protected:
			void mousePressEvent(QMouseEvent* event) override;
			void mouseMoveEvent(QMouseEvent* event) override;
			void mouseReleaseEvent(QMouseEvent* event) override;

		private:
			bool move = false;
	};
}
