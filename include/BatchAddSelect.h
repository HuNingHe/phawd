/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-2-24
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file BatchAddSelect.h
 * @brief Add curve in waveform displayer
 */

#pragma once
#include <QPen>
#include <QDialog>
#include <QSignalMapper>
#include "LineAttribute.h"
#include "../ui_include/ui_batch_add_select.h"

QT_BEGIN_NAMESPACE
namespace Ui { class BatchAddSelectWindow; }
QT_END_NAMESPACE

class BatchAddSelectWindow : public QDialog{
	Q_OBJECT
public:
  	explicit BatchAddSelectWindow(QWidget *parent = nullptr);

  	~BatchAddSelectWindow() override;
	void addDataSelections(const QStringList& selections);
	bool readyCheck(int button);
private slots:
	void allSelected();
	void selectColor(int button);

signals:
    void selectFinished(QList<LineAttribute> selected);

private:
  	Ui::BatchAddSelectWindow *ui;
	QSignalMapper *m_signalMapper;
	QList<LineAttribute> m_selections;
	QColor m_colorSelected[4];
};