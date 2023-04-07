/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-2-24
 * @version 0.3
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file SharedParameter.h
 * @brief definition of parameters stored in shared memory
 */

#pragma once

#include "phawd/Parameter.h"

namespace phawd {
struct PHAWD_DLLAPI GamepadCommand {
    bool down, left, up, right, LB, RB,
         back, start, A, B, X, Y;

    double axisLeftXY[2], axisRightXY[2];
    double LT, RT;

    void init();

    /*!
     * Construct a gamepad and set to zero.
     */
    GamepadCommand();
};

class PHAWD_DLLAPI SharedParameters {
public:
    int connected;                      // Number of connected objects, and whenever an object is connected, the value should manually increment 1
    size_t numControlParams;            // Number of control parameters
    size_t numWaveParams;               // Number of waveform parameters
    phawd::GamepadCommand gameCommand;  // Commands from joystick
    Parameter parameters[];	            // (control parameters)[0, numControlParams) and (waveform parameters)[numControlParams, numControlParams + numWaveParams)

private:
    SharedParameters();

    SharedParameters(const SharedParameters &p);

    SharedParameters(SharedParameters &&p) noexcept;
public:
    SharedParameters& operator=(const SharedParameters &p);

    SharedParameters& operator=(SharedParameters &&p) noexcept;

    void collectParameters(ParameterCollection *pc);

    static SharedParameters* create(int num_control_params, int num_wave_params);

    static void destroy(SharedParameters* p);
};

class PHAWD_DLLAPI SocketFromPhawd {
public:
    size_t numControlParams;
    GamepadCommand gameCommand;
    Parameter parameters[];
private:
    SocketFromPhawd();

    SocketFromPhawd(const SocketFromPhawd &p);

    SocketFromPhawd(SocketFromPhawd &&p) noexcept;
public:
    SocketFromPhawd& operator=(const SocketFromPhawd &p);

    SocketFromPhawd& operator=(SocketFromPhawd &&p) noexcept;

    static SocketFromPhawd* create(int num_params);

    static void destroy(SocketFromPhawd* p);

    void collectParameters(ParameterCollection *pc);
};

class PHAWD_DLLAPI SocketToPhawd {
public:
    size_t numWaveParams;
    Parameter parameters[];
private:
    SocketToPhawd();

    SocketToPhawd(const SocketToPhawd &p);

    SocketToPhawd(SocketToPhawd &&p) noexcept;

public:
    SocketToPhawd& operator=(const SocketToPhawd &p);

    SocketToPhawd& operator=(SocketToPhawd &&p) noexcept;

    static SocketToPhawd* create(int num_params);

    static void destroy(SocketToPhawd* p);

    void collectParameters(ParameterCollection *pc);
};
}