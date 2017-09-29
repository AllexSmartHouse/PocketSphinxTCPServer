#include "recthread.h"
#include <QDataStream>

const QByteArray RecThread::PACKAGE_IN_ID("SNDR");
const QByteArray RecThread::PACKAGE_OUT_ID("BHYP");

RecThread::RecThread(qintptr ID, sRecognitionModule *module, QObject *parent)
    :QThread(parent)
    ,m_Module(module)
    ,m_SocketDescriptor(ID)
{
    ps_start_utt(m_Module->decoder);
    m_UttStarted = false;
    qDebug() << "recognition thread createdЫ";
}

RecThread::~RecThread()
{
    qDebug() << "recognition thread destroyed";
}

void RecThread::run()
{
    m_Socket = new QTcpSocket();

    if(!m_Socket->setSocketDescriptor(this->m_SocketDescriptor))
    {
        m_Module->inUse = false;
        emit error(m_Socket->error());
        return;
    }


    connect(m_Socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(m_Socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    m_State = WAIT_HEADER;



    exec();
}

void RecThread::readyRead()
{
    QByteArray newData = m_Socket->readAll();
    m_Data.append(newData);
     while (processInput())
         ;
}

void RecThread::disconnected()
{
    m_Module->inUse = false;
    m_Socket->deleteLater();
    exit(0);
}

void RecThread::processData(const QByteArray& data, bool final)
{
    QVector<sVariant> words;
    if (!final){
        ps_process_raw(m_Module->decoder, (const int16*)data.constData(), data.size() / 2, FALSE, FALSE);
        m_InSpeech = ps_get_in_speech(m_Module->decoder)!=0;
    }
    else{
        m_InSpeech = false;
    }

    if (m_InSpeech && !m_UttStarted) {
        m_UttStarted = true;
    }
    if (!m_InSpeech && m_UttStarted) {
        /* speech -> silence transition, time to start new utterance  */
        ps_end_utt(m_Module->decoder);

        ps_nbest_t* it = ps_nbest(m_Module->decoder);
        if (it){
            qDebug() << "recognized variants:";
        }
        for (unsigned int i = 0; i < MAX_VARIANTS_COUNT && it && (it = ps_nbest_next(it)); i++) {
            sVariant v;
            v.value = QString::fromUtf8(ps_nbest_hyp(it, &v.score));
            words.append(v);
            qDebug() << v.value;
        }
        if (it)
            ps_nbest_free(it);

        ps_start_utt(m_Module->decoder);
        m_UttStarted = false;
    }

    if (words.size()>0){
        QByteArray data;
        QDataStream ds(&data, QIODevice::ReadWrite);
        unsigned int s = words.size();
        ds.writeRawData((const char*)&s,sizeof(unsigned int));
        for (int i = 0; i<words.size(); ++i){
            QByteArray word = words[i].value.toUtf8();

            s = word.size();
            ds.writeRawData((const char*)&s,sizeof(unsigned int));
            ds.writeRawData(word.constData(),word.size());
            s = words[i].score;
            ds.writeRawData((const char*)&s,sizeof(unsigned int));
        }

        QByteArray package;
        QDataStream ps(&package, QIODevice::ReadWrite);
        ps.writeRawData(PACKAGE_OUT_ID.constData(),PACKAGE_OUT_ID.size());
        s = data.size();
        ps.writeRawData((const char*)&s,sizeof(unsigned int));
        ps.writeRawData(data.constData(),data.size());
        m_Socket->write(package);
    }
}

bool RecThread::processInput()
{
    switch(m_State){
    case WAIT_HEADER:{
        if (m_Data.size()>=PACKAGE_IN_ID.size()){
            if (m_Data.startsWith(PACKAGE_IN_ID)){
                m_Data.remove(0,PACKAGE_IN_ID.size());
                m_State = WAIT_SIZE;
                return true;
            }
            else{
                errorFinish();
                qCritical() << "RecThread incorrect package";
                qDebug() << m_Data;
            }
        }
    }break;
    case WAIT_SIZE:{
        if ((unsigned int)m_Data.size()>=sizeof(unsigned int)){
            QDataStream ds(m_Data);
            ds.readRawData((char*)&m_Size,sizeof(unsigned int));
            m_Data.remove(0,sizeof(unsigned int));
            if (m_Size<=MAX_DATA_SIZE){
                if (m_Size==0){
                    m_State = WAIT_HEADER;
                    processData(m_Data,true);
                    return false;
                }
                else{
                    m_State = WAIT_DATA;
                    return true;
                }
            }
            else{
                errorFinish();
                qCritical() << "RecThread too large package"<<m_Size;
            }
        }
    }break;
    case WAIT_DATA:{
        if ((unsigned int)m_Data.size()>=m_Size){
            processData(m_Data.mid(0,m_Size),false);
            m_Data.remove(0,m_Size);
            m_State = WAIT_HEADER;
            return true;
        }
    }break;
    }

    return false;
}

void RecThread::errorFinish()
{
    m_Module->inUse = false;
    m_Socket->disconnectFromHost();
    m_Socket->deleteLater();
    exit(0);
}
