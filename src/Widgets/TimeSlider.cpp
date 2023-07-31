#include <QMouseEvent>

#include "TimeSlider.hpp"

namespace Widgets {
	void TimeSlider::mousePressEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton)
		{
			move = true;
			setValue(minimum() + ((maximum() - minimum()) * event->position().x()) / width());
			event->accept();
		}
		else
		{
			QSlider::mousePressEvent(event);
		}
	}

	void TimeSlider::mouseMoveEvent(QMouseEvent* event)
	{
		if (event->buttons() & Qt::LeftButton)
		{
			setValue(minimum() + ((maximum() - minimum()) * event->position().x()) / width());
			event->accept();
		}
		else
		{
			QSlider::mouseMoveEvent(event);
		}
	}
	void TimeSlider::mouseReleaseEvent(QMouseEvent* event) {
		if (event->button() == Qt::LeftButton) {
			move = false;
			emit sliderReleased();
			event->accept();
		} else {
			QSlider::mouseReleaseEvent(event);
		}
	}
	void TimeSlider::setValueDontMove(int value) {
		if(!move)
		{
			setValue(value);
		}
	}
}
