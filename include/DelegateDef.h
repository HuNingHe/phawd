/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-14
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file DelegateDef.h
 */
#pragma once
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QItemDelegate>

// After the OK is pressed, except for the last column, which can continue editing,
// the Editor of the remaining columns is deleted
class ReadOnlyDelegate: public QItemDelegate {
public:
    explicit ReadOnlyDelegate(QWidget *parent = nullptr):QItemDelegate(parent){

    }
    // There is no editor, and there is no need to exchange data between the model and view,
    // the data of the model will be directly displayed in the view
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        Q_UNUSED(parent)
        Q_UNUSED(option)
        Q_UNUSED(index)
        return nullptr;
    }
};

// The delegate in the second column, which is used to select the data type
class ComboBoxDelegate : public QItemDelegate {
public:
    explicit ComboBoxDelegate(QObject *parent = nullptr): QItemDelegate(parent) {}
    void setItems(const QStringList &texts) { _texts = texts; }

public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        Q_UNUSED(option)
        Q_UNUSED(index)
        auto *editor = new QComboBox(parent);
        editor->addItems(_texts);
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override{
        QString text = index.model()->data(index, Qt::EditRole).toString();
        auto *comboBox = dynamic_cast<QComboBox*>(editor);
        int text_index = comboBox->findText(text);
        comboBox->setCurrentIndex(text_index);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override{
        auto *comboBox = dynamic_cast<QComboBox*>(editor);
        QString text = comboBox->currentText();
        model->setData(index, text, Qt::EditRole);
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        Q_UNUSED(option)
        Q_UNUSED(index)
        editor->setGeometry(option.rect);
    }

private:
    QStringList _texts;
};

// The delegate in the first and third columns is used to qualify the beginning of the input character
// and the positive integer, respectively. And the fourth column does not specifically specify the delegate,
// because the data format of each row may be different
class LineEditDelegate : public QItemDelegate {
private:
    int column;
public:
    explicit LineEditDelegate(QObject *parent = nullptr): QItemDelegate(parent) ,column(0){ }
    void setColumn(int column_){this->column = column_;}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        Q_UNUSED(option)
        Q_UNUSED(index)
        auto *editor = new QLineEdit(parent);
        switch (this->column) {
        case 0:{
            QRegExp regExp("^[a-zA-Z_]\\w{,15}$");
            editor->setValidator(new QRegExpValidator(regExp, parent));
            return editor;
        }
        default:{
//            QRegExp regExp("^\\[?((-?[1-9]\\d*(\\.\\d+)?(\\,\\s*)?)|(-?0\\.\\d*[1-9](\\,\\s*)?))+\\]?$");
//            editor->setValidator(new QRegExpValidator(regExp, parent));
            //editor->setAlignment(Qt::AlignHCenter);
            return editor;
        }
        }
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override{
        QString text = index.model()->data(index, Qt::EditRole).toString();
        auto *lineEdit = dynamic_cast<QLineEdit*>(editor);
        lineEdit->setText(text);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override{
        auto *lineEdit = dynamic_cast<QLineEdit*>(editor);
        QString text = lineEdit->text();
        model->setData(index, text, Qt::EditRole);
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        Q_UNUSED(option)
        Q_UNUSED(index)
        editor->setGeometry(option.rect);
    }
};

class SpinBoxDelegate : public QItemDelegate{
public:
    explicit SpinBoxDelegate(QObject *parent = nullptr): QItemDelegate(parent) {

    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        Q_UNUSED(option)
        Q_UNUSED(index)
        auto *editor = new QSpinBox(parent);
        editor->setMinimum(1);
        editor->setMaximum(30);
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override{
        int value = index.model()->data(index, Qt::EditRole).toInt();
        auto *spinBox = dynamic_cast<QSpinBox*>(editor);
        spinBox->setValue(value);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override{
        auto *spinBox = dynamic_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();
        model->setData(index, value, Qt::EditRole);
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        Q_UNUSED(option)
        Q_UNUSED(index)
        editor->setGeometry(option.rect);
    }
};
