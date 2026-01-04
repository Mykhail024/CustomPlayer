#include <cmath>
#include <vector>

#include "EffectParameter.h"
#include "IAudioEffect.h"

class BassBoostEffect : public IAudioEffect
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID IAudioEffect_iid FILE "BassBoostEffect.json")
        Q_INTERFACES(IAudioEffect)
    public:
        BassBoostEffect(QObject *parent = nullptr)
        {
            m_boost = QSharedPointer<EffectParameter>::create("Boost", 1.0, 0.0, 5.0, EffectParameter::Float, this);
            m_cutoff = QSharedPointer<EffectParameter>::create("CutOffHz", 200.0, 20.0, 1000.0, EffectParameter::Float, this);

            emit parametersChanged();
        }

        QString effectName() const override { return "Bass Boost"; }

        bool applyEffect(char *data, qint64 len, const QAudioFormat &format) const override
        {
            const float boostVal = m_boost->value().toFloat();

            if (boostVal <= 0) {
                return true;
            }

            const float cutoffHz = m_cutoff->value().toFloat();
            const int channels = format.channelCount();
            const int sampleRate = format.sampleRate();

            // α = exp(-2π * fc / fs)
            const float alpha = std::exp(-2.0f * M_PI * cutoffHz / sampleRate);
            const float oneMinusAlpha = 1.0f - alpha;

            if (m_state.size() != static_cast<size_t>(channels))
                m_state.assign(channels, 0.0f);

            if (format.sampleFormat() == QAudioFormat::Float) {
                float *samples = reinterpret_cast<float *>(data);
                const int sampleCount = len / sizeof(float);

                for (int i = 0; i < sampleCount; ++i) {
                    int ch = i % channels;
                    float x = samples[i];

                    float low = alpha * m_state[ch] + oneMinusAlpha * x;
                    m_state[ch] = low;

                    samples[i] = x + boostVal * low;
                }
            } else if (format.sampleFormat() == QAudioFormat::Int16) {
                qint16 *samples = reinterpret_cast<qint16 *>(data);
                const int sampleCount = len / sizeof(qint16);

                for (int i = 0; i < sampleCount; ++i) {
                    int ch = i % channels;
                    float x = samples[i] / 32768.0f;

                    float low = alpha * m_state[ch] + oneMinusAlpha * x;
                    m_state[ch] = low;

                    float y = x + boostVal * low;

                    int scaled = static_cast<int>(y * 32768);
                    samples[i] = qBound(-32768, scaled, 32767);
                }
            } else if (format.sampleFormat() == QAudioFormat::Int32) {
                qint32 *samples = reinterpret_cast<qint32 *>(data);
                const int sampleCount = len / sizeof(qint32);

                for (int i = 0; i < sampleCount; ++i) {
                    int ch = i % channels;
                    float x = samples[i] / 2147483648.0f;

                    float low = alpha * m_state[ch] + oneMinusAlpha * x;
                    m_state[ch] = low;

                    float y = x + boostVal * low;
                    qint32 scaled = static_cast<qint32>(y * 2147483648.0f);
                    samples[i] = qBound<qint32>(INT32_MIN, scaled, INT32_MAX);
                }
            } else {
                return false;
            }
            return true;
        }

        QList<QSharedPointer<EffectParameter>> parameters() const override { return {m_boost, m_cutoff}; }

    private:
        QSharedPointer<EffectParameter> m_boost;
        QSharedPointer<EffectParameter> m_cutoff;
        mutable std::vector<float> m_state;
};

#include "BassBoostEffect.moc"
