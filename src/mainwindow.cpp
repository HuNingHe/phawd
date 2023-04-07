/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-14
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file mainwindow.cpp
 * @brief main window gui
 */

#include <utility>
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), m_waveShow(new WaveShow) {
    ui->setupUi(this);
    this->initForm();
    this->slotConnect();
}

MainWindow::~MainWindow(){
    delete readOnlyDelegate;
    delete comboBoxDelegate;
    delete lineEditDelegate0;
    delete lineEditDelegate2;
    delete m_waveShow;

    if (m_socketConnect != nullptr){
        m_socketConnect->deleteLater();
    }

    if(m_joystickWindow != nullptr){
        m_joystickWindow->deleteLater();
    }

    m_dataDetect->setStopFlag(true);
    m_dataDetectThread->quit();
    m_dataDetect->clearPtr();

    m_dataDetectThread->deleteLater();
    m_dataDetect->deleteLater();

    delete ui;
}

void MainWindow::initForm(){
    QIcon hintIcon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion);
    ui->nameHint->setIcon(hintIcon);
    ui->nameHint->setToolTip(tr("SharedMemory Name or Socket Port"));

    QStringList Items1 = {"SharedMemory","Socket"};
    ui->choicesBox->addItems(Items1);

    ui->waveParameterNum->setMaximum(INT16_MAX);
    ui->waveParameterNum->setMinimum(0);

    this->readOnlyDelegate = new ReadOnlyDelegate();
    this->comboBoxDelegate = new ComboBoxDelegate();

    QRegExp regExp("^[a-zA-Z_]\\w{,10}$");
    ui->robotNameEdit->setValidator(new QRegExpValidator(regExp, this));

    QStringList Items2 = {"FLOAT","DOUBLE","S64","VEC3_FLOAT", "VEC3_DOUBLE"};
    this->comboBoxDelegate->setItems(Items2);
    ui->paramTableWidget->setItemDelegateForColumn(1, this->comboBoxDelegate);

    //The relevant delegate is set for each column, primarily to control the data that the user enters using regular expressions
    this->lineEditDelegate0 = new LineEditDelegate();
    this->lineEditDelegate2 = new LineEditDelegate();

    this->lineEditDelegate0->setColumn(0);
    this->lineEditDelegate2->setColumn(2);
    ui->paramTableWidget->setItemDelegateForColumn(0, this->lineEditDelegate0);
    ui->paramTableWidget->setItemDelegateForColumn(2, this->lineEditDelegate2);

    ui->paramTableWidget->horizontalHeader()->setVisible(true);
    ui->paramTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    ui->consoleOutput->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->paramTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    m_dataDetectThread = new QThread;

    m_dataDetect = new DataDetect;
    m_dataDetect->moveToThread(m_dataDetectThread);

    m_socketConnect = new SocketConnect(this);
    m_joystickWindow = new JoystickTestWindow();
}

void MainWindow::slotConnect(){
    connect(ui->paramSetting, &QAction::triggered, this, [=](){ui->stackedWidget->setCurrentIndex(0);});
    connect(ui->joystickSetting, &QAction::triggered, this, [=](){m_joystickWindow->show();});
    connect(ui->waveSetting, &QAction::triggered, this, [=](){m_waveShow->show();});
    connect(ui->logWave, &QAction::triggered, this, [=](){ui->stackedWidget->setCurrentIndex(1);});
    /* The slot function used above to control the parameters page */
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(clickAddButton()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(clickDeleteButton()));
    connect(ui->readyButton, SIGNAL(clicked()), this, SLOT(clickReadyButton()));
    connect(ui->undoButton,SIGNAL(clicked()), this, SLOT(clickUndoButton()));
    connect(ui->saveFileButton, SIGNAL(clicked()), this, SLOT(clickSaveFileButton()));
    connect(ui->readFileButton, SIGNAL(clicked()), this, SLOT(clickReadFileButton()));

    connect(ui->paramTableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(cellModified(int,int)));
    connect(ui->consoleOutput, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(consoleMenuRequested(QPoint)));
    connect(ui->paramTableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableMenuRequested(QPoint)));

    connect(ui->choicesBox, SIGNAL(currentIndexChanged(int)), this, SLOT(choiceChanged(int)));

    connect(m_dataDetect, SIGNAL(detected(QStringList)), this, SLOT(receiveWaveParams(QStringList)));
    connect(m_dataDetect,&DataDetect::detected, m_dataDetectThread,&QThread::quit);
    connect(this, SIGNAL(startDetect()), m_dataDetectThread, SLOT(start()));
    connect(this, SIGNAL(startDetect()), m_dataDetect, SLOT(doDetection()));

    connect(m_joystickWindow, SIGNAL(updated()), this, SLOT(updateGamepadCommand()));
    connect(m_socketConnect, &SocketConnect::connected, this, [=](bool isConnected) {
        m_socketConnected = isConnected;
        this->createMessage("Socket Connect! PLease set all waveform parameters(Name, Value, ValueKind...)");
    });
    connect(m_socketConnect, &SocketConnect::readReady, this, &MainWindow::socketReadyRead);
    // finished Signal detection is unnecessary, since we do not need to clean up immediately after the thread exits,
    // the following one is only used to test the thread exit
    // connect(m_dataDetectThread, &QThread::finished, this ,[=](){ printf("thread exit\n");});
}

