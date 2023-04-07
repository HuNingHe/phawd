/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-12
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file BatchDeleteSelect.cpp
 * @brief Delete curve in waveform displayer
 */
#include "BatchDeleteSelect.h"

BatchDeleteSelectWindow::BatchDeleteSelectWindow(QWidget *parent) : QDialog(parent), ui(new Ui::BatchDeleteSelectWindow){
    ui->setupUi(this);
	connect(ui->checkButton, SIGNAL(clicked()), this, SLOT(allSelected()));
    m_selections.clear();
}

BatchDeleteSelectWindow::~BatchDeleteSelectWindow(){
    delete ui;
}

void BatchDeleteSelectWindow::addDataSelections(int index, const QStringList& selections){
    switch (index){
        case 0:{
            ui->source1->clear();
            ui->source1->addItem("");
            ui->source1->addItems(selections);
            break;
        }
        case 1:{
            ui->source2->clear();
            ui->source2->addItem("");
            ui->source2->addItems(selections);
            break;
        }
        case 2:{
            ui->source3->clear();
            ui->source3->addItem("");
            ui->source3->addItems(selections);
            break;
        }
        case 3:{
            ui->source4->clear();
            ui->source4->addItem("");
            ui->source4->addItems(selections);
            break;
        }
        default:
            break;
    }
}

void BatchDeleteSelectWindow::allSelected(){
    QString dataName[4];
    dataName[0] = ui->source1->currentText();
    dataName[1] = ui->source2->currentText();
    dataName[2] = ui->source3->currentText();
    dataName[3] = ui->source4->currentText();
    if (dataName[0].isEmpty() && dataName[1].isEmpty() && dataName[2].isEmpty() && dataName[3].isEmpty()){
        QMessageBox::critical(this, tr("Warning"),  tr("Please complete at least one Graph selection."),
                              QMessageBox::Discard,  QMessageBox::Discard);
        return;
    }
    for (const auto & i : dataName) {
        m_selections.append(i);
    }
	emit selectFinished(m_selections);
    m_selections.clear();
    this->close();
}