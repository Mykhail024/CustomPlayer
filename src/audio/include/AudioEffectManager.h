#pragma once

#include <QAbstractListModel>
#include <QAudioFormat>
#include <QList>
#include <QObject>
#include <QPluginLoader>
#include <QQmlEngine>
#include <QString>
#include <QtQml/qqmlregistration.h>

class IAudioEffect;

class AudioEffectManager : public QAbstractListModel
{
        Q_OBJECT
        QML_ELEMENT
        QML_SINGLETON
    public:
        static AudioEffectManager *instance();
        static AudioEffectManager *create(QQmlEngine *, QJSEngine *);

        void loadPlugins(const QStringList &folders = pluginSearchPaths());
        bool loadPlugin(const QString &path);
        bool deleteEffect(qsizetype index);
        void applyAll(char *data, qint64 len, const QAudioFormat &format);
        bool effectSetEnabled(qsizetype index, bool enabled);

        static QStringList pluginSearchPaths();

        Q_INVOKABLE int rowCount(const QModelIndex & = QModelIndex()) const override;
        Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const override;
        Q_INVOKABLE bool setData(const QModelIndex &index, const QVariant &value, int role) override;
        Q_INVOKABLE QHash<int, QByteArray> roleNames() const override;

        enum Roles {
            EffectName = Qt::UserRole + 1,
            EffectEnabled,
            EffectParameters
        };
        Q_ENUM(Roles)

    private:
        explicit AudioEffectManager(QObject *parent = nullptr);
        struct AudioEffect
        {
                QSharedPointer<QPluginLoader> loader;
                QPointer<IAudioEffect> effect;
                bool enabled = false;
        };

        QList<AudioEffect> m_effects;
};
