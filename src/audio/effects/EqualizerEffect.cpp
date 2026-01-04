#include <vector>

#include "EffectParameter.h"
#include "IAudioEffect.h"

class EqualizerEffect : public IAudioEffect
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID IAudioEffect_iid FILE "EqualizerEffect.json")
        Q_INTERFACES(IAudioEffect)
    public:
        EqualizerEffect(QObject *parent = nullptr)
        {
            for (size_t i = 0; i < m_centerFrequencies.size(); ++i) {
                QString name = QString("%1 Hz").arg(m_centerFrequencies[i]);
                auto gain = QSharedPointer<EffectParameter>::create(name, 0.0, -12.0, 12.0, EffectParameter::Float, this);
                connect(gain.get(), &EffectParameter::valueChanged, this, &EqualizerEffect::onParameterChanged);
                m_bandGains.push_back(gain);
            }

            emit parametersChanged();
        }

        QString effectName() const override { return "16-Band Equalizer"; }

        bool applyEffect(char *data, qint64 len, const QAudioFormat &format) const override
        {
            const int channels = format.channelCount();
            const int sampleRate = format.sampleRate();

            if (m_lastSampleRate != sampleRate || m_coeffsDirty) {
                precalculateCoefficients(sampleRate);
                m_lastSampleRate = sampleRate;
                m_coeffsDirty = false;
            }

            // init states
            if (m_states.empty() || m_states[0].size() != static_cast<size_t>(channels)) {
                m_states.resize(m_bandGains.size(), std::vector<BiquadState>(channels));
            }

            if (format.sampleFormat() == QAudioFormat::Float) {
                float *samples = reinterpret_cast<float *>(data);
                const int totalSamples = len / sizeof(float);

                for (int i = 0; i < totalSamples; ++i) {
                    const int channel = i % channels;
                    float sample = samples[i];

                    // Sequential processing across all bands
                    for (qsizetype band = 0; band < m_bandGains.size(); ++band) {
                        const float gain = m_bandGains[band]->value().toFloat();
                        if (gain == 0.0)
                            continue;

                        // const float freq = m_centerFrequencies[band];

                        sample = processBiquad(m_coeffs[band], m_states[band][channel], sample);
                    }

                    samples[i] = sample;
                }
            } else {
                return false;
            }
            /**
             * @todo Realize process dor other formats
             */
            return true;
        }

        QList<QSharedPointer<EffectParameter>> parameters() const override { return m_bandGains; }

    private slots:
        void onParameterChanged() { m_coeffsDirty = true; }

    private:
        struct BiquadCoefficients
        {
                float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f;
                float a1 = 0.0f, a2 = 0.0f;
        };

        struct BiquadState
        {
                float x1 = 0.0f, x2 = 0.0f;
                float y1 = 0.0f, y2 = 0.0f;
        };

        const std::vector<float> m_centerFrequencies = {
            20.0f,    // 20 Hz
            40.0f,    // 40 Hz
            80.0f,    // 80 Hz
            160.0f,   // 160 Hz
            320.0f,   // 320 Hz
            640.0f,   // 640 Hz
            1300.0f,  // 1.3 kHz
            2500.0f,  // 2.5 kHz
            5000.0f,  // 5 kHz
            10000.0f, // 10 kHz
            12500.0f, // 12.5 kHz
            14000.0f, // 14 kHz
            16000.0f  // 16 kHz
        };

        QList<QSharedPointer<EffectParameter>> m_bandGains;

        const float Q = 1.0f;

        mutable bool m_coeffsDirty = true;
        mutable int m_lastSampleRate = -1;
        mutable std::vector<std::vector<BiquadState>> m_states;
        mutable std::vector<BiquadCoefficients> m_coeffs;

        void precalculateCoefficients(float sampleRate) const
        {
            m_coeffs.clear();
            for (qsizetype band = 0; band < m_bandGains.size(); ++band) {
                const float freq = m_centerFrequencies[band];
                m_coeffs.push_back(calculateCoefficients(m_bandGains[band]->value().toFloat(), freq, Q, sampleRate));
            }
        }

        BiquadCoefficients calculateCoefficients(float gainDB, float freq, float Q, float sampleRate) const
        {
            BiquadCoefficients coeff;

            const float A = powf(10.0f, gainDB / 40.0f);
            const float omega = 2.0f * M_PI * freq / sampleRate;
            const float alpha = sinf(omega) / (2.0f * Q);
            const float a0 = 1.0f + alpha / A;

            // Calculating coeff for peaking EQ
            coeff.b0 = (1.0f + alpha * A) / a0;
            coeff.b1 = (-2.0f * cosf(omega)) / a0;
            coeff.b2 = (1.0f - alpha * A) / a0;
            coeff.a1 = (-2.0f * cosf(omega)) / a0;
            coeff.a2 = (1.0f - alpha / A) / a0;

            return coeff;
        }
        float processBiquad(const BiquadCoefficients &coeff, BiquadState &state, float input) const
        {
            const float output =
                coeff.b0 * input + coeff.b1 * state.x1 + coeff.b2 * state.x2 - coeff.a1 * state.y1 - coeff.a2 * state.y2;

            // Update state
            state.x2 = state.x1;
            state.x1 = input;
            state.y2 = state.y1;
            state.y1 = output;

            return output;
        }
};

#include "EqualizerEffect.moc"