void MainWindow::receiveWaveParams(QStringList paramsNames){
    this->createMessage("Data entry detected! Now you can choose to display waveforms");
    m_paramsNameList.clear();
    m_paramsNameList = std::move(paramsNames);
    m_waveShow->setSelections(m_paramsNameList);
}

void MainWindow::updateGamepadCommand(){
    if(m_usingSocket){
        if(ui->readyButton->signalsBlocked() && m_socketFromPhawd != nullptr && m_socketConnected){
            m_socketFromPhawd->gameCommand = m_joystickWindow->m_command;
            try{
                m_socketConnect->sendData(m_socketFromPhawd, sizeof(phawd::SocketFromPhawd) +
                                          sizeof(phawd::Parameter) * ui->paramTableWidget->rowCount());
            } catch (std::runtime_error& err){
                createWarningMessage(err.what());
                createMessage("[Socket Connect] write data failed");
            }
        }
    }else{
        if(ui->readyButton->signalsBlocked() && m_sharedObject.get() != nullptr){
            m_sharedObject().gameCommand = m_joystickWindow->m_command;
        }
    }
}

void MainWindow::socketReadyRead(){
    m_paramsNameList.clear();
    try {
        if (m_socketConnect->getRead() == nullptr && m_socketConnect->getRead()->numWaveParams <= 0) {
            throw std::runtime_error("Socket read error");
        }
        if (m_socketConnect->getRead()->numWaveParams > ui->waveParameterNum->value()) {
            throw std::runtime_error("The number of parameters is incorrect: parameters in heap is less than the client sends");
        }
    }catch (std::runtime_error& err){
        this->createWarningMessage(err.what());
        return;
    }

    for(size_t i = 0; i < m_socketConnect->getRead()->numWaveParams; i++) {
        std::string paramName = m_socketConnect->getRead()->parameters[i].getName();
        bool isSet = m_socketConnect->getRead()->parameters[i].isSet();
        try {
            if(paramName.empty()){
                throw std::runtime_error("Please complete all parameters name in client program!");
            }
            if(!isSet){
                std::string message = "Please complete " + std::to_string(i) + "th parameter's value and kind";
                throw std::runtime_error(message);
            }
        } catch(std::runtime_error& err){
            this->createWarningMessage(err.what());
            this->createWarningMessage("Read waveform parameters error");
            return;
        }

        switch (m_socketConnect->getRead()->parameters[i].getValueKind()) {
            case phawd::ParameterKind::VEC3_DOUBLE: {
                std::string paramNameX = paramName + "-x";
                std::string paramNameY = paramName + "-y";
                std::string paramNameZ = paramName + "-z";
                m_paramsNameList.append(QString::fromStdString(paramNameX));
                m_paramsNameList.append(QString::fromStdString(paramNameY));
                m_paramsNameList.append(QString::fromStdString(paramNameZ));
                break;
            }
            case phawd::ParameterKind::VEC3_FLOAT: {
                std::string paramNameX = paramName + "-x";
                std::string paramNameY = paramName + "-y";
                std::string paramNameZ = paramName + "-z";
                m_paramsNameList.append(QString::fromStdString(paramNameX));
                m_paramsNameList.append(QString::fromStdString(paramNameY));
                m_paramsNameList.append(QString::fromStdString(paramNameZ));
                break;
            }
            default:
                m_paramsNameList.append(QString::fromStdString(paramName));
                break;
        }
    }

    try{
        if (m_paramsNameList.isEmpty()) {
            throw std::runtime_error("Socket received data but they are not initialized!");
        }
    }catch(std::runtime_error& err){
        this->createWarningMessage(err.what());
        return;
    }

    bool sameName = false;
    QMap<QString, int> countOfString;
    for (int j = 0; j < m_paramsNameList.count(); j++) {
        countOfString[m_paramsNameList[j]]++;
        if (countOfString[m_paramsNameList[j]] > 1) {
            sameName = true;
            break;
        }
    }

    try{
        if (sameName) {
            throw std::runtime_error("Socket received data but the parameter with the same name exists");
        }
    }catch(std::runtime_error& err){
        this->createWarningMessage(err.what());
        return;
    }

    m_waveShow->setSocketMessage(m_socketConnect->getRead());
    m_waveShow->setSelections(m_paramsNameList);
    try{
        m_socketConnect->sendData(m_socketFromPhawd, sizeof(phawd::SocketFromPhawd) +
                                                     sizeof(phawd::Parameter) * ui->paramTableWidget->rowCount());
    } catch (std::runtime_error& err){
        createWarningMessage(err.what());
        createMessage("[Socket Connect] write data failed");
    }
}

