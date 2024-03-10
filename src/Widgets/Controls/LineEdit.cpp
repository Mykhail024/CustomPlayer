
#include "LineEdit.h"

namespace Controls {
    void LineEdit::keyPressEvent(QKeyEvent *event) {
        if (event->key() == Qt::Key_Escape && event->modifiers() == Qt::NoModifier) {
            this->clearFocus();
            return;
        }

        QLineEdit::keyPressEvent(event);
    }

    void LineEdit::focusInEvent(QFocusEvent *e)
    {
        emit onFocusIn();
        eventHandler()->LineEditActivate();
        QLineEdit::focusInEvent(e);
    }

    void LineEdit::focusOutEvent(QFocusEvent *e)
    {
        emit onFocusOut();
        eventHandler()->LineEditFinish();
        QLineEdit::focusOutEvent(e);
    }
}
