/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-12
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file BatchDeleteSelect.h
 * @brief Delete curve in waveform displayer
 */

#pragma once
#include <QDialog>
#include <QMessageBox>
#include "../ui_include/ui_batch_delete_select.h"

QT_BEGIN_NAMESPACE
namespace Ui { class BatchDeleteSelectWindow; }
QT_END_NAMESPACE

class BatchDeleteSelectWindow : public QDialog{
	Q_OBJECT
public:
  	explicit BatchDeleteSelectWindow(QWidget *parent = nullptr);

  	~BatchDeleteSelectWindow() override;
	void addDataSelections(int index, const QStringList& selections);
private slots:
	void allSelected();

signals:
    void selectFinished(QStringList selected);

private:
  	Ui::BatchDeleteSelectWindow *ui;
    QStringList m_selections;
};