void MainWindow::choiceChanged(int index){
    // Modify the robot name input format
    // In Socket mode, only 6 non-0 digits are allowed to be entered
    // In shared memory mode, only C variable name paradigm names are allowed
    switch (index){
        case 0:{
            ui->robotNameEdit->clear();
            QRegExp regExp("^[a-zA-Z_]\\w{,10}$");
            ui->robotNameEdit->setValidator(new QRegExpValidator(regExp, this));
            m_usingSocket = false;
            m_waveShow->setUsingSocket(m_usingSocket);
            break;
        }
        case 1:{
            ui->robotNameEdit->clear();
            QRegExp regExp("^[1-9]{1,1}\\d{,5}$");
            ui->robotNameEdit->setValidator(new QRegExpValidator(regExp, this));
            m_usingSocket = true;
            m_waveShow->setUsingSocket(m_usingSocket);
            break;
        }
        default:
            break;
    }
}

void MainWindow::clickAddButton() {
    if(!ui->readyButton->signalsBlocked()){
        int currentRow = ui->paramTableWidget->currentRow();
        ui->paramTableWidget->insertRow(currentRow + 1);
    }else{
        QMessageBox::critical(this, tr("Warning"),  tr("Add Failed"), QMessageBox::Discard,  QMessageBox::Discard);
        QString strMessage = "Undo First!";
        this->createWarningMessage(strMessage);
    }
}

void MainWindow::clickDeleteButton() {
    int currentRow = ui->paramTableWidget->currentRow();
    int rowCount = ui->paramTableWidget->rowCount();
    if(!ui->readyButton->signalsBlocked()){
        if (rowCount <= 0){
            QMessageBox::critical(this, tr("Warning"),  tr("Delete Failed"), QMessageBox::Discard,  QMessageBox::Discard);
            QString strMessage = "No Left Rows!";
            this->createWarningMessage(strMessage);
        }else{
            if(currentRow < 0){// No rows are selected
                ui->paramTableWidget->removeRow(rowCount - 1);
            }else{
                ui->paramTableWidget->removeRow(currentRow);
            }
        }
    }else{
        QMessageBox::critical(this, tr("Warning"),  tr("Delete Failed"), QMessageBox::Discard,  QMessageBox::Discard);
        QString strMessage = "Undo First!";
        this->createWarningMessage(strMessage);
    }
}

void MainWindow::clickReadyButton(){
    this->createMessage("Checking Data format...");
    if(this->readyCheck()){
        this->createMessage("Checking Finished");
        int rowCount = ui->paramTableWidget->rowCount();
        int waveParamCount = ui->waveParameterNum->text().toInt();
        /*
         * According to number of parameters to create shared memory
         */
        if(!m_usingSocket){
            this->createMessage("Building Shared Memory...");
            size_t memSize = sizeof(phawd::SharedParameters) + (rowCount + waveParamCount) * sizeof(phawd::Parameter);
            try{
                m_sharedObject.createNew(ui->robotNameEdit->text().toStdString(), memSize);
            }catch (std::runtime_error& err){
                this->createWarningMessage(err.what());
                this->createWarningMessage("Build Shared Memory failed, please follow the tips and retry!");
                return;
            }

            QString strMessage1 = QString("[Shared Memory] CreateNew(%1) success, size: %2 bytes").arg(ui->robotNameEdit->text()).arg(memSize);
            this->createMessage(strMessage1);
            m_sharedObject().gameCommand.init();
            m_sharedObject().numControlParams = rowCount;
            m_sharedObject().numWaveParams = waveParamCount;

            for(int row = 0; row < rowCount; row++){
                QString dataOfCol1 = ui->paramTableWidget->model()->index(row, 0, QModelIndex()).data().toString();
                QString dataOfCol2 = ui->paramTableWidget->model()->index(row, 1, QModelIndex()).data().toString();
                QString dataOfCol3 = ui->paramTableWidget->model()->index(row, 2, QModelIndex()).data().toString();

                QRegExp bracket("(\\[?)|(\\]?)");
                QRegExp split("\\,\\s*");
                QString withoutBracket = dataOfCol3.remove(bracket);
                QStringList list = withoutBracket.split(split);

                m_sharedObject().parameters[row].setName(dataOfCol1.toStdString());
                phawd::ParameterKind kind = phawd::getParameterKindFromString(dataOfCol2.toStdString());
                m_sharedObject().parameters[row].setValueKind(kind);

                switch (kind){
                    case phawd::ParameterKind::FLOAT:
                        m_sharedObject().parameters[row].setValue(list.at(0).toFloat());
                        break;
                    case phawd::ParameterKind::DOUBLE:
                        m_sharedObject().parameters[row].setValue(list.at(0).toDouble());
                        break;
                    case phawd::ParameterKind::S64:
                        m_sharedObject().parameters[row].setValue(list.at(0).toLong());
                        break;
                    case phawd::ParameterKind::VEC3_FLOAT:{
                        float value[3];
                        for (int i = 0; i < 3; i++){
                            value[i] = list.at(i).toFloat();
                        }
                        m_sharedObject().parameters[row].setValue(value);
                        break;
                    }
                    case phawd::ParameterKind::VEC3_DOUBLE:{
                        double value[3];
                        for (int i = 0; i < 3; i++){
                            value[i] = list.at(i).toDouble();
                        }
                        m_sharedObject().parameters[row].setValue(value);
                        break;
                    }
                    default:
                        break;
                }
            }
            /*! Todo: Close the data detect thread is important */
            if(waveParamCount > 0){
                m_dataDetect->setSharedMessage(m_sharedObject.get());
                m_dataDetect->setStopFlag(false);
                m_waveShow->setSharedMessage(m_sharedObject.get());
                emit startDetect();
                this->createMessage("Enter the data input detecting state");
                this->createMessage("PLease set all waveform parameters(Name, Value, ValueKind...)");
            }

            QString strMessage2 = QString("[Shared Memory] Construction completed, of which there are %1 control parameters and %2 waveform parameters").arg(rowCount).arg(waveParamCount);
            this->createMessage(strMessage2);
            this->createMessage("[Shared Memory] Please attach the same size to this shared memory!");
        }else{
            this->createMessage("Creating Socket Server...");
            // using socket
            m_socketFromPhawd = (phawd::SocketFromPhawd*)malloc(sizeof(phawd::SocketFromPhawd)
                                + rowCount * sizeof(phawd::SocketFromPhawd));
            m_socketFromPhawd->numControlParams = rowCount;
            m_socketFromPhawd->gameCommand.init();
            try{
                m_socketConnect->init(ui->robotNameEdit->text().toLong(), waveParamCount);
            }catch(std::runtime_error& err){
                this->createWarningMessage(err.what());
                this->createWarningMessage("This port may be occupied, change and retry!");
                return;
            }
            this->createMessage("[Socket Connect]: Enter the listening state");

            for(int row = 0; row < rowCount; row++){
                QString dataOfCol1 = ui->paramTableWidget->model()->index(row, 0, QModelIndex()).data().toString();
                QString dataOfCol2 = ui->paramTableWidget->model()->index(row, 1, QModelIndex()).data().toString();
                QString dataOfCol3 = ui->paramTableWidget->model()->index(row, 2, QModelIndex()).data().toString();

                QRegExp bracket("(\\[?)|(\\]?)");
                QRegExp split("\\,\\s*");
                QString withoutBracket = dataOfCol3.remove(bracket);
                QStringList list = withoutBracket.split(split);

                m_socketFromPhawd->parameters[row].setName(dataOfCol1.toStdString());
                phawd::ParameterKind kind = phawd::getParameterKindFromString(dataOfCol2.toStdString());
                m_socketFromPhawd->parameters[row].setValueKind(kind);

                switch (kind) {
                    case phawd::ParameterKind::FLOAT:
                        m_socketFromPhawd->parameters[row].setValue(list.at(0).toFloat());
                        break;
                    case phawd::ParameterKind::DOUBLE:
                        m_socketFromPhawd->parameters[row].setValue(list.at(0).toDouble());
                        break;
                    case phawd::ParameterKind::S64:
                        m_socketFromPhawd->parameters[row].setValue(list.at(0).toLong());
                        break;
                    case phawd::ParameterKind::VEC3_FLOAT: {
                        float value[3];
                        for (int i = 0; i < 3; i++) {
                            value[i] = list.at(i).toFloat();
                        }
                        m_socketFromPhawd->parameters[row].setValue(value);
                        break;
                    }
                    case phawd::ParameterKind::VEC3_DOUBLE: {
                        double value[3];
                        for (int i = 0; i < 3; i++) {
                            value[i] = list.at(i).toDouble();
                        }
                        m_socketFromPhawd->parameters[row].setValue(value);
                        break;
                    }
                    default:
                        break;
                }
            }
            /*! Todo: There is no need to detect data input for socket */
        }
        /* Set not editable to choose socket or shared memory */
        for(int i = 0; i < 2; i++){
            QModelIndex index = ui->choicesBox->model()->index(i, 0);
            QVariant v(0);
            ui->choicesBox->model()->setData(index, v, Qt::UserRole - 1);
        }

        //Set first two columns to read-only, keeping the last column editable
        ui->paramTableWidget->setItemDelegateForColumn(0, this->readOnlyDelegate);
        ui->paramTableWidget->setItemDelegateForColumn(1, this->readOnlyDelegate);
        ui->waveParameterNum->setReadOnly(true);

        ui->addButton->blockSignals(true);
        ui->deleteButton->blockSignals(true);
        ui->robotNameEdit->setReadOnly(true);
        ui->readyButton->blockSignals(true);
        ui->readFileButton->blockSignals(true);
        ui->undoButton->blockSignals(false);
        this->createMessage("All Buttons Locked except undo and saveFile button!");
    }else{
        QMessageBox::warning(this, tr("Warning"),  tr("Add parameters as required!"), QMessageBox::Discard,  QMessageBox::Discard);
    }
}

