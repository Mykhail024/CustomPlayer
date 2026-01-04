#include <QAudioFormat>

#include "EffectParameter.h"
#include "IAudioEffect.h"

class GainEffect : public IAudioEffect
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID IAudioEffect_iid FILE "GainEffect.json")
        Q_INTERFACES(IAudioEffect)
    public:
        GainEffect(QObject *parent = nullptr)
        {
            m_gain = QSharedPointer<EffectParameter>::create("Gain", 1.0, 0.0, 5.0, EffectParameter::Float, this);
            emit parametersChanged();
        }

        QString effectName() const override { return "Gain"; }

        bool applyEffect(char *data, qint64 len, const QAudioFormat &format) const override
        {
            auto gainVal = m_gain->value().toFloat();

            if (gainVal == 1.0) {
                return true;
            }

            switch (format.sampleFormat()) {
            case QAudioFormat::Float: {
                int sampleCount = len / sizeof(float);
                float *samples = reinterpret_cast<float *>(data);
                for (int i = 0; i < sampleCount; ++i)
                    samples[i] *= gainVal;
                break;
            }
            case QAudioFormat::Int16: {
                int sampleCount = len / sizeof(qint16);
                qint16 *samples = reinterpret_cast<qint16 *>(data);
                for (int i = 0; i < sampleCount; ++i) {
                    int32_t scaled = static_cast<int32_t>(samples[i] * gainVal);
                    samples[i] = std::clamp(scaled, -32768, 32767);
                }
                break;
            }
            case QAudioFormat::Int32: {
                int sampleCount = len / sizeof(qint32);
                qint32 *samples = reinterpret_cast<qint32 *>(data);
                for (int i = 0; i < sampleCount; ++i) {
                    int64_t scaled = static_cast<int64_t>(samples[i] * gainVal);
                    samples[i] = std::clamp(scaled, int64_t(INT32_MIN), int64_t(INT32_MAX));
                }
                break;
            }
            default:
                return false;
            }

            return true;
        }

        QList<QSharedPointer<EffectParameter>> parameters() const override { return {m_gain}; }

    private:
        QSharedPointer<EffectParameter> m_gain;
};

#include "GainEffect.moc"
