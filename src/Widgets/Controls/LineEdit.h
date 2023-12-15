#pragma once

#include <QLineEdit>
#include <QKeyEvent>

namespace Controls
{

	class LineEdit : public QLineEdit {
		Q_OBJECT
		public:
			LineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}
		
		protected:
			void keyPressEvent(QKeyEvent *event) override {
				if (event->key() == Qt::Key_Escape && event->modifiers() == Qt::NoModifier) {
					this->clearFocus();
					return;
				}

				QLineEdit::keyPressEvent(event);
			}
			virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
	};

}
