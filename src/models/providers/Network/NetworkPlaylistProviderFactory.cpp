#include <QFileInfo>
#include <QObject>
#include <QQmlContext>
#include <QStandardPaths>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>

#include "IPlaylistProviderFactory.h"
#include "IPlaylistProviderInterface.h"
#include "Log.h"
#include "NetworkBroadcaster.h"
#include "NetworkDiscoverer.h"
#include "NetworkPlaylistProvider.h"

const QString PLAYLISTS_DIR = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/CustomPlayer/Playlists/";

class NetworkPlaylistProviderFactory : public IPlaylistProviderFactory, public IPlaylistProviderInterface
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID IPlaylistProviderFactory_iid FILE "NetworkPlaylistProvider.json")
        Q_INTERFACES(IPlaylistProviderFactory IPlaylistProviderInterface)
        Q_PROPERTY(bool isRequirementsProvided READ isRequirementsProvided NOTIFY isRequirementsProvidedChanged)
        Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)
        Q_PROPERTY(NetworkDiscoverer *networkDiscovery READ networkDiscovery CONSTANT)
        Q_PROPERTY(QString mode READ mode WRITE setMode NOTIFY modeChanged)
        Q_PROPERTY(QStringList modes READ modes CONSTANT)
        Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

    public:
        NetworkPlaylistProviderFactory(QObject *parent = nullptr)
        {
            connect(this, &NetworkPlaylistProviderFactory::modeChanged, &NetworkPlaylistProviderFactory::onModeChanged);
        }
        IPlaylistProvider *create(QObject *parent = nullptr) const override
        {
            NetworkPlaylistProvider::Mode mode =
                (m_mode == "Create") ? NetworkPlaylistProvider::Create : NetworkPlaylistProvider::Connect;

            NetworkPlaylistProvider *provider = new NetworkPlaylistProvider(mode, m_password, m_ipAddress, parent);

            // Створюємо шлях до файлу
            QString fileName;
            if (mode == NetworkPlaylistProvider::Create) {
                fileName = QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".l.npl";
                provider->setPlaylistName("New Network Playlist");
            } else {
                fileName = QFileInfo(m_ipAddress).fileName() + ".r.npl";
            }

            QString filePath = PLAYLISTS_DIR + fileName;
            provider->setUri(filePath);
            provider->save();

            return provider;
        }
        Q_INVOKABLE QObject *providerUi(QObject *qmlContextParent) override
        {
            QQmlEngine *engine = qmlEngine(qmlContextParent);
            if (!engine)
                return nullptr;

            if (!m_component || m_component->isNull()) {
                m_component.reset(new QQmlComponent(engine, QUrl("qrc:/NetworkPlaylistProvider/NetworkPlaylistSetup.qml")));
                if (m_component->isError()) {
                    Log_Error(m_component->errors());
                    return nullptr;
                }
            }

            QQmlContext *context = new QQmlContext(engine->rootContext());
            context->setContextProperty("factory", this);

            QObject *ui = m_component->create(context);
            ui->setParent(qmlContextParent);

            m_n.startBroadcasting({"remote"});

            return ui;
        }

        bool isRequirementsProvided() const override { return m_isRequirementsProvided; }

        QString ipAddress() const { return m_ipAddress; }
        NetworkDiscoverer *networkDiscovery() { return &m_netDiscovery; }

        QString mode() const { return m_mode; }

        constexpr QStringList modes() const { return {"Create", "Connect"}; }
        QString password() const { return m_password; }

    public slots:
        void setIpAddress(const QString &ip)
        {
            if (ip != m_ipAddress) {
                m_ipAddress = ip;
                updateRequirements();
                emit ipAddressChanged(m_ipAddress);
            }
        }
        void setMode(const QString &mode)
        {
            if (mode != m_mode) {
                m_mode = mode;
                updateRequirements();
                emit modeChanged(mode);
            }
        }
        void onModeChanged(const QString &mode)
        {
            if (mode == "Create") {
                m_netDiscovery.stopDiscovering();
            } else [[likely]] {
                m_netDiscovery.startDiscovering();
            }
            Log_Debug(QString("Mode: %1").arg(m_mode));
        }
        void setPassword(const QString &password)
        {
            if (m_password != password) {
                m_password = password;
                updateRequirements();
                emit passwordChanged(m_password);
                Log_Debug(m_password);
            }
        }

    signals:
        void isRequirementsProvidedChanged(bool val);
        void ipAddressChanged(const QString &ip);
        void modeChanged(const QString &mode);
        void passwordChanged(const QString &password);

    private:
        bool updateRequirements()
        {
            const bool old = m_isRequirementsProvided;
            m_isRequirementsProvided = (!m_ipAddress.isEmpty() || m_mode == "Create") && !m_password.isEmpty();

            if (old != m_isRequirementsProvided) {
                emit isRequirementsProvidedChanged(m_isRequirementsProvided);
            }
            return m_isRequirementsProvided;
        }

        QString m_password;
        QString m_mode = "Create";
        QScopedPointer<QQmlComponent> m_component;
        QString m_ipAddress;
        bool m_isRequirementsProvided = false;
        NetworkDiscoverer m_netDiscovery;
        NetworkBroadcaster m_n;
};

#include "NetworkPlaylistProviderFactory.moc"
