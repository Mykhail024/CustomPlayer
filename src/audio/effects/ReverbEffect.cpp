#include <vector>

#include "EffectParameter.h"
#include "IAudioEffect.h"

class ReverbEffect : public IAudioEffect
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID IAudioEffect_iid FILE "ReverbEffect.json")
        Q_INTERFACES(IAudioEffect)
    public:
        ReverbEffect(QObject *parent = nullptr)
        {
            m_decayTime = QSharedPointer<EffectParameter>::create("Decay", 2.0, 0.1, 5.0, EffectParameter::Float, this);
            m_damping = QSharedPointer<EffectParameter>::create("Damping", 0.5, 0.0, 1.0, EffectParameter::Float, this);
            m_wetLevel = QSharedPointer<EffectParameter>::create("Wet", 0.3, 0.0, 1.0, EffectParameter::Float, this);
            m_roomSize = QSharedPointer<EffectParameter>::create("Room", 0.7, 0.0, 1.0, EffectParameter::Float, this);

            emit parametersChanged();
        }

        QString effectName() const override { return "Reverb"; }

        bool applyEffect(char *data, qint64 len, const QAudioFormat &format) const override
        {
            // const int channels = format.channelCount();
            const float sampleRate = format.sampleRate();

            if (m_combs.empty() || m_lastSampleRate != sampleRate) {
                updateFilters(sampleRate);
                m_lastSampleRate = sampleRate;
            }

            const float decay = m_decayTime->value().toFloat();
            const float damp = m_damping->value().toFloat();
            const float wet = m_wetLevel->value().toFloat();
            const float room = m_roomSize->value().toFloat();

            for (auto &comb : m_combs) {
                comb.feedback = powf(10.0f, -3.0f * comb.buffer.size() / (decay * sampleRate));
                comb.damp = damp;
            }

            if (format.sampleFormat() == QAudioFormat::Float) {
                float *samples = reinterpret_cast<float *>(data);
                const int totalSamples = len / sizeof(float);

                for (int i = 0; i < totalSamples; ++i) {
                    // const int channel = i % channels;
                    float input = samples[i];

                    // Обробка комб-фільтрами
                    float combSum = 0.0f;
                    for (auto &comb : m_combs) {
                        combSum += processComb(comb, input);
                    }
                    combSum *= 0.1f; // Нормалізація

                    // Обробка алпас-фільтрами
                    float allpassOut = combSum;
                    for (auto &allpass : m_allpasses) {
                        allpassOut = processAllpass(allpass, allpassOut);
                    }

                    // Змішування "мокрого" та "сухого" сигналу
                    samples[i] = input * (1.0f - wet) + allpassOut * wet * room;
                }
            } else {
                return false;
            }
            return true;
        }

        QList<QSharedPointer<EffectParameter>> parameters() const override
        {
            return {m_decayTime, m_damping, m_wetLevel, m_roomSize};
        }

    private:
        QSharedPointer<EffectParameter> m_decayTime; // Час загасання (в секундах)
        QSharedPointer<EffectParameter> m_damping;   // Загасання високих частот (0.0...1.0)
        QSharedPointer<EffectParameter> m_wetLevel;  // Рівень "мокрого" сигналу (0.0...1.0)
        QSharedPointer<EffectParameter> m_roomSize;  // Розмір кімнати (0.0...1.0)

        struct CombFilter
        {
                std::vector<float> buffer;
                size_t bufferIndex = 0;
                float feedback = 0.0f;
                float damp = 0.0f;
        };
        mutable std::vector<CombFilter> m_combs;

        struct AllpassFilter
        {
                std::vector<float> buffer;
                size_t bufferIndex = 0;
        };
        mutable std::vector<AllpassFilter> m_allpasses;

        void updateFilters(float sampleRate) const
        {
            const size_t combDelaysMs[] = {50, 56, 61, 68, 72, 78, 84, 90};
            const size_t combDelaysMs_size = sizeof(combDelaysMs) / sizeof(combDelaysMs[0]);
            m_combs.resize(combDelaysMs_size);
            for (size_t i = 0; i < combDelaysMs_size; ++i) {
                size_t delaySamples = static_cast<size_t>(combDelaysMs[i] * sampleRate / 1000.0f);
                m_combs[i].buffer.resize(delaySamples, 0.0f);
                m_combs[i].bufferIndex = 0;
            }

            const size_t allpassDelaysMs[] = {10, 25};
            const size_t allpassDelaysMs_size = sizeof(allpassDelaysMs) / sizeof(allpassDelaysMs[0]);
            m_allpasses.resize(allpassDelaysMs_size);
            for (size_t i = 0; i < allpassDelaysMs_size; ++i) {
                size_t delaySamples = static_cast<size_t>(allpassDelaysMs[i] * sampleRate / 1000.0f);
                m_allpasses[i].buffer.resize(delaySamples, 0.0f);
                m_allpasses[i].bufferIndex = 0;
            }
        }
        float processComb(CombFilter &filter, float input) const
        {
            float output = filter.buffer[filter.bufferIndex];
            output += input * (1.0f - filter.damp); // Загасання високих частот
            filter.buffer[filter.bufferIndex] = input + output * filter.feedback;
            filter.bufferIndex = (filter.bufferIndex + 1) % filter.buffer.size();
            return output;
        }
        float processAllpass(AllpassFilter &filter, float input) const
        {
            float bufferOut = filter.buffer[filter.bufferIndex];
            float output = -input + bufferOut;
            filter.buffer[filter.bufferIndex] = input + bufferOut * 0.5f; // Фіксований коефіцієнт
            filter.bufferIndex = (filter.bufferIndex + 1) % filter.buffer.size();
            return output;
        }

        mutable int m_lastSampleRate = -1;
};

#include "ReverbEffect.moc"