void MainWindow::clickUndoButton(){
    if(ui->readyButton->signalsBlocked()){
        if(!m_usingSocket){
            if(ui->waveParameterNum->value() > 0){
                m_dataDetect->setStopFlag(true);
                m_dataDetectThread->quit();
                m_dataDetect->clearPtr();
                m_waveShow->undoRequest();
                m_waveShow->clearPtr();
            }
            this->createMessage("[Shared Memory] Releasing the shared memory");
            try{
                m_sharedObject.closeNew();
            }catch(std::runtime_error& err){
                this->createWarningMessage(err.what());
                this->createWarningMessage("Undo error!");
                return;
            }
            this->createMessage("[Shared Memory] You should check that if the generated file has been deleted automatically, when you try to close");
        }else{
            if(ui->waveParameterNum->value() > 0) {
                m_waveShow->undoRequest();
                m_waveShow->clearPtr();
            }
            if(m_socketFromPhawd != nullptr){
                free(m_socketFromPhawd);
                m_socketFromPhawd = nullptr;
            }
            m_socketConnect->close();
        }
        // recover editing
        ui->paramTableWidget->setItemDelegateForColumn(0, this->lineEditDelegate0);
        ui->paramTableWidget->setItemDelegateForColumn(1, this->comboBoxDelegate);
        ui->waveParameterNum->setReadOnly(false);
        ui->addButton->blockSignals(false);
        ui->deleteButton->blockSignals(false);
        ui->robotNameEdit->setReadOnly(false);
        ui->readyButton->blockSignals(false);
        ui->readFileButton->blockSignals(false);
        ui->undoButton->blockSignals(true);

        /* Set editable*/
        for(int i = 0; i < 2; i++){
            QModelIndex index = ui->choicesBox->model()->index(i, 0);
            QVariant v(1 | 32);
            ui->choicesBox->model()->setData(index, v, Qt::UserRole - 1);
        }
        this->createMessage("Undo Done!");
    }else{
        QMessageBox::warning(this, tr("Warning"),  tr("Please establish communication first"), QMessageBox::Discard,  QMessageBox::Discard);
        QString strMessage = "Not Ready Yet!";
        this->createWarningMessage(strMessage);
    }
}

void MainWindow::cellModified(int row, int column) {
    //Prevent the modification process from triggering again, getting stuck in iteration
    ui->paramTableWidget->blockSignals(true);
    ui->paramTableWidget->item(row, column)->setTextAlignment(Qt::AlignCenter);
    ui->paramTableWidget->item(row, column)->setFont(QFont("Microsoft YaHei", 10));
    ui->paramTableWidget->blockSignals(false);

    QString dataOfCol2 = ui->paramTableWidget->model()->index(row, 1, QModelIndex()).data().toString();
    phawd::ParameterKind kind = phawd::getParameterKindFromString(dataOfCol2.toStdString());
    QString dataOfCol3 = ui->paramTableWidget->model()->index(row, 2, QModelIndex()).data().toString();

    QRegExp bracket("(\\[?)|(\\]?)");
    QRegExp split("\\,\\s*");
    QString withoutBracket = dataOfCol3.remove(bracket);
    QStringList list = withoutBracket.split(split);

    if(m_usingSocket){
        if(ui->readyButton->signalsBlocked() && m_socketFromPhawd != nullptr && m_socketConnected){
            if(checkOneRow(row)){
                switch (kind) {
                    case phawd::ParameterKind::FLOAT:{
                        m_socketFromPhawd->parameters[row].setValue(list.at(0).toFloat());
                        break;
                    }
                    case phawd::ParameterKind::DOUBLE:{
                        m_socketFromPhawd->parameters[row].setValue(list.at(0).toDouble());
                        break;
                    }
                    case phawd::ParameterKind::S64:{
                        m_socketFromPhawd->parameters[row].setValue(list.at(0).toLong());
                        break;
                    }
                    case phawd::ParameterKind::VEC3_FLOAT:{
                        float value[3];
                        for (int i = 0; i < 3; i++) {
                            value[i] = list.at(i).toFloat();
                        }
                        m_socketFromPhawd->parameters[row].setValue(value);
                        break;
                    }
                    case phawd::ParameterKind::VEC3_DOUBLE:{
                        double value[3];
                        for (int i = 0; i < 3; i++) {
                            value[i] = list.at(i).toDouble();
                        }
                        m_socketFromPhawd->parameters[row].setValue(value);
                        break;
                    }
                    default:
                        return;
                        break;
                }
            } else {
                return;
            }
            try{
                m_socketConnect->sendData(m_socketFromPhawd, sizeof(phawd::SocketFromPhawd) +
                                                             sizeof(phawd::Parameter) * ui->paramTableWidget->rowCount());
            } catch (std::runtime_error& err){
                createWarningMessage(err.what());
                createMessage("[Socket Connect] write data failed");
            }
        }
    }else{
        if(ui->readyButton->signalsBlocked() && m_sharedObject.get() != nullptr){
            if(checkOneRow(row)){
                switch (kind) {
                    case phawd::ParameterKind::FLOAT:{
                        m_sharedObject().parameters[row].setValue(list.at(0).toFloat());
                        break;
                    }
                    case phawd::ParameterKind::DOUBLE:{
                        m_sharedObject().parameters[row].setValue(list.at(0).toDouble());
                        break;
                    }
                    case phawd::ParameterKind::S64:{
                        m_sharedObject().parameters[row].setValue(list.at(0).toLong());
                        break;
                    }
                    case phawd::ParameterKind::VEC3_FLOAT:{
                        float value[3];
                        for (int i = 0; i < 3; i++) {
                            value[i] = list.at(i).toFloat();
                        }
                        m_sharedObject().parameters[row].setValue(value);
                        break;
                    }
                    case phawd::ParameterKind::VEC3_DOUBLE:{
                        double value[3];
                        for (int i = 0; i < 3; i++) {
                            value[i] = list.at(i).toDouble();
                        }
                        m_sharedObject().parameters[row].setValue(value);
                        break;
                    }
                    default:
                        return;
                        break;
                }
            }else{
                return;
            }
        }
    }
}

