/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-13
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file SocketConnect.cpp
 * @brief socket connection
 */
#include"SocketConnect.h"

SocketConnect::SocketConnect(QObject *parent): QObject(parent) {
    m_numWaveParams = 0;
}

void SocketConnect::init(unsigned short port, size_t numWaveParams){
    m_Socket = nullptr;
    m_Server = nullptr;
    m_Server = new QTcpServer();

    if(!m_Server->listen(QHostAddress::Any, port)){
        delete m_Server;
        throw std::runtime_error("[Socket Connect]: The listening status is abnormal\n");
    }

    m_numWaveParams = numWaveParams;
    m_socketToPhawd = (phawd::SocketToPhawd *) malloc(sizeof(phawd::SocketToPhawd) + numWaveParams * sizeof(phawd::Parameter));
    connect(m_Server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void SocketConnect::sendData(void *data, size_t sendSize){
    if(m_Socket == nullptr) {
        throw std::runtime_error("[Socket Connect]: No clients connected, send data failed\n");
    }
    char *sendData = new char[sendSize];
    memcpy(sendData, data, sendSize);
    qint64 count = m_Socket->write(sendData, sendSize);
    if (count <= 0){
        throw std::runtime_error("[Socket Connect]: Write error: read on closed socket or no data for reading");
    }
    m_Socket->flush();
    delete []sendData;
}

void SocketConnect::slotNewConnection(){
    m_Socket = m_Server->nextPendingConnection();
    connect(m_Socket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_Socket, SIGNAL(disconnected()) ,this, SLOT(closeSocket()));
    emit connected(true);
}

void SocketConnect::readData() {
    size_t readSize = sizeof(phawd::SocketToPhawd) + m_numWaveParams * sizeof(phawd::Parameter);
    char *recvData = new char[readSize];
    qint64 count = m_Socket->read(recvData, readSize);

    if (count <= 0){
        return;
//        throw std::runtime_error("[Socket Connect]: Read error: read on closed socket or no data for reading");
    }
    memcpy(m_socketToPhawd, recvData, readSize);

    delete []recvData;
    recvData = nullptr;
    if(m_numWaveParams > 0){
        emit readReady();
    }
}

void SocketConnect::close(){
    if(m_Server != nullptr){
        disconnect(m_Server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
        delete m_Server;
        m_Server = nullptr;
    }

    if(m_Socket != nullptr){
        disconnect(m_Socket, SIGNAL(readyRead()), this, SLOT(readData()));
        disconnect(m_Socket, SIGNAL(disconnected()) ,this, SLOT(closeSocket()));
        m_Socket->close();
        m_Socket = nullptr;
    }

    if(m_socketToPhawd != nullptr){
        free(m_socketToPhawd);
        m_socketToPhawd = nullptr;
    }
    emit connected(false);
}

void SocketConnect::closeSocket(){
    disconnect(m_Socket, SIGNAL(readyRead()), this, SLOT(readData()));
    disconnect(m_Socket, SIGNAL(disconnected()) ,this, SLOT(closeSocket()));
    m_Socket->close();
    m_Socket = nullptr;
    emit connected(false);
}

SocketConnect::~SocketConnect() {
    if(m_Server != nullptr){
        disconnect(m_Server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
        delete m_Server;
        m_Server = nullptr;
    }
    if(m_Socket != nullptr){
        m_Socket->close();
        m_Socket = nullptr;
    }
    if(m_socketToPhawd != nullptr){
        free(m_socketToPhawd);
        m_socketToPhawd = nullptr;
    }
}

phawd::SocketToPhawd* SocketConnect::getRead(){
    return m_socketToPhawd;
}