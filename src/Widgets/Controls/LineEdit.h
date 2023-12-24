#pragma once

#include <QLineEdit>
#include <QKeyEvent>

#include "Core/EventHandler.h"

namespace Controls
{

	class LineEdit : public QLineEdit {
		Q_OBJECT
		public:
			LineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

		signals:
			void onFocusIn();
			void onFocusOut();

		protected:
			void keyPressEvent(QKeyEvent *event) override {
				if (event->key() == Qt::Key_Escape && event->modifiers() == Qt::NoModifier) {
					this->clearFocus();
					return;
				}

				QLineEdit::keyPressEvent(event);
			}
			void focusInEvent(QFocusEvent *e) override
			{
				emit onFocusIn();
				eventHandler()->LineEditActivate();
				QLineEdit::focusInEvent(e);
			}
			void focusOutEvent(QFocusEvent *e) override
			{
				emit onFocusOut();
				eventHandler()->LineEditFinish();
				QLineEdit::focusOutEvent(e);
			}
	};

}
