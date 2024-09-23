#pragma once

#include <QWidget>
#include <QPainter>
#include <QStyleOption>
#include <QStyle>
#include <QPaintEvent>

#define DEFAULT_WIDGET_PAINT_EVENT \
    protected: \
    void paintEvent(QPaintEvent *pe) override { \
        QStyleOption o; \
        o.initFrom(this); \
        QPainter p(this); \
        style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this); \
    }
