#include <vector>
#include <memory>
#include <iostream>
#include "phawd/phawd.h"

using namespace phawd;
int main() {
    bool usingPhawd = true;
    size_t iter = 0;
    size_t waveParamNum = 5;
    size_t controlParamNum = 5;
    size_t sendSize = sizeof(SocketToPhawd) + waveParamNum * sizeof(Parameter);
    size_t readSize = sizeof(SocketFromPhawd) + controlParamNum * sizeof(Parameter);
    auto socket = std::make_shared<SocketConnect<SocketToPhawd, SocketFromPhawd>>();

    try {
        socket->Init(sendSize, readSize);
        socket->connectToServer("127.0.0.1", 5230);
    } catch(std::runtime_error &err) {
        printf("%s\n", err.what());
        printf("Connect server error, don't use phawd here \n");
        usingPhawd = false;
    }

    if (usingPhawd) {
        std::string nameList[5] = {"pf", "pd", "ps64", "pvec3f", "pvec3d"};
        auto send_data = socket->getSend();
        send_data->numWaveParams = waveParamNum;
        for (int i = 0; i < waveParamNum; ++i) {
            send_data->parameters[i].setName(nameList[i]);
        }
        float f_value = 1.456;
        double d_value = 3.1516926;
        long s64_value = 12;
        std::vector<float> vec3f_value{1, 2 , 3};
        std::vector<double> vec3d_value{3, 2, 1};

        send_data->parameters[0].setValue(f_value);
        send_data->parameters[1].setValue(d_value);
        send_data->parameters[2].setValue(s64_value);
        send_data->parameters[3].setValue(vec3f_value);
        send_data->parameters[4].setValue(vec3d_value);
    }

    while (iter < 500000 && usingPhawd){
        iter++;
        socket->Send();
        int read_count = socket->Read();

        if(read_count > 0) { // This is important
            float pf = socket->getRead()->parameters[0].getFloat();
            double pd = socket->getRead()->parameters[1].getDouble();
            long ps64 = socket->getRead()->parameters[2].getS64();
            std::vector<float> pvec3f = socket->getRead()->parameters[3].getVec3f();
            std::vector<double> pvec3d = socket->getRead()->parameters[4].getVec3d();

            std::cout << "pf:" << pf<< std::endl;
            std::cout << "pd:" << pd<< std::endl;
            std::cout << "ps64:" << ps64<< std::endl;
            std::cout << "pvec3f:" << pvec3f[0] << " " << pvec3f[1] << " " << pvec3f[2] << std::endl;
            std::cout << "pvec3d:" << pvec3d[0] << " " << pvec3d[1] << " " << pvec3d[2] << std::endl;
        }
    }
    return 0;
}
