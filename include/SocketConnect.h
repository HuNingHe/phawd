/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-13
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file SocketConnect.h
 * @brief socket connection
 */

#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include "phawd/SharedParameter.h"
/*!
 * Note that the socket header file is similar to the socket function under the Windows system, 
 * which is used to store sockets, which contain the destination address and the current address and the corresponding port
 */
class SocketConnect : public QObject{
    Q_OBJECT
private:
    // Note that last time it was caused by not initializing the pointer, and when the program exited,
    // the field pointer was destroyed, resulting in an exit exception
    QTcpServer *m_Server = nullptr;
    QTcpSocket *m_Socket = nullptr;

    size_t m_numWaveParams;
    phawd::SocketToPhawd *m_socketToPhawd = nullptr;

signals:
    void connected(bool isConnected);
    void readReady();
public:
    explicit SocketConnect(QObject *parent = nullptr);
    ~SocketConnect() override;
    void init(unsigned short port, size_t parametersNum);
    phawd::SocketToPhawd *getRead();
    void sendData(void *data, size_t sendSize);
    void close();

private slots:
    void slotNewConnection();
    void readData();
    void closeSocket();
};