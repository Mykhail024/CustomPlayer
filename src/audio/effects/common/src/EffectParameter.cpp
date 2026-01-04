#include "EffectParameter.h"

EffectParameter::EffectParameter(QString name, QVariant value, QVariant min, QVariant max, ParamType type, QObject *parent)
    : QObject(parent)
    , m_name(std::move(name))
    , m_value(std::move(value))
    , m_min(std::move(min))
    , m_max(std::move(max))
    , m_default(m_value)
    , m_type(type)
{
}

QString EffectParameter::name() const { return m_name; }
QVariant EffectParameter::value() const { return m_value; }
QVariant EffectParameter::min() const { return m_min; }
QVariant EffectParameter::max() const { return m_max; }
QVariant EffectParameter::defaultValue() const { return m_default; }
EffectParameter::ParamType EffectParameter::type() const { return m_type; }

void EffectParameter::setValue(const QVariant &v)
{
    if (v != m_value) {
        m_value = v;
        emit valueChanged();
    }
}
