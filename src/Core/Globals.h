#pragma once

#include <QTimer>
#include <QObject>

namespace Audio {
    class AudioServer;
}
class History;

struct COLLUMNS
{
    bool Id;
    bool Title;
    bool Artist;
    bool Album;
    bool Length;
    bool ModifiedDate;
    bool Year;
};

enum PLAYBACK_STATE
{
    PLAYING,
    PAUSED,
    STOPPED
};

struct SONG_METADATA
{
    QString Title;
    QString Artist;
    QString Album;
    QString ArtUrl;
    unsigned int Length;
    unsigned long long ModifiedDate;
    unsigned int Year;
    QString Path;
    uint32_t SampleRate;
    unsigned int Channels;
    unsigned int BitRate;
};

class Globals : public QObject
{
    Q_OBJECT
    public:
        void initAudioServer();
        void deinitAudioServer();

        Globals(const Globals&) = delete;
        Globals& operator=(const Globals&) = delete;

    public slots:
        void startVolumeSaveTimer();
        void save();

        unsigned int historyCapacity() const;

        PLAYBACK_STATE playbackState() const;

        bool canNext() const;
        bool canPrev() const;
        bool canPlay() const;

        bool loopState() const;
        bool shuffleState() const;

        float volume() const;
        bool isMuted() const;

        COLLUMNS columns() const;

        bool softwareVolumeControl() const;

        int fadeIn_Primary() const;
        int fadeIn_Secondary() const;
        std::pair<int, int> fadeIn() const;

        unsigned int forwardTime() const;

        unsigned long int songPosition() const;
        SONG_METADATA currentSong() const;

        Audio::AudioServer* audioServer();
        History* history();
    private:
        friend class EventHandler;

        Audio::AudioServer *m_audioServer = nullptr;
        History *m_history = nullptr;

        unsigned int m_historyCapacity;
        unsigned int m_Forward_Backward_Time;
        PLAYBACK_STATE m_playbackState;
        bool m_loopState;
        bool m_shuffleState;
        bool m_isMuted;

        bool m_canNext;
        bool m_canPrev;
        bool m_canPlay;

        float m_volume;
        COLLUMNS m_columns;
        bool m_softwareVolumeControl;
        int m_fadeInTime_Primary;
        int m_fadeInTime_Secondary;

        unsigned long int m_songPosition;

        SONG_METADATA m_currentSong;

        QTimer *m_saveTimer;


        friend Globals& globals();
        Globals();
        ~Globals()
        {
            deinitAudioServer();
            delete m_saveTimer;
        }
};

Globals& globals();
