/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-13
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file DataDetectThread.h
 * @brief detect waveform data writing in shared memory
 */
#pragma once
#include <QMap>
#include <QObject>
#include <QThread>
#include "phawd/SharedParameter.h"

class DataDetect :public QObject {
    Q_OBJECT
public:
    explicit DataDetect(QObject *parent = nullptr);
    ~DataDetect() noexcept override = default;

    void setSharedMessage(phawd::SharedParameters *sharedParameters){ m_sharedMessage = sharedParameters; }
    void setStopFlag(bool stop){ m_stop = stop; }
    void clearPtr();

public slots:
    void doDetection();

signals:
    void detected(const QStringList paramsNameLists);

private:
    volatile bool m_stop = true;
    phawd::SharedParameters *m_sharedMessage = nullptr;
};
