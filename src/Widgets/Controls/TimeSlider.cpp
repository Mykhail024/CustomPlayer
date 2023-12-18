#include <QMouseEvent>

#include "../../Core/EventHandler.h"
#include "../../Core/Globals.h"

#include "TimeSlider.h"

#include "moc_TimeSlider.cpp"

namespace Controls {

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
	void TimeSlider::wheelEvent(QWheelEvent *event)
	{
        int delta = event->angleDelta().y();
		if(event->modifiers() == Qt::ControlModifier)
		{
			eventHandler()->Seek(globals()->songPosition() + ((delta > 0) ? 1000 : -1000));
		}
		else
		{
			if(delta > 0)
			{
				eventHandler()->Forward();
			}
			else
			{
				eventHandler()->Backward();
			}
		}
    }
	void TimeSlider::keyPressEvent(QKeyEvent *event)
	{
		if(event->key() == Qt::Key_Left)
		{
			eventHandler()->Backward();
			return;
		}
		else if(event->key() == Qt::Key_Right)
		{
			eventHandler()->Forward();
			return;
		}

		QWidget::keyPressEvent(event);
	}

}
