#pragma once

#include <QSlider>

namespace Controls
{
	class Slider : public QSlider
	{
		public:
			Slider(Qt::Orientation orientation, QWidget *parent = nullptr) : QSlider(orientation, parent) {}

		protected:
			void keyPressEvent(QKeyEvent *event) override
			{
				if(event->key() == Qt::Key_Left)
				{
					this->setValue(this->value() - 5);
					return;
				}
				else if(event->key() == Qt::Key_Right)
				{
					this->setValue(this->value() + 5);
					return;
				}

				QWidget::keyPressEvent(event);
			}
			void wheelEvent(QWheelEvent *event) override
			{
				int delta = event->angleDelta().y();
				if(event->modifiers() == Qt::ControlModifier)
				{
					this->setValue(this->value() + ((delta > 0) ? 1 : -1));
				}
				else
				{
					if(delta > 0)
					{
						this->setValue(this->value() + 5);
					}
					else
					{
						this->setValue(this->value() - 5);
					}
				}
			}
	};
}