bool MainWindow::checkOneRow(int row){
    QString dataOfCol1 = ui->paramTableWidget->model()->index(row, 0, QModelIndex()).data().toString();
    QString dataOfCol2 = ui->paramTableWidget->model()->index(row, 1, QModelIndex()).data().toString();
    phawd::ParameterKind kind = phawd::getParameterKindFromString(dataOfCol2.toStdString());
    QString dataOfCol3 = ui->paramTableWidget->model()->index(row, 2, QModelIndex()).data().toString();
    if(dataOfCol1 == "" || dataOfCol2 == "" || dataOfCol3 == ""){
        QString strMessage = "Complete All Parameters First!";
        this->createWarningMessage(strMessage);
        return false;
    }
    QRegExp bracket("(\\[?)|(\\]?)");
    QRegExp split("\\,\\s*");
    QString withoutBracket = dataOfCol3.remove(bracket);
    QStringList list = withoutBracket.split(split);

    switch (kind){
        case phawd::ParameterKind::FLOAT:{
            if(list.count() != 1){
                QString strMessage = QString("The size of value in row %1 is not 1").arg(row+1);
                this->createWarningMessage(strMessage);
                return false;
            }
            bool isOk;
            // The default is decimal conversion
            list.at(0).toFloat(&isOk);
            if(!isOk){
                QString strMessage = QString("The values of row %1 is not an float").arg(row+1);
                this->createWarningMessage(strMessage);
                return false;
            }
            break;
        }
        case phawd::ParameterKind::DOUBLE:{
            if(list.count() != 1){
                QString strMessage = QString("The size of value in row %1 is not 1").arg(row+1);
                this->createWarningMessage(strMessage);
                return false;
            }
            bool isOk;
            // The default is decimal conversion
            list.at(0).toDouble(&isOk);
            if(!isOk){
                QString strMessage = QString("The values of row %1 is not an double").arg(row+1);
                this->createWarningMessage(strMessage);
                return false;
            }
            break;
        }
        case phawd::ParameterKind::S64:{
            if(list.count() != 1){
                QString strMessage = QString("The size of value in row %1 is not 1").arg(row+1);
                this->createWarningMessage(strMessage);
                return false;
            }
            bool isOk;
            // The default is decimal conversion
            list.at(0).toLong(&isOk);
            if(!isOk){
                QString strMessage = QString("The values of row %1 is not an S64").arg(row+1);
                this->createWarningMessage(strMessage);
                return false;
            }
            break;
        }
        case phawd::ParameterKind::VEC3_FLOAT: {
            if(list.count() != 3){
                QString strMessage = QString("The size of value in row %1 is not 3").arg(row+1);
                this->createWarningMessage(strMessage);
                return false;
            }
            bool isOk;
            // The default is decimal conversion
            for (int i = 0; i < 3; i++){
                list.at(i).toFloat(&isOk);
                if(!isOk){
                    QString strMessage = QString("The values of row %1 is not an VEC3_FLOAT").arg(row+1);
                    this->createWarningMessage(strMessage);
                    return false;
                }
            }
            break;
        }
        case phawd::ParameterKind::VEC3_DOUBLE: {
            if(list.count() != 3){
                QString strMessage = QString("The size of value in row %1 is not 3").arg(row+1);
                this->createWarningMessage(strMessage);
                return false;
            }
            bool isOk;
            // The default is decimal conversion
            for (int i = 0; i < 3; i++){
                list.at(i).toDouble(&isOk);
                if(!isOk){
                    QString strMessage = QString("The values of row %1 is not an VEC3_DOUBLE").arg(row+1);
                    this->createWarningMessage(strMessage);
                    return false;
                }
            }
            break;
        }
        default:
            return false;
            break;
    }
    return true;
}

// Check that the data is formatted correctly, and if it is correct, you can start to build up shared memory
bool MainWindow::readyCheck(){
    int rowCount = ui->paramTableWidget->rowCount();
    int waveCount = ui->waveParameterNum->value();
    if(rowCount + waveCount <= 0 || ui->robotNameEdit->text() == ""){
        QString strMessage = "Complete Parameters/WaveParameters and Robot Name!";
        this->createWarningMessage(strMessage);
        return false;
    }else{
        if (ui->choicesBox->currentIndex() == 1){
            bool isOk = false;
            ui->robotNameEdit->text().toLong(&isOk);
            if (!isOk){
                QString strMessage = "Invalid port number!";
                this->createWarningMessage(strMessage);
                return false;
            }
        }

        QStringList listOfColumn1;
        for(int row = 0; row < rowCount; row++){
            QString dataOfCol1 = ui->paramTableWidget->model()->index(row, 0, QModelIndex()).data().toString();

            if(!checkOneRow(row)){
                return false;
            }else {
                // Determine if there are duplicates in the first column
                listOfColumn1.append(dataOfCol1);
            }
        }
        // duplication check in first column
        int duplicates = listOfColumn1.removeDuplicates();
        if (duplicates > 0){
            QString strMessage = QString("There may exist same names in first column");
            this->createWarningMessage(strMessage);
            return false;
        }
        return true;
    }
}

void MainWindow::consoleMenuRequested(QPoint pos){
    if (!pos.isNull()){ // The right mouse button selects the valid location of The Console
        // create menu
        QMenu menu;
        menu.addAction(tr("Clear"), this, [=](){ui->consoleOutput->clear();});
        menu.addAction(tr("Select All"), this, [=](){ui->consoleOutput->selectAll();});
        menu.addAction(tr("Copy"), this, [=](){ui->consoleOutput->copy();});
        menu.exec(QCursor::pos());
    }
}

void MainWindow::tableMenuRequested(QPoint pos){
    if (!pos.isNull()) {// The right mouse button selects the valid location of The tableWidget
        QMenu menu;
        menu.addAction(tr("Add Row"), this, &MainWindow::clickAddButton);
        menu.addAction(tr("Remove Row"), this, &MainWindow::clickDeleteButton);
        menu.addAction(tr("Clear All"), this, [=](){
            int rowCount = ui->paramTableWidget->rowCount();
            for (int i = 0; i < rowCount; ++i) {
                ui->paramTableWidget->removeRow(0);
            }});
        menu.addAction(tr("Load From File"), this, &MainWindow::clickReadFileButton);
        menu.addAction(tr("Save to File"), this, &MainWindow::clickSaveFileButton);
        menu.exec(QCursor::pos());
    }
}

void MainWindow::clickReadFileButton() {
    if(!ui->readyButton->signalsBlocked()){
        this->readFromFile();
    }
}

void MainWindow::clickSaveFileButton(){
    this->createMessage("Checking Data Format...");
    if(ui->readyButton->signalsBlocked() || this->readyCheck()){
        this->createMessage("Checking Finished");
        this->saveToFile();
        this->createMessage("Done!");
    }else{
        QMessageBox:: StandardButton result = QMessageBox::warning(this, tr("Warning"),
                   tr("There may be a problem with the data format, is it saved?"),
                   QMessageBox::Save | QMessageBox::Discard,  QMessageBox::Discard);
        switch (result){
            case QMessageBox::Save:
                this->saveToFile();
                break;
            default:
                break;
        }
    }
}

void MainWindow::saveToFile(){
    int rowCount = ui->paramTableWidget->rowCount();
    YAML::Node userParameters;
    QString configFileName = QFileDialog::getSaveFileName(this,"Set the file name","./","YAML(*.yaml)");
    if(!configFileName.endsWith(".yaml"))
        configFileName.append(".yaml");

    if (!configFileName.isEmpty()){
        std::ofstream configFile(configFileName.toStdString());
        if (rowCount > 0){
            userParameters["RobotName"] = ui->robotNameEdit->text().toStdString();
            userParameters["Type"] = ui->choicesBox->currentText().toStdString();
            userParameters["WaveParamNum"] = ui->waveParameterNum->value();
            for (int row = 0; row < rowCount; row++) {
                QString dataOfCol1 = ui->paramTableWidget->model()->index(row, 0, QModelIndex()).data().toString();
                QString dataOfCol2 = ui->paramTableWidget->model()->index(row, 1, QModelIndex()).data().toString();
                QString dataOfCol3 = ui->paramTableWidget->model()->index(row, 2, QModelIndex()).data().toString();
                // Add the brackets before saving the parameters to yaml files
                if (!dataOfCol3.startsWith("[")){
                    dataOfCol3.insert(0, "[");
                }
                if (!dataOfCol3.endsWith("]")){
                    dataOfCol3.append("]");
                }
                // Value Type Check
                userParameters[dataOfCol2.toStdString()][dataOfCol1.toStdString()] = YAML::Load(dataOfCol3.toStdString());
            }
        }else{
            userParameters["RobotName"] = ui->robotNameEdit->text().toStdString();
            userParameters["Type"] = ui->choicesBox->currentText().toStdString();
            userParameters["WaveParamNum"] = ui->waveParameterNum->value();
            userParameters["FLOAT"]["ParametersName"] = YAML::Load("[]");
            userParameters["DOUBLE"]["ParametersName"] = YAML::Load("[]");
            userParameters["S64"]["ParametersName"] = YAML::Load("[]");
            userParameters["VEC3_FLOAT"]["ParametersName"] = YAML::Load("[]");
            userParameters["VEC3_DOUBLE"]["ParametersName"] = YAML::Load("[]");
        }
        configFile << userParameters;
        configFile.close();
    }
}

void MainWindow::readFromFile(){
    QString configFileName = QFileDialog::getOpenFileName(this,"Select a file to open","./","YAML(*.yaml)");
    int rowCount = ui->paramTableWidget->rowCount();
    YAML::Node userParameters;
    if (!configFileName.isEmpty()){
        try {
            userParameters = YAML::LoadFile(configFileName.toStdString());
        }catch (const YAML::ParserException &e){
            this->createWarningMessage(e.what());
            return;
        }

        for (int i = 0; i < rowCount; ++i) {
            ui->paramTableWidget->removeRow(0);
        }

        if(userParameters["Type"].IsDefined() && userParameters["Type"].IsScalar()){
            QString type = QString::fromStdString(userParameters["Type"].as<std::string>());
            if (type == "SharedMemory"){
                ui->choicesBox->setCurrentIndex(0);
            } else if (type == "Socket"){
                ui->choicesBox->setCurrentIndex(1);
            } else {
                this->createMessage("Invalid Shared Type");
            }
        }else{
            this->createMessage("No definition for (Shared Type) in this yaml file");
        }

        // foreach the parameter types
        bool existParameters = false;
        if(userParameters["RobotName"].IsDefined() && userParameters["RobotName"].IsScalar()){
            QString robotName = QString::fromStdString(userParameters["RobotName"].as<std::string>());
            ui->robotNameEdit->setText(robotName);
        }else{
            this->createMessage("No definition for RobotName in this yaml file");
        }

        bool ok;
        if(userParameters["WaveParamNum"].IsDefined() && userParameters["WaveParamNum"].IsScalar()){
            QString waveParamNum = QString::fromStdString(userParameters["WaveParamNum"].as<std::string>());
            int num = waveParamNum.toInt(&ok);
            if (ok && num >= 0){
                ui->waveParameterNum->setValue(num);
            }else{
                this->createMessage("Invalid WaveParamNum: Should be positive integer");
            }
        }else{
            this->createMessage("No definition for WaveParamNum in this yaml file");
        }

        rowCount = 0;
        // Check all parameter value kind
        for (auto &kind : phawd::ParameterKinds){
            std::string typeString = phawd::ParameterKindToString(kind);
            if (userParameters[typeString].IsDefined() && userParameters[typeString].IsMap()){
                existParameters = true;
                // foreach the YAML node //
                for(YAML::const_iterator it = userParameters[typeString].begin();it != userParameters[typeString].end(); ++it) {
                    QStringList rowDataList;    
                    // parameter name
                    rowDataList << QString::fromStdString(it->first.as<std::string>());
                    // parameter type
                    rowDataList << QString::fromStdString(typeString);
                    // parameter value
                    rowDataList << QString::fromStdString(this->sequence2String(it->second));
                    this->addTableRowData(rowCount, rowDataList);
                    ++rowCount;
                }
            }else{
                QString typeQString = QString::fromStdString(typeString);
                QString strMessage = QString("No definition or map node for ").append(typeQString);
                this->createMessage(strMessage);
            }
        }
        if (!existParameters){
            this->createWarningMessage("Read Yaml file finished, No valid parameters!");
        }
    }else{
        this->createWarningMessage("Read Yaml file error, No Yaml file exists");
    }
}

std::string MainWindow::sequence2String(YAML::Node sequenceNode){
    std::string arrayString;
    if (sequenceNode.IsSequence()){
        arrayString.append("[");
        for (size_t i = 0; i < sequenceNode.size(); i++){
            std::string tmp = sequenceNode[i].as<std::string>();
            arrayString.append(tmp);
            if (i != sequenceNode.size() - 1){
                arrayString.append(", ");
            }
        }
        arrayString.append("]");
    }else{
        this->createMessage("No Sequence Node in the yaml file, regard as NULL string automatically");
    }
    return arrayString;
}

void MainWindow::createMessage(const QString& message){
    ui->consoleOutput->append(message);
}

void MainWindow::createWarningMessage(const QString& message){
    QString warningMessage;
    QString warning = "[Warning]:  ";
    QString colorHead = "<font color=\"#FF0000\">";
    QString colorEnd = "</font> ";
    warningMessage.append(colorHead);
    warning.append(message);
    warningMessage.append(warning);
    warningMessage.append(colorEnd);
    ui->consoleOutput->append(warningMessage);
}

void MainWindow::addTableRowData(int row, const QStringList &dataList){
    ui->paramTableWidget->insertRow(row);
    if (dataList.size() != ui->paramTableWidget->columnCount()){
        this->createWarningMessage("Setting Data from YAML file meets errors: data size is bigger than column count");
    }else{
        for (int i = 0; i < dataList.size(); i++){
            QModelIndex indexOfColumn = ui->paramTableWidget->model()->index(row, i, QModelIndex());
            ui->paramTableWidget->model()->setData(indexOfColumn, dataList[i]);
            // Make the data align in the center of view 
            ui->paramTableWidget->item(row, i)->setTextAlignment(Qt::AlignCenter);
            ui->paramTableWidget->item(row, i)->setFont(QFont("Microsoft YaHei", 10));
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    m_waveShow->undoRequest();
    m_waveShow->clearPtr();
    m_dataDetect->setStopFlag(true);
    m_dataDetectThread->quit();
    m_dataDetect->clearPtr();
    if(m_waveShow != nullptr) {
        delete m_waveShow;
        m_waveShow = nullptr;
    }

    if (m_socketConnect != nullptr) {
        m_socketConnect->deleteLater();
        m_socketConnect = nullptr;
    }

    if(m_joystickWindow != nullptr) {
        m_joystickWindow->deleteLater();
        m_joystickWindow = nullptr;
    }
}