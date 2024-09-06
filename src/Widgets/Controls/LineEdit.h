#pragma once

#include <QLineEdit>
#include <QKeyEvent>

#include "Core/EventHandler.h"

namespace Controls {
    class LineEdit : public QLineEdit {
        Q_OBJECT
        public:
            LineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

        signals:
            void onFocusIn();
            void onFocusOut();

        protected:
            void keyPressEvent(QKeyEvent *event) override;
            void focusInEvent(QFocusEvent *e) override;
            void focusOutEvent(QFocusEvent *e) override;
    };

}
