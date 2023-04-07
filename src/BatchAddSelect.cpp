/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-2-24
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file BatchAddSelect.cpp
 * @brief Add curve in waveform displayer
 */
#include <QMessageBox>
#include <QColorDialog>
#include "BatchAddSelect.h"

BatchAddSelectWindow::BatchAddSelectWindow(QWidget *parent) : QDialog(parent), ui(new Ui::BatchAddSelectWindow){
    ui->setupUi(this);
	qRegisterMetaType<LineAttribute>("LineAttribute");

	m_signalMapper = new QSignalMapper;
	m_signalMapper->setMapping(ui->color1, 0);
    m_signalMapper->setMapping(ui->color2, 1);
    m_signalMapper->setMapping(ui->color3, 2);
    m_signalMapper->setMapping(ui->color4, 3);
    connect(ui->color1, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    connect(ui->color2, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    connect(ui->color3, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    connect(ui->color4, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
	connect(m_signalMapper, SIGNAL(mapped(int)), this, SLOT(selectColor(int)));
	for (auto & i : m_colorSelected){
		i = QColor::Invalid;
	}
	connect(ui->checkButton, SIGNAL(clicked()), this, SLOT(allSelected()));
}

BatchAddSelectWindow::~BatchAddSelectWindow(){
	delete m_signalMapper;
    delete ui;
}

void BatchAddSelectWindow::addDataSelections(const QStringList& selections){
	ui->source1->clear();
	ui->source2->clear();
	ui->source3->clear();
	ui->source4->clear();

	ui->source1->addItem("");
	ui->source2->addItem("");
	ui->source3->addItem("");
	ui->source4->addItem("");

	ui->source1->addItems(selections);
	ui->source2->addItems(selections);
	ui->source3->addItems(selections);
	ui->source4->addItems(selections);
}

void BatchAddSelectWindow::allSelected() {
	bool isReady[4] = {false, false, false, false};
	for (int i = 0; i < 4; i++) {
		if(readyCheck(i)){
			isReady[i] = true;
		}
	}

	if (!isReady[0] && !isReady[1] && !isReady[2] && !isReady[3]){
		QMessageBox::critical(this, tr("Warning"),  tr("Please complete at least one Graph selection"),
                              QMessageBox::Discard,  QMessageBox::Discard);
		return;
	}

	std::string dataSource[4] = {"", "", "", ""};
	dataSource[0] = ui->source1->currentText().toStdString();
	dataSource[1] = ui->source2->currentText().toStdString();
	dataSource[2] = ui->source3->currentText().toStdString();
	dataSource[3] = ui->source4->currentText().toStdString();

	int dataIndex[4] = {0, 0, 0, 0};
	dataIndex[0] = ui->source1->currentIndex();
	dataIndex[1] = ui->source2->currentIndex();
	dataIndex[2] = ui->source3->currentIndex();
	dataIndex[3] = ui->source4->currentIndex();

	int width[4] = {0, 0, 0, 0};
	width[0] = ui->width1->value();
	width[1] = ui->width2->value();
	width[2] = ui->width3->value();
	width[3] = ui->width4->value();

	std::string lineStyle[4] = {"", "", "", ""};
	lineStyle[0] = ui->style1->currentText().toStdString();
	lineStyle[1] = ui->style2->currentText().toStdString();
	lineStyle[2] = ui->style3->currentText().toStdString();
	lineStyle[3] = ui->style4->currentText().toStdString();

	for (int i = 0; i < 4; i++){
		LineAttribute tmp;
		// Here we return the index minus 1, because when we add options, we add an empty option at the front
		tmp.init(dataSource[i], dataIndex[i] - 1, width[i], lineStyle[i], m_colorSelected[i]);
		m_selections.append(tmp);
	}

	emit selectFinished(m_selections);
	m_selections.clear();
	
	for (auto & i : m_colorSelected){
		i = QColor::Invalid;
	}
	this->close();
}

bool BatchAddSelectWindow::readyCheck(int button){
	switch (button){
	case 0:{
		if(m_colorSelected[button].isValid() && !ui->source1->currentText().isEmpty()){
			return true;
		}else{
			return false;
		}
	}
	case 1:{
		if(m_colorSelected[button].isValid() && !ui->source2->currentText().isEmpty()){
			return true;
		}else{
			return false;
		}
	}
	case 2:{
		if(m_colorSelected[button].isValid() && !ui->source3->currentText().isEmpty()){
			return true;
		}else{
			return false;
		}
	}
	case 3:{
		if(m_colorSelected[button].isValid() && !ui->source4->currentText().isEmpty()){
			return true;
		}else{
			return false;
		}
	}
	default:
		return false;
	}
}

void BatchAddSelectWindow::selectColor(int button){
	QColor color = QColorDialog::getColor(Qt::red, this, tr("Please select a color."));

	switch (button){
	case 0:{
		QPalette palette = ui->color1->palette();
		palette.setColor(QPalette::Button, color);
		ui->color1->setPalette(palette);
		ui->color1->setAutoFillBackground(true);
		ui->color1->setFlat(true);
		break;
	}
	case 1:{
		QPalette palette = ui->color2->palette();
		palette.setColor(QPalette::Button, color);
		ui->color2->setPalette(palette);
		ui->color2->setAutoFillBackground(true);
		ui->color2->setFlat(true);
		break;
	}
	case 2:{
		QPalette palette = ui->color3->palette();
		palette.setColor(QPalette::Button, color);
		ui->color3->setPalette(palette);
		ui->color3->setAutoFillBackground(true);
		ui->color3->setFlat(true);
		break;
	}
	case 3:{
		QPalette palette = ui->color1->palette();
		palette.setColor(QPalette::Button, color);
		ui->color4->setPalette(palette);
		ui->color4->setAutoFillBackground(true);
		ui->color4->setFlat(true);
		break;
	}
	default:
		break;
	}
	m_colorSelected[button] = color;
}