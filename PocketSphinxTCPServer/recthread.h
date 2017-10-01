#ifndef RECTHREAD_H
#define RECTHREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <pocketsphinx.h>

struct sRecognitionModule{
    bool inUse;
    int deleteCounter;
    ps_decoder_t* decoder;
};

class RecThread : public QThread
{
    Q_OBJECT
protected:
    enum ePackageState{
        WAIT_HEADER,
        WAIT_SIZE,
        WAIT_DATA
    };

    enum ePackageType{
        STREAM,
        DICTIONARY
    };

    struct sVariant{
        QString value;
        int32 score;
    };

    static const unsigned int MAX_DATA_SIZE = 4194304;
    static const unsigned int MAX_VARIANTS_COUNT = 10;

    static const QByteArray PACKAGE_IN_STREAM_ID;
    static const QByteArray PACKAGE_IN_DIC_ID;
    static const QByteArray PACKAGE_OUT_ID;

    ePackageType        m_Type;
    ePackageState       m_State;
    unsigned int        m_Size;
    QByteArray          m_Data;

    bool                m_CustomDicLoaded;
    QString             m_DefaultDicFileName;
    void processData(const QByteArray& data);
    void processDic(const QByteArray& data);
    bool processInput();

    void releaseModule();
protected:
    bool                m_InSpeech;
    bool                m_UttStarted;
public:
    explicit RecThread(qintptr ID, sRecognitionModule* module, QString defaultDicFileName, QObject *parent = 0);
    virtual ~RecThread();

    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

private:
    sRecognitionModule* m_Module;
    QTcpSocket*         m_Socket;
    qintptr             m_SocketDescriptor;


    void errorFinish();
};

#endif // RECTHREAD_H
