#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QtQml/qqmlregistration.h>

class EffectParameter : public QObject
{
        Q_OBJECT
        // QML_ELEMENT
        // QML_UNCREATABLE("Cannot create this type in QML")
        Q_PROPERTY(QString name READ name CONSTANT)
        Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
        Q_PROPERTY(QVariant min READ min CONSTANT)
        Q_PROPERTY(QVariant max READ max CONSTANT)
        Q_PROPERTY(QVariant defaultValue READ defaultValue CONSTANT)
        Q_PROPERTY(ParamType type READ type CONSTANT)
    public:
        enum ParamType {
            Float,
            Int,
            Bool,
            String
        };
        Q_ENUM(ParamType)

        EffectParameter(QString name, QVariant value, QVariant min, QVariant max, ParamType type, QObject *parent = nullptr);

        QString name() const;
        QVariant value() const;
        QVariant min() const;
        QVariant max() const;
        QVariant defaultValue() const;
        ParamType type() const;

        void setValue(const QVariant &v);

    signals:
        void valueChanged();

    private:
        QString m_name;
        QVariant m_value;
        const QVariant m_min, m_max, m_default;
        ParamType m_type;
};
