#ifndef MAINTCPSERVER_H
#define MAINTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include <QTimer>
#include "recthread.h"


class MainTCPServer : public QTcpServer
{
    Q_OBJECT
protected:
    static const int DELETE_TIMEOUT = 86400000;
    static const int DELETE_TIMEOUT_STEP = 10000;
    QList<sRecognitionModule*> m_ActiveDecorders;
    QTimer              m_GCTimer;

    cmd_ln_t *          m_DecoderConfig;
    sRecognitionModule* getAvailableDecoder();
protected:
    int                 m_Port;
public:
    explicit MainTCPServer(cmd_ln_t* config, int port);
    ~MainTCPServer();
    void startServer();
signals:

public slots:
protected slots:
    void updateActiveDecoders(bool forceCreation);
    void onTimeout();
protected:
    void incomingConnection(qintptr socketDescriptor);
};

#endif // MAINTCPSERVER_H
