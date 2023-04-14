/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-13
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file WaveShow.cpp
 * @brief waveform display
 */
#include "WaveShow.h"

WaveShow::WaveShow(QWidget *parent): QWidget(parent), ui(new Ui::WaveShow){
    ui->setupUi(this);
    m_timer = new QTimer;
    initForm();
    slotConnect();
}

WaveShow::~WaveShow(){
    delete m_timer;
    delete ui;
}

void WaveShow::initForm(){
    setWindowIcon(QIcon(":/image/phawd.ico"));

    ui->xAxisWIDTH->setMinimum(2);
    ui->xAxisWIDTH->setMaximum(10000);
    m_selectedToAddIndex.clear();
    m_selectedToAddName.clear();
    m_usingSocket = false;

    ui->freqInGraph->setRange(1, 100);
    m_paramsNameList.clear();
    m_lineAttribute.clear();
    m_selectedNamesToDelete.clear();
    ui->widget->xAxis->setVisible(true);
    ui->widget->yAxis->setVisible(true);
    ui->widget->xAxis->setTickLabels(true);// Displays the X-axis scale
    ui->widget->yAxis->setTickLabels(true);// Displays the left Y-axis scale
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend |
                                QCP::iMultiSelect | QCP::iSelectItems); // Zoom in and drag to select, scroll to zoom
    ui->widget->xAxis->setLabel("time/s");
    ui->widget->yAxis->setRange(-1, 1);
    ui->widget->xAxis->setRange(0, 2);

    ui->widget->setOpenGl(true);
    ui->widget->legend->setWrap(6);
    ui->widget->legend->setVisible(true);

    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    pen.setStyle(Qt::PenStyle::CustomDashLine);
    ui->widget->selectionRect()->setPen(pen);
    ui->widget->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));
    ui->widget->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);  // Box selection to zoom
}

void WaveShow::slotConnect(){
    connect(m_timer, SIGNAL(timeout()), this, SLOT(addDataToGraph()));
    connect(ui->freqInGraph, SIGNAL(valueChanged(int)), this, SLOT(freqChangeInGraph(int)));
    connect(ui->xAxisWIDTH, SIGNAL(valueChanged(int)), this, SLOT(xAxisWidthChange(int)));

    connect(ui->allReset, SIGNAL(clicked()), this, SLOT(ResetGraph()));
    connect(ui->allStart, SIGNAL(clicked()), this, SLOT(StartGraph()));
    connect(ui->allPause, SIGNAL(clicked()), this, SLOT(PauseGraph()));
    connect(ui->batchAdd, SIGNAL(clicked()), this, SLOT(addChannel()));
    connect(ui->batchDel, SIGNAL(clicked()), this, SLOT(deleteChannel()));
    connect(ui->save, SIGNAL(clicked()), this, SLOT(saveGraph()));
    connect(ui->widget, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
}

void WaveShow::freqChangeInGraph(int value){
    if(m_timer->isActive()){
        m_timer->stop();
        m_timer->setTimerType(Qt::PreciseTimer);
        m_timer->setInterval(value);
        m_timer->start();
    }
}

void WaveShow::clearPtr(){
    m_sharedMessage = nullptr;
    m_socketToPhawd = nullptr;
}

void WaveShow::receiveLineAttribute(QList<LineAttribute> selected){
    m_lineAttribute.clear();
    m_lineAttribute = std::move(selected);
}

void WaveShow::receiveDeleteSelections(QStringList selected){
    m_selectedNamesToDelete.clear();
    m_selectedNamesToDelete = std::move(selected);
}

// QPair's first is the index of the actual parameter, second is the index of the vector (-1 for non-vectors),
// and stores the real index of all selected parameters on a chart
QPair<int, int> WaveShow::getIndexOfSelectedParameters(int graphIndex){
    QPair<int, int> indexPair;
    int vecCount = 0;
    int index = m_selectedToAddIndex[graphIndex];
    // Note here that the m_paramsNameList header is not added with a null character, and the returned index has been
    // subtracted by 1, so there is no need to subtract it here
    for (int i = 2; i < index; i++){
        if(m_paramsNameList[i].contains("-z")) {
            vecCount++;
        }
    }

    if(m_paramsNameList[index].contains("-x")){
        if(m_usingSocket){
            indexPair.first = index - vecCount * 2;
            indexPair.second = 0;
        }else{
            indexPair.first = index - vecCount * 2 + m_sharedMessage->numControlParams;
            indexPair.second = 0;
        }
    }else if(m_paramsNameList[index].contains("-y")){
        if(m_usingSocket){
            indexPair.first = index - vecCount * 2 - 1;
            indexPair.second = 1;
        }else{
            indexPair.first = index - vecCount * 2 + m_sharedMessage->numControlParams - 1;
            indexPair.second = 1;
        }
    }else if(m_paramsNameList[index].contains("-z")){
        if (m_usingSocket){
            indexPair.first = index - vecCount * 2 - 2;
            indexPair.second = 2;
        }else{
            indexPair.first = index - vecCount * 2 + m_sharedMessage->numControlParams - 2;
            indexPair.second = 2;
        }
    }else{
        if(m_usingSocket){
            indexPair.first = index - vecCount * 2;
            indexPair.second = -1;
        }else{
            indexPair.first = index - vecCount * 2 + m_sharedMessage->numControlParams;
            indexPair.second = -1;
        }
    }
    return indexPair;
}

bool WaveShow::newDeleteSelectWindow(){
    m_deleteSelectWindow = new BatchDeleteSelectWindow(this);
    connect(m_deleteSelectWindow, SIGNAL(selectFinished(QStringList)), this, SLOT(receiveDeleteSelections(QStringList)));
    QStringList tmp;
    tmp.clear();
    for (int i = 0; i < ui->widget->graphCount(); ++i) {
        tmp.append(ui->widget->graph(i)->name());
    }
    for (int i = 0; i < 4; ++i) {
        m_deleteSelectWindow->addDataSelections(i, tmp);
    }

    m_deleteSelectWindow->exec();
    delete m_deleteSelectWindow;
    m_deleteSelectWindow = nullptr;

    if (m_selectedNamesToDelete.count() != 4){
        return false;
    }
    if (m_selectedNamesToDelete[0].isEmpty() && m_selectedNamesToDelete[1].isEmpty() &&
        m_selectedNamesToDelete[2].isEmpty() && m_selectedNamesToDelete[3].isEmpty()){
        QMessageBox::critical(this, tr("Warning"),  tr("Please complete at least one Graph selection"),
                              QMessageBox::Discard,  QMessageBox::Discard);
        return false;
    }
    return true;
}

void WaveShow::deleteChannel() {
    if(!newDeleteSelectWindow()) return;
    int alreadyDeleteCount = 0;
    for (int i = 0; i < ui->widget->graphCount(); ++i) {
        if(m_selectedNamesToDelete.contains(ui->widget->graph(i)->name())) {
            m_selectedToAddName.removeAt(i);
            m_selectedToAddIndex.removeAt(i);
            ui->widget->removeGraph(i);
            alreadyDeleteCount++;
            if (alreadyDeleteCount >= 4) break;
        }
    }
    m_selectedNamesToDelete.clear();
}

bool WaveShow::newDataSelectWindow() {
    m_dataSelectWindow = new BatchAddSelectWindow(this);
    connect(m_dataSelectWindow, SIGNAL(selectFinished(QList<LineAttribute>)), this, SLOT(receiveLineAttribute(QList<LineAttribute>)));
    m_dataSelectWindow->addDataSelections(m_paramsNameList);
    // Adding a curve while the image is being drawn will stick the program, so the curve should be paused first
    if (m_timer->isActive()){
        m_timer->stop();
    }
    m_dataSelectWindow->exec();
    delete m_dataSelectWindow;
    m_dataSelectWindow = nullptr;
    if (m_isStarted){
        if (!m_timer->isActive()){ // Recovery
            m_timer->start();
        }
    }
    if(m_lineAttribute.count() != 4) {
        QMessageBox::warning(this, tr("Warning"),  tr("LineAttribute send error"), QMessageBox::Discard,  QMessageBox::Discard);
        return false;
    }
    if(!m_lineAttribute[0].isInit() && !m_lineAttribute[1].isInit() &&
       !m_lineAttribute[2].isInit() && !m_lineAttribute[3].isInit()) {
        return false;
    }
    return true;
}

void WaveShow::addChannel(){
    if (ui->widget->graphCount() >= 16) {
        QMessageBox::critical(this, tr("Warning"),  tr("Up to sixteen curves to avoid stuttering"),
                              QMessageBox::Discard,  QMessageBox::Discard);
        return;
    }
    if(!newDataSelectWindow()) return;
    for (int i = 0; i < 4; i++){
        if(m_lineAttribute[i].isInit()){
            // If this data has already been added, it will not be added repeatedly
            if (!m_selectedToAddName.contains(m_lineAttribute[i].name)) {
                ui->widget->addGraph();
                int current_graph_idx = ui->widget->graphCount();
                QPen drawPen;
                drawPen.setColor(m_lineAttribute[i].color);
                drawPen.setWidth(m_lineAttribute[i].width);
                drawPen.setStyle(m_lineAttribute[i].style);
                ui->widget->graph(current_graph_idx-1)->setPen(drawPen);
                ui->widget->graph(current_graph_idx-1)->setName(m_lineAttribute[i].name);
                m_selectedToAddName.append(m_lineAttribute[i].name);
                m_selectedToAddIndex.append(m_lineAttribute[i].dataIndex);
            }
        }
    }
    m_lineAttribute.clear();
}

void WaveShow::StartGraph(){
    if(!m_paramsNameList.isEmpty()){
        if(!m_timer->isActive()){
            m_timer->setTimerType(Qt::PreciseTimer);
            m_timer->setInterval(ui->freqInGraph->value());
            m_timer->start();
            m_isStarted = true;
        }
    }
}

void WaveShow::undoRequest() {
    if(m_timer->isActive()){
        m_timer->stop();
        m_isStarted = false;
    }
    iter = 0;
    ui->widget->clearGraphs();
    m_selectedToAddName.clear();
    m_selectedToAddIndex.clear();
    m_paramsNameList.clear();
    ui->widget->yAxis->setRange(-1, 1);
    ui->widget->xAxis->setRange(0, 2);
    ui->widget->replot();
}

void WaveShow::PauseGraph(){
    if(!m_paramsNameList.isEmpty()){
        if(m_timer->isActive()){
            m_timer->stop();
            m_isStarted = false;
        }
    }
}

void WaveShow::ResetGraph(){
    if(m_timer->isActive()){
        m_timer->stop();
        m_isStarted = false;
    }

    ui->widget->clearGraphs();
    m_selectedToAddName.clear();
    m_selectedToAddIndex.clear();
    ui->widget->yAxis->setRange(-1, 1);
    ui->widget->xAxis->setRange(0, 2);
    ui->widget->replot();
    iter = 0;
}

void WaveShow::addDataToGraph(){
    QPair<int, int> paramsIndex;
    double time = iter * 0.001;

    for (int i = 0; i < ui->widget->graphCount(); i++){
        paramsIndex = getIndexOfSelectedParameters(i);

        if(!m_usingSocket){
            switch (m_sharedMessage->parameters[paramsIndex.first].getValueKind()){
                case phawd::ParameterKind::VEC3_FLOAT:{
                    float value;
                    try{
                        value = m_sharedMessage->parameters[paramsIndex.first].getFromVec3fByIndex(paramsIndex.second);
                        ui->widget->graph(i)->addData(time, value);
                        break;
                    } catch (std::runtime_error &err){
                        break;
                    }
                }
                case phawd::ParameterKind::VEC3_DOUBLE:{
                    double value;
                    try{
                        value = m_sharedMessage->parameters[paramsIndex.first].getFromVec3dByIndex(paramsIndex.second);
                        ui->widget->graph(i)->addData(time, value);
                        break;
                    } catch (std::runtime_error &err){
                        break;
                    }
                }
                case phawd::ParameterKind::DOUBLE:{
                    double value;
                    try{
                        value = m_sharedMessage->parameters[paramsIndex.first].getDouble();
                        ui->widget->graph(i)->addData(time, value);
                        break;
                    } catch (std::runtime_error &err){
                        break;
                    }
                }
                case phawd::ParameterKind::FLOAT:{
                    float value;
                    try{
                        value = m_sharedMessage->parameters[paramsIndex.first].getFloat();
                        ui->widget->graph(i)->addData(time, value);
                        break;
                    } catch (std::runtime_error &err){
                        break;
                    }
                }
                case phawd::ParameterKind::S64:{
                    long value;
                    try{
                        value = m_sharedMessage->parameters[paramsIndex.first].getS64();
                        ui->widget->graph(i)->addData(time, value);
                        break;
                    } catch (std::runtime_error &err){
                        break;
                    }
                }
                default:
                    break;
            }
        }else{
            switch (m_socketToPhawd->parameters[paramsIndex.first].getValueKind()){
                case phawd::ParameterKind::VEC3_FLOAT:{
                    float value;
                    try{
                        value = m_socketToPhawd->parameters[paramsIndex.first].getFromVec3fByIndex(paramsIndex.second);
                        ui->widget->graph(i)->addData(time, value);
                        break;
                    } catch (std::runtime_error &err){
                        break;
                    }
                }
                case phawd::ParameterKind::VEC3_DOUBLE:{
                    double value;
                    try{
                        value = m_socketToPhawd->parameters[paramsIndex.first].getFromVec3dByIndex(paramsIndex.second);
                        ui->widget->graph(i)->addData(time, value);
                        break;
                    } catch (std::runtime_error &err){
                        break;
                    }
                }
                case phawd::ParameterKind::DOUBLE:{
                    double value;
                    try{
                        value = m_socketToPhawd->parameters[paramsIndex.first].getDouble();
                        ui->widget->graph(i)->addData(time, value);
                        break;
                    } catch (std::runtime_error &err){
                        break;
                    }
                }
                case phawd::ParameterKind::FLOAT:{
                    float value;
                    try{
                        value = m_socketToPhawd->parameters[paramsIndex.first].getFloat();
                        ui->widget->graph(i)->addData(time, value);
                        break;
                    } catch (std::runtime_error &err){
                        break;
                    }
                }
                case phawd::ParameterKind::S64:{
                    long value;
                    try{
                        value = m_socketToPhawd->parameters[paramsIndex.first].getS64();
                        ui->widget->graph(i)->addData(time, value);
                        break;
                    } catch (std::runtime_error &err){
                        break;
                    }
                }
                default:
                    break;
            }
        }
    }
    QCPRange XAxis_Range_Pre = ui->widget->xAxis->range();// Gets the axis value before adjustment
    if (time >= XAxis_Range_Pre.upper){
        XAxis_Range_Pre.lower = time - ui->xAxisWIDTH->value();
        XAxis_Range_Pre.upper = time;
        ui->widget->xAxis->setRange(XAxis_Range_Pre);
    }

    ui->widget->replot();
    iter += m_timer->interval();
}

void WaveShow::closeEvent(QCloseEvent *event){
    PauseGraph();
}

void WaveShow::setSelections(QStringList paramsNames) {
    m_paramsNameList = std::move(paramsNames);
}

void WaveShow::saveGraph() {
    if(m_timer->isActive()){
        m_timer->stop();
    }
    QString configFileName = QFileDialog::getSaveFileName(this,"Set the image name","./","*.png");

    if (!configFileName.isEmpty()) {
        ui->widget->savePng(configFileName);
        if(!m_timer->isActive()){
            m_timer->start();
        }
    } else{
        return;
    }
}

void WaveShow::selectionChanged(){
    // Synchronously select the graph with the corresponding legend item:
    for (int i=0; i<ui->widget->graphCount(); ++i){
        QCPGraph *graph = ui->widget->graph(i);
        QCPPlottableLegendItem *item = ui->widget->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected()){
            item->setSelected(true);                                           // Finish checking the legend
            graph->setSelection(QCPDataSelection(graph->data()->dataRange())); // Complete the curve selection
        }
    }
}

void WaveShow::xAxisWidthChange(int value) {
    QCPRange XAxis_Range_Pre = ui->widget->xAxis->range();// Gets the axis value before adjustment
    ui->widget->xAxis->setRange(XAxis_Range_Pre.lower, XAxis_Range_Pre.lower + value);
    ui->widget->replot();
}
