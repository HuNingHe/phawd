/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-14
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file mainwindow.h
 * @brief main window gui
 */

#pragma once
#include <QTimer>
#include <QThread>
#include <QGamepad>
#include <QPushButton>
#include <QMessageBox>
#include <QModelIndex>
#include <QMainWindow>
#include <QFileDialog>
#include <QStandardItemModel>

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "WaveShow.h"
#include "DelegateDef.h"
#include "SocketConnect.h"
#include "DataDetectThread.h"
#include "phawd/SharedMemory.h"
#include "JoystickTestWindow.h"
#include "../ui_include/ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void startDetect();

protected:
	void closeEvent(QCloseEvent *event) override;

private:
    void slotConnect();
    void initForm();
    // Check For Shared Memory Building or Socket Connection Create
    bool readyCheck();
    bool checkOneRow(int row);

    // Convert YAML Sequence to String
    std::string sequence2String(YAML::Node sequenceNode);

    // Create Console Message
    void createMessage(const QString& message);
    void createWarningMessage(const QString& message);

    // Set Data For TableWidget Rows
    void addTableRowData(int row, const QStringList &dataList);

    // Save/Read Config File
    void saveToFile();
    void readFromFile();

private slots:
    /************For Parameter Page**************/
    void clickDeleteButton();
    void clickAddButton();
    void clickReadyButton();
    void clickUndoButton();
    void clickReadFileButton();
    void clickSaveFileButton();

    void cellModified(int row, int column);
    void consoleMenuRequested(QPoint pos);
    void tableMenuRequested(QPoint pos);
    void choiceChanged(int index);

    void receiveWaveParams(QStringList paramsNames);
    /*********************************************/
    void socketReadyRead();
    void updateGamepadCommand();

private:
    bool m_usingSocket = false;
    bool m_socketConnected = false;

    WaveShow *m_waveShow;
    Ui::MainWindow *ui = nullptr;
    ReadOnlyDelegate* readOnlyDelegate;
    ComboBoxDelegate* comboBoxDelegate;
    LineEditDelegate* lineEditDelegate0;
    LineEditDelegate* lineEditDelegate2;

    phawd::SharedMemory<phawd::SharedParameters> m_sharedObject;
    phawd::SocketFromPhawd *m_socketFromPhawd;

    QStringList m_paramsNameList;

    JoystickTestWindow *m_joystickWindow;
    SocketConnect *m_socketConnect;
    QThread *m_dataDetectThread;
    DataDetect *m_dataDetect;
};
