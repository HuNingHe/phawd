/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-13
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file JoystickTestWindow.cpp
 * @brief check joystick
 */

#include <QDebug>
#include <QWindow>
#include "JoystickTestWindow.h"

JoystickTestWindow::JoystickTestWindow(QWidget *parent) :QWidget(parent),m_manager(QGamepadManager::instance()), ui(new Ui::JoystickTestWindow){
    ui->setupUi(this);

    auto lstDevices = m_manager->connectedGamepads();
    m_gamepad = new QGamepad(lstDevices[0], this);
    if (!lstDevices.empty()){
        ui->connectedState->setText("Connected");
    } else {
        ui->connectedState->setText("Disconnected");
    }

    ui->connectedState->setReadOnly(true);
    ui->currentBtnName->setReadOnly(true);

    ui->LT->setRange(0, 100);
    ui->LT->setValue(0);
    ui->LT->setFormat(tr("%1").arg(QString::number(0.0, 'f', 2)));
    ui->LT->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    ui->RT->setRange(0, 100);
    ui->RT->setValue(0);
    ui->RT->setFormat(tr("%1").arg(QString::number(0.0, 'f', 2)));
    ui->RT->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    ui->leftX->setRange(-100, 100);
    ui->leftX->setValue(0);
    ui->leftX->setFormat(tr("%1").arg(QString::number(0.0, 'f', 2)));
    ui->leftX->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    ui->leftY->setRange(-100, 100);
    ui->leftY->setValue(0);
    ui->leftY->setFormat(tr("%1").arg(QString::number(0.0, 'f', 2)));
    ui->leftY->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    ui->rightX->setRange(-100, 100);
    ui->rightX->setValue(0);
    ui->rightX->setFormat(tr("%1").arg(QString::number(0.0, 'f', 2)));
    ui->rightX->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    ui->rightY->setRange(-100, 100);
    ui->rightY->setValue(0);
    ui->rightY->setFormat(tr("%1").arg(QString::number(0.0, 'f', 2)));
    ui->rightY->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    connect(m_gamepad, &QGamepad::buttonSelectChanged, this, [=](bool pressed){
        showCurrentBtnName(pressed, "Select");
    });
    connect(m_gamepad, &QGamepad::buttonStartChanged, this, [=](bool pressed){
        showCurrentBtnName(pressed, "Start");
    });

    connect(m_gamepad, &QGamepad::buttonCenterChanged, this, [=](bool pressed){
        showCurrentBtnName(pressed, "Center");
    });

    connect(m_gamepad, &QGamepad::buttonAChanged, this, [=](bool pressed){
        showCurrentBtnName(pressed, "A");
    });
    connect(m_gamepad, &QGamepad::buttonBChanged, this, [=](bool pressed){
        showCurrentBtnName(pressed, "B");
    });
    connect(m_gamepad, &QGamepad::buttonXChanged, this, [=](bool pressed){
        showCurrentBtnName(pressed, "X");
    });
    connect(m_gamepad, &QGamepad::buttonYChanged, this, [=](bool pressed){
        showCurrentBtnName(pressed, "Y");
    });
    connect(m_gamepad, &QGamepad::buttonL1Changed, this, [=](bool pressed){
        showCurrentBtnName(pressed, "LB");
    });
    connect(m_gamepad, &QGamepad::buttonR1Changed, this, [=](bool pressed){
        showCurrentBtnName(pressed, "RB");
    });
    connect(m_gamepad, &QGamepad::buttonL2Changed, this, [=](bool pressed){
        showCurrentBtnName(pressed, "LT");
    });
    connect(m_gamepad, &QGamepad::buttonR2Changed, this, [=](bool pressed){
        showCurrentBtnName(pressed, "RT");
    });

    connect(m_gamepad, &QGamepad::buttonUpChanged, this, [=](bool pressed){
        showCurrentBtnName(pressed, "Up");
    });
    connect(m_gamepad, &QGamepad::buttonDownChanged, this, [=](bool pressed){
        showCurrentBtnName(pressed, "Down");
    });
    connect(m_gamepad, &QGamepad::buttonLeftChanged, this, [=](bool pressed){
        showCurrentBtnName(pressed, "Left");
    });
    connect(m_gamepad, &QGamepad::buttonRightChanged, this, [=](bool pressed){
        showCurrentBtnName(pressed, "Right");
    });

    connect(m_gamepad, &QGamepad::buttonL3Changed, this, [=](bool pressed){
        showCurrentBtnName(pressed, "LeftAxis");
    });
    connect(m_gamepad, &QGamepad::buttonR3Changed, this, [=](bool pressed){
        showCurrentBtnName(pressed, "RightAxis");
    });

    connect(m_gamepad, &QGamepad::buttonL2Changed, this, [=](double value){
        showBtnValue(1, value);
    });
    connect(m_gamepad, &QGamepad::buttonR2Changed, this, [=](double value){
        showBtnValue(2, value);
    });
    connect(m_gamepad, &QGamepad::axisLeftXChanged, this, [=](double value){
        showBtnValue(3, value);
    });
    connect(m_gamepad, &QGamepad::axisLeftYChanged, this, [=](double value){
        showBtnValue(4, value);
    });
    connect(m_gamepad, &QGamepad::axisRightXChanged, this, [=](double value){
        showBtnValue(5, value);
    });
    connect(m_gamepad, &QGamepad::axisRightYChanged, this, [=](double value){
        showBtnValue(6, value);
    });

    connect(m_gamepad, &QGamepad::connectedChanged, this, [=](bool value){
        if(value){
            ui->connectedState->setText("Connected");
        }else{
            ui->connectedState->setText("Disconnected");
        }
    });
}

JoystickTestWindow::~JoystickTestWindow(){
    delete ui;
    delete m_gamepad;
}

void JoystickTestWindow::showCurrentBtnName(bool pressed, const QString& name){
    if(pressed){
        ui->currentBtnName->setText(name);
    }else{
        ui->currentBtnName->clear();
    }
    updateJoystick();
}

void JoystickTestWindow::showBtnValue(int btnName, double value){
    switch (btnName) {
        //buttonL2
    case 1:{
        if(value * 100 >= 100){
            ui->LT->setValue(100);
            ui->LT->setFormat(tr("%1").arg(QString::number(1, 'f', 2)));
        }else{
            ui->LT->setValue(qRound(value * 100));
            ui->LT->setFormat(tr("%1").arg(QString::number(value, 'f', 2)));
        }
        break;
    }
        //buttonR2
    case 2:{
        if(value * 100 >= 100){
            ui->RT->setValue(100);
            ui->RT->setFormat(tr("%1").arg(QString::number(1, 'f', 2)));
        }else{
            ui->RT->setValue(qRound(value * 100));
            ui->RT->setFormat(tr("%1").arg(QString::number(value, 'f', 2)));
        }
        break;
    }
        //buttonL3, axisLeftX
    case 3:{
        if(value * 100 >= 100){
            ui->leftX->setValue(100);
            ui->leftX->setFormat(tr("%1").arg(QString::number(1, 'f', 2)));
        }else if(value * 100 <= -100){
            ui->leftX->setValue(-100);
            ui->leftX->setFormat(tr("%1").arg(QString::number(-1, 'f', 2)));
        }else{
            ui->leftX->setValue(qRound(value * 100));
            ui->leftX->setFormat(tr("%1").arg(QString::number(value, 'f', 2)));
        }
        break;
    }
        //buttonL3, axisLeftY
    case 4:{
        if(value * 100 >= 100){
            ui->leftY->setValue(100);
            ui->leftY->setFormat(tr("%1").arg(QString::number(1, 'f', 2)));
        }else if(value * 100 <= -100){
            ui->leftY->setValue(-100);
            ui->leftY->setFormat(tr("%1").arg(QString::number(-1, 'f', 2)));
        }else{
            ui->leftY->setValue(qRound(value * 100));
            ui->leftY->setFormat(tr("%1").arg(QString::number(value, 'f', 2)));
        }
        break;
    }
        //buttonR3, axisRightX
    case 5:{
        if(value * 100 >= 100){
            ui->rightX->setValue(100);
            ui->rightX->setFormat(tr("%1").arg(QString::number(1, 'f', 2)));
        }else if(value * 100 <= -100){
            ui->rightX->setValue(-100);
            ui->rightX->setFormat(tr("%1").arg(QString::number(-1, 'f', 2)));
        }else{
            ui->rightX->setValue(qRound(value * 100));
            ui->rightX->setFormat(tr("%1").arg(QString::number(value, 'f', 2)));
        }
        break;
    }
        //buttonR3, axisRightY
    case 6:{
        if(value * 100 >= 100){
            ui->rightY->setValue(100);
            ui->rightY->setFormat(tr("%1").arg(QString::number(1, 'f', 2)));
        }else if(value * 100 <= -100){
            ui->rightY->setValue(-100);
            ui->rightY->setFormat(tr("%1").arg(QString::number(-1, 'f', 2)));
        }else{
            ui->rightY->setValue(qRound(value * 100));
            ui->rightY->setFormat(tr("%1").arg(QString::number(value, 'f', 2)));
        }
        break;
    }
    default:
        break;
    }
    updateJoystick();
}

void JoystickTestWindow::updateJoystick() {
    m_command.A = m_gamepad->buttonA();
    m_command.B = m_gamepad->buttonB();
    m_command.X = m_gamepad->buttonX();
    m_command.Y = m_gamepad->buttonY();
    m_command.up = m_gamepad->buttonUp();
    m_command.down = m_gamepad->buttonDown();
    m_command.left = m_gamepad->buttonLeft();
    m_command.right = m_gamepad->buttonRight();
    m_command.back = m_gamepad->buttonSelect();
    m_command.start = m_gamepad->buttonStart();
    m_command.LB = m_gamepad->buttonL1();
    m_command.RB = m_gamepad->buttonR1();

    m_command.LT = m_gamepad->buttonL2();
    m_command.RT = m_gamepad->buttonR2();

    m_command.axisLeftXY[0] = m_gamepad->axisLeftX();
    m_command.axisLeftXY[1] = m_gamepad->axisLeftY();

    m_command.axisRightXY[0] = m_gamepad->axisRightX();
    m_command.axisRightXY[1] = m_gamepad->axisRightY();

    emit updated();
}