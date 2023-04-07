/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-13
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file DataDetectThread.cpp
 * @brief detect waveform data writing in shared memory
 */

#include "DataDetectThread.h"

DataDetect::DataDetect(QObject *parent) : QObject(parent) {

}

void DataDetect::clearPtr(){
    m_sharedMessage = nullptr;
}

void DataDetect::doDetection() {
    // clear the parameters name list before next check
    QStringList _paramsNameList;
    size_t vecCount = 0;
    while (!m_stop) {
        vecCount = 0;
        _paramsNameList.clear();
        if (m_sharedMessage != nullptr && m_sharedMessage->connected > 0) {
            for (size_t i = m_sharedMessage->numControlParams;
                 i < m_sharedMessage->numControlParams + m_sharedMessage->numWaveParams; i++) {
                std::string paramName = m_sharedMessage->parameters[i].getName();
                if (paramName.empty() || !m_sharedMessage->parameters[i].isSet()){
                    continue;
                }
                switch (m_sharedMessage->parameters[i].getValueKind()){
                    case phawd::ParameterKind::VEC3_DOUBLE: {
                        std::string paramNameX = paramName + "-x";
                        std::string paramNameY = paramName + "-y";
                        std::string paramNameZ = paramName + "-z";
                        _paramsNameList.append(QString::fromStdString(paramNameX));
                        _paramsNameList.append(QString::fromStdString(paramNameY));
                        _paramsNameList.append(QString::fromStdString(paramNameZ));
                        vecCount++;
                        break;
                    }
                    case phawd::ParameterKind::VEC3_FLOAT: {
                        std::string paramNameX = paramName + "-x";
                        std::string paramNameY = paramName + "-y";
                        std::string paramNameZ = paramName + "-z";
                        _paramsNameList.append(QString::fromStdString(paramNameX));
                        _paramsNameList.append(QString::fromStdString(paramNameY));
                        _paramsNameList.append(QString::fromStdString(paramNameZ));
                        vecCount++;
                        break;
                    }
                    default:
                        _paramsNameList.append(QString::fromStdString(paramName));
                        break;
                }
            }

            if (_paramsNameList.isEmpty()) {
                continue;
            }
            if (_paramsNameList.count() != m_sharedMessage->numControlParams + 2 * vecCount ){
                continue;
            }

            bool sameName = false;
            QMap<QString, int> countOfString;
            for (int j = 0; j < _paramsNameList.count(); j++) {
                countOfString[_paramsNameList[j]]++;
                if (countOfString[_paramsNameList[j]] > 1) {
                    sameName = true;
                    break;
                }
            }

            if (sameName) {
                continue;
            }

            emit detected(_paramsNameList);
            break;
        }
        QThread::currentThread()->msleep(1000);
    }
}