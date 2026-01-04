#pragma once

#include <QAudioFormat>
#include <QObject>

#include "EffectParameter.h"

class IAudioEffect : public QObject
{
        Q_OBJECT
    public:
        virtual bool applyEffect(char *data, qint64 len, const QAudioFormat &format) const = 0;

        virtual QString effectName() const = 0;
        virtual QList<QSharedPointer<EffectParameter>> parameters() const = 0;
    signals:
        void parametersChanged();
};

#define IAudioEffect_iid "CustomPlayer.AudioEffect"
Q_DECLARE_INTERFACE(IAudioEffect, IAudioEffect_iid)
