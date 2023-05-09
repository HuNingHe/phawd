#include <vector>
#include <memory>
#include <iostream>
#include "phawd/phawd.h"

using namespace phawd;
int main() {
    bool usingPhawd = true;
    size_t waveParamNum = 5;
    size_t controlParamNum = 5;
    auto shm = std::make_shared<SharedMemory<SharedParameters>>();
    auto paramCollection = std::make_shared<ParameterCollection>();
    try {
        shm->attach("demo", sizeof(SharedParameters) +
        (waveParamNum + controlParamNum) * sizeof(Parameter));
    } catch(std::runtime_error &err) {
        printf("%s\n", err.what());
        printf("Attach shared memory error, don't use phawd here \n");
        usingPhawd = false;
    }
    if (usingPhawd){
        std::string nameList[5] = {"pf", "pd", "ps64", "pvec3f", "pvec3d"};
        controlParamNum = shm->get()->numControlParams;
        shm->get()->connected += 1;
        shm->get()->numWaveParams = waveParamNum;
        for (int i = 0; i < waveParamNum; ++i) {
            shm->get()->parameters[controlParamNum +i].setName(nameList[i]);//Waveform parameters are appended after Control parameters
        }
        for (int i = 0; i < controlParamNum; ++i){
            paramCollection->addParameter(&shm->get()->parameters[i]);
        }
    }

    size_t iter = 0;
    float pf;
    double pd;
    long int ps64;
    std::vector<float> pvec3f = {0, 0, 0};
    std::vector<double> pvec3d = {0, 0, 0};

    while (iter < 500000 && usingPhawd) {
        pf = paramCollection->lookup("pf").getFloat();
        pd = paramCollection->lookup("pd").getDouble();
        ps64 = paramCollection->lookup("ps64").getS64();
        pvec3f = paramCollection->lookup("pvec3f").getVec3f();
        pvec3d = paramCollection->lookup("pvec3d").getVec3d();
        shm->get()->parameters[controlParamNum + 0].setValue(pf);
        shm->get()->parameters[controlParamNum + 1].setValue(pd);
        shm->get()->parameters[controlParamNum + 2].setValue(ps64);
        shm->get()->parameters[controlParamNum + 3].setValue(pvec3f);
        shm->get()->parameters[controlParamNum + 4].setValue(pvec3d);
        if (iter % 20 == 0){
            std::cout << "pf:" << pf<< std::endl;
            std::cout << "pd:" << pd<< std::endl;
            std::cout << "ps64:" << ps64<< std::endl;
            std::cout << "pvec3f:" << pvec3f[0] << " " << pvec3f[1] << " " << pvec3f[2] << std::endl;
            std::cout << "pvec3d:" << pvec3d[0] << " " << pvec3d[1] << " " << pvec3d[2] << std::endl;
        }
        iter++;
    }
    shm->get()->connected -= 1;
    return 0;
}
