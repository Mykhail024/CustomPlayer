#pragma once

#include <QWidget>
#include <QKeyEvent>

#include "../Core/EventHandler.h"
#include "../Core/Globals.h"

class GlobalEventFilter : public QObject {
    Q_OBJECT
public:
    explicit GlobalEventFilter(QObject *parent = nullptr) : QObject(parent) {}

protected:
	bool eventFilter(QObject *obj, QEvent *event) override {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
			if(keyEvent->modifiers() == Qt::ShiftModifier)
			{
				switch (keyEvent->key())
				{
					case Qt::Key_Left:
						eventHandler()->emitPrevPlaylist();
						return true;
					case Qt::Key_Right:
						eventHandler()->emitNextPlaylist();
						return true;
				}
			}
			else if(keyEvent->modifiers() == Qt::ControlModifier)
			{
				switch (keyEvent->key())
				{
					case Qt::Key_Left:
						eventHandler()->emitBackward();
						return true;
					case Qt::Key_Right:
						eventHandler()->emitForward();
						return true;
					case Qt::Key_M:
						eventHandler()->emitVolumeMuteUnmute();
						return true;
					case Qt::Key_F:
						eventHandler()->emitFindActivate();
						return true;
					case Qt::Key_R:
						eventHandler()->emitLoopStateEnableDisable();
						return true;
					case Qt::Key_S:
						eventHandler()->emitShuffleStateEnableDisable();
						return true;
				}
			}
			else if(keyEvent->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
			{
				switch (keyEvent->key()) {
					case Qt::Key_Left:
						eventHandler()->emitVolumeDown();
						return true;
					case Qt::Key_Right:
						eventHandler()->emitVolumeUp();
						return true;
				}
			}
			else if(keyEvent->modifiers() == (Qt::ControlModifier | Qt::AltModifier))
			{
				switch (keyEvent->key()) {
					case Qt::Key_Left:
						eventHandler()->emitPrevSong();
						return true;
					case Qt::Key_Right:
						eventHandler()->emitNextSong();
						return true;
				}
			}
			else if(keyEvent->modifiers() == Qt::NoModifier)
			{
				switch (keyEvent->key()) {
					case Qt::Key_Space:
						if(globals()->findIsFocused()) break;
						eventHandler()->emitPlayPause();
						return true;
				}
			}
		}
		return QObject::eventFilter(obj, event);
	}
};
