#pragma once

#include <QSlider>
#include <QPropertyAnimation>

namespace Controls {

	class TimeSlider : public QSlider
	{
		Q_OBJECT
		public:
			TimeSlider(QWidget *parent = nullptr) : QSlider(parent){}

		public slots:
			void setValueDontMove(int value);

		protected:
			void mousePressEvent(QMouseEvent* event) override;
			void mouseMoveEvent(QMouseEvent* event) override;
			void mouseReleaseEvent(QMouseEvent* event) override;
			void keyPressEvent(QKeyEvent *event) override;
			void wheelEvent(QWheelEvent *event) override;

		private:
			bool move = false;
	};

}
