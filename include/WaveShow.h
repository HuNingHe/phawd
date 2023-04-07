/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-13
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file WaveShow.h
 * @brief waveform display
 */

#pragma once
#include <QWidget>
#include <QTimer>
#include "BatchAddSelect.h"
#include "BatchDeleteSelect.h"
#include "../ui_include/ui_waveshow.h"
#include "phawd/SharedParameter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WaveShow; }
QT_END_NAMESPACE

class WaveShow : public QWidget{
    Q_OBJECT
public:
    explicit WaveShow(QWidget *parent = nullptr);
    ~WaveShow() override;
    void slotConnect();
    void initForm();

    void setSharedMessage(phawd::SharedParameters *sharedParameters){ m_sharedMessage = sharedParameters; }
    void setSocketMessage(phawd::SocketToPhawd *socketToPhawd){ m_socketToPhawd = socketToPhawd; }
    void setUsingSocket(bool usingSocket) { m_usingSocket = usingSocket; }
    void clearPtr();
    void setSelections(QStringList paramsNames);

    QPair<int, int> getIndexOfSelectedParameters(int graphIndex);
    void undoRequest();

protected:
	void closeEvent(QCloseEvent *event) override;
private slots:
    void selectionChanged();
    void saveGraph();
    void addChannel();
    void deleteChannel();
    void StartGraph();
    void PauseGraph();
    void ResetGraph();

    void addDataToGraph();
    void freqChangeInGraph(int value);
    void xAxisWidthChange(int value);
    bool newDataSelectWindow();
    bool newDeleteSelectWindow();
    void receiveLineAttribute(QList<LineAttribute> selected);
    void receiveDeleteSelections(QStringList selected);

private:
    bool m_usingSocket = false;
    bool m_isStarted = false;
    unsigned long long int iter = 0;
    Ui::WaveShow *ui;
    BatchAddSelectWindow *m_dataSelectWindow = nullptr;
    BatchDeleteSelectWindow *m_deleteSelectWindow = nullptr;

    QTimer *m_timer;

    // Note that the map container is not used here because a name may correspond to multiple indexes
    QStringList m_paramsNameList;
    QList<LineAttribute> m_lineAttribute;
    QStringList m_selectedNamesToDelete;
    QStringList m_selectedToAddName;
    QList<int> m_selectedToAddIndex;
    QVector<double> time_lapsed;
    phawd::SharedParameters *m_sharedMessage = nullptr;
    phawd::SocketToPhawd *m_socketToPhawd = nullptr;
};
