/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-3-13
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file JoystickTestWindow.h
 * @brief check joystick
 */

#pragma once
#include <QWidget>
#include <QGamepad>
#include <QGamepadManager>
#include "phawd/SharedParameter.h"
#include "../ui_include/ui_joystick_test.h"

namespace Ui { class JoystickTestWindow; }

class JoystickTestWindow : public QWidget{
    Q_OBJECT
public:
    explicit JoystickTestWindow(QWidget *parent = nullptr);
    ~JoystickTestWindow() override;
    void updateJoystick();
    phawd::GamepadCommand m_command;
signals:
    void updated();
private:
    Ui::JoystickTestWindow *ui;
    QGamepad *m_gamepad;
    QGamepadManager *m_manager;
    void showCurrentBtnName(bool pressed, const QString& name);
    void showBtnValue(int btnName, double value);
};