#include "maintcpserver.h"
#include <QMutableListIterator>

void MainTCPServer::updateActiveDecoders()
{
    bool activeDecoderAvailable = false;
    int i = 0;
    while (i<m_ActiveDecorders.size()) {
        sRecognitionModule* mod = m_ActiveDecorders.at(i);
        if (!mod->inUse){
            mod->deleteCounter+=DELETE_TIMEOUT_STEP;
            if (activeDecoderAvailable && mod->deleteCounter>=DELETE_TIMEOUT){
                ps_free(mod->decoder);
                delete mod;
                m_ActiveDecorders.removeAt(i);
            }
            else{
                activeDecoderAvailable = true;
                ++i;
            }
        }
        else
            ++i;
    }

    if (!activeDecoderAvailable){
        qDebug() << "creating new decoder";
        sRecognitionModule* mod = new sRecognitionModule;
        mod->inUse = false;
        mod->deleteCounter = 0;
        mod->decoder = ps_init(m_DecoderConfig);
        m_ActiveDecorders.append(mod);
        qDebug() << "decoder ready";
    }
}

sRecognitionModule *MainTCPServer::getAvailableDecoder()
{
    for (int i = 0; i<m_ActiveDecorders.size(); i++){
        sRecognitionModule* mod = m_ActiveDecorders.at(i);
        if (!mod->inUse){
            return mod;
        }
    }
    updateActiveDecoders();
    return getAvailableDecoder();
}

MainTCPServer::MainTCPServer(cmd_ln_t *config, int port)
    : QTcpServer(NULL)
    , m_DecoderConfig(config)
    , m_Port(port)
{
    updateActiveDecoders();

    connect(&m_GCTimer,SIGNAL(timeout()),this,SLOT(updateActiveDecoders()));
    m_GCTimer.start(DELETE_TIMEOUT_STEP);
}

MainTCPServer::~MainTCPServer()
{
    for (int i = 0; i<m_ActiveDecorders.size(); ++i){
        ps_free(m_ActiveDecorders.at(i)->decoder);
        delete m_ActiveDecorders.at(i);
    }
}

void MainTCPServer::startServer()
{
    listen(QHostAddress::Any, m_Port);
}

void MainTCPServer::incomingConnection(qintptr socketDescriptor)
{
    sRecognitionModule* mod = getAvailableDecoder();
    mod->inUse = true;
    mod->deleteCounter = 0;

    RecThread *thread = new RecThread(socketDescriptor, mod, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}
