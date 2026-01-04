#include <QCoreApplication>
#include <QDir>
#include <qqmllist.h>

#include "AudioEffectManager.h"
#include "EffectParameter.h"
#include "IAudioEffect.h"
#include "Log.h"
#include "config.h"

static qsizetype effectParametersCount(QQmlListProperty<EffectParameter> *list)
{
    IAudioEffect *effect = qobject_cast<IAudioEffect *>(list->object);
    if (!effect)
        return 0;
    return effect->parameters().size();
}

static EffectParameter *effectParameterAt(QQmlListProperty<EffectParameter> *list, qsizetype index)
{
    IAudioEffect *effect = qobject_cast<IAudioEffect *>(list->object);
    if (!effect)
        return nullptr;
    const auto &params = effect->parameters();
    if (index < 0 || index >= params.size())
        return nullptr;
    return params.at(index).data();
}

static QQmlListProperty<EffectParameter> makeEffectParametersProperty(IAudioEffect *effect)
{
    return QQmlListProperty<EffectParameter>(effect, nullptr, &effectParametersCount, &effectParameterAt);
}

QStringList AudioEffectManager::pluginSearchPaths()
{
    return QStringList({QStringLiteral(EFFECT_PLUGIN_INSTALL_DIR), QCoreApplication::applicationDirPath() + "/plugins/effects"});
}

void AudioEffectManager::loadPlugins(const QStringList &folders)
{
    beginResetModel();
    for (const auto &folder : std::as_const(folders)) {
        QDir dir(folder);

        const QStringList files = dir.entryList({"*.so*", "*.dll", "*.dylib"}, QDir::Files);
        for (const QString &file : files) {
            const auto filePath = dir.filePath(file);
            if (!loadPlugin(filePath)) {
                Log_Warning(QString("[AudioEffectManager]: failed to load effect plugin: %1").arg(filePath));
            }
        }
    }

    if (!m_effects.empty()) {
        QStringList effectsList;
        effectsList.reserve(m_effects.size());
        for (const auto &effect : std::as_const(m_effects)) {
            effectsList << effect.effect->effectName();
        }
        Log_Info(QString("[AudioEffectManager] Loaded effect plugins: %1").arg(effectsList.join(", ")));
    }
    endResetModel();
}

bool AudioEffectManager::loadPlugin(const QString &path)
{
    QSharedPointer<QPluginLoader> loader = QSharedPointer<QPluginLoader>::create(path);

    if (!loader->load()) {
        Log_Warning(QString("[AudioEffectManager]: Failed to load plugin: %1").arg(loader->errorString()));
        return false;
    }

    QObject *pluginInstance = loader->instance();
    if (!pluginInstance) {
        Log_Warning(QString("[AudioEffectManager]: Plugin has no instance: %1").arg(loader->errorString()));
        return false;
    }

    auto effect = qobject_cast<IAudioEffect *>(pluginInstance);
    if (!effect) {
        Log_Warning(QString("[AudioEffectManager]: Plugin does not implement IAudioEffect: %1").arg(path));
        loader->unload();
        return false;
    }

    QString pluginId = loader->metaData().value("MetaData").toObject().value("PluginId").toString();
    if (pluginId.isEmpty()) {
        Log_Warning("[AudioEffectManager]: Plugin has no PluginId in MetaData");
        return false;
    }

    for (const auto &existingEffect : std::as_const(m_effects)) {
        const QString existingPluginId = existingEffect.loader->metaData().value("MetaData").toObject().value("PluginId").toString();
        if (existingPluginId == pluginId) {
            Log_Info(QString("[AudioEffectManager]: Skipping duplicate plugin with PluginId: %1.\n\t Files: %2 and %3")
                         .arg(pluginId, path, existingEffect.loader->fileName()));
            loader->unload();
            return true; // Plugin already loaded
        }
    }

    AudioEffect effectStruct{.loader = loader, .effect = effect, .enabled = false};

    m_effects.append(effectStruct);
    return true;
}

void AudioEffectManager::applyAll(char *data, qint64 len, const QAudioFormat &format)
{
    for (const auto &es : std::as_const(m_effects))
        if (es.enabled)
            es.effect->applyEffect(data, len, format);
}

AudioEffectManager::AudioEffectManager(QObject *parent) : QAbstractListModel(parent) {}

int AudioEffectManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_effects.size();
}
QVariant AudioEffectManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_effects.size())
        return QVariant();

    const auto &eff = m_effects[index.row()];
    switch (role) {
    case EffectName:
        return eff.effect->effectName();
    case EffectEnabled:
        return eff.enabled;
    case EffectParameters:
        return QVariant::fromValue(makeEffectParametersProperty(eff.effect));
    }
    return QVariant();
}

bool AudioEffectManager::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_effects.size())
        return false;

    if (role == EffectEnabled) {
        return effectSetEnabled(index.row(), value.toBool());
    }
    return false;
}

bool AudioEffectManager::deleteEffect(qsizetype index)
{
    if (index < m_effects.size()) {
        m_effects.erase(m_effects.begin() + index);
        emit dataChanged(this->index(index), this->index(index), {EffectEnabled});
        return true;
    }

    return false;
}

bool AudioEffectManager::effectSetEnabled(qsizetype index, bool enabled)
{
    if (index >= m_effects.size())
        return false;

    auto &eff = m_effects[index];
    if (eff.enabled != enabled) {
        eff.enabled = enabled;
        emit dataChanged(this->index(index), this->index(index), {EffectEnabled});
        return true;
    }

    return false;
}

QHash<int, QByteArray> AudioEffectManager::roleNames() const
{
    return {
        {      EffectName,       "effectName"},
        {EffectParameters, "effectParameters"},
        {   EffectEnabled,    "effectEnabled"}
    };
}

AudioEffectManager *AudioEffectManager::instance()
{
    static AudioEffectManager instance(nullptr);
    return &instance;
}

AudioEffectManager *AudioEffectManager::create(QQmlEngine *engine, QJSEngine *)
{
    auto i = AudioEffectManager::instance();
    engine->setObjectOwnership(i, QQmlEngine::CppOwnership);
    return i;
}
