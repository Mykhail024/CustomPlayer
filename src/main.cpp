#include "AudioEffectManager.h"
#ifndef NDEBUG
#include "sanitize.h"
#endif

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QTranslator>

#include "Log.h"

int main(int argc, char *argv[])
{
#ifdef __linux__
    qputenv("QT_QPA_PLATFORMTHEME", "flatpak");
#endif

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("customplayer"));

    AudioEffectManager::instance()->loadPlugins();

    Q_INIT_RESOURCE(translations_translations);
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "CustomPlayer_" + QLocale(locale).name();
        if (translator.load(":/translations/" + baseName)) {
            QGuiApplication::installTranslator(&translator);
            break;
        } else {
            Log_Debug(QString("No translation for language: %1").arg(QLocale(locale).name()));
        }
    }

    QQuickStyle::setStyle(QStringLiteral("CustomPlayer"));

    const QUrl url(QStringLiteral("qrc:/CustomPlayer/Presentation/main.qml"));

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qrc:/Style/"));
    engine.addImportPath(QStringLiteral("qrc:/CustomPlayer/Presentation/"));

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
