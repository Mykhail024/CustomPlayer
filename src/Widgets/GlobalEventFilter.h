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
						eventHandler()->PrevPlaylist();
						return true;
					case Qt::Key_Right:
						eventHandler()->NextPlaylist();
						return true;
				}
			}
			else if(keyEvent->modifiers() == Qt::ControlModifier)
			{
				switch (keyEvent->key())
				{
					case Qt::Key_Left:
						eventHandler()->Backward();
						return true;
					case Qt::Key_Right:
						eventHandler()->Forward();
						return true;
					case Qt::Key_M:
						eventHandler()->VolumeMuteUnmute();
						return true;
					case Qt::Key_F:
						eventHandler()->FindActivate();
						return true;
					case Qt::Key_R:
						eventHandler()->LoopStateEnableDisable();
						return true;
					case Qt::Key_S:
						eventHandler()->ShuffleStateEnableDisable();
						return true;
				}
			}
			else if(keyEvent->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
			{
				switch (keyEvent->key()) {
					case Qt::Key_Left:
						eventHandler()->VolumeDown();
						return true;
					case Qt::Key_Right:
						eventHandler()->VolumeUp();
						return true;
				}
			}
			else if(keyEvent->modifiers() == (Qt::ControlModifier | Qt::AltModifier))
			{
				switch (keyEvent->key()) {
					case Qt::Key_Left:
						eventHandler()->PrevSong();
						return true;
					case Qt::Key_Right:
						eventHandler()->NextSong();
						return true;
				}
			}
			else if(keyEvent->modifiers() == Qt::NoModifier)
			{
				switch (keyEvent->key()) {
					case Qt::Key_Space:
						if(globals()->lineEditFocused() > 0) break;
						eventHandler()->PlayPause();
						return true;
				}
			}
		}
		return QObject::eventFilter(obj, event);
	}
};
