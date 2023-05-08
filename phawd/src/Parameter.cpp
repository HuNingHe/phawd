/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-2-24
 * @version 0.3
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file Parameters.cpp
 * @brief definition of parameter in phawd and parameters collection
 */

#include "phawd/Parameter.h"
#include <algorithm>
using namespace phawd;

/*!
 * Check if the unordered map contain the given element
 */
template<typename T1, typename T2>
bool mapContains(const std::map<T1, T2> &set, T1 key) {
    return set.find(key) != set.end();
}

void ParameterValue::init() {
    d = 0;
    std::memset(this, 0, sizeof(ParameterValue));
}

ParameterValue::ParameterValue() {
    d = 0;
    std::memset(this, 0, sizeof(ParameterValue));
}

std::string phawd::ParameterKindToString(phawd::ParameterKind kind) {
    switch (kind){
        case ParameterKind::FLOAT:
            return {"FLOAT"};
        case ParameterKind::DOUBLE:
            return {"DOUBLE"};
        case ParameterKind::S64:
            return {"S64"};
        case ParameterKind::VEC3_FLOAT:
            return {"VEC3_FLOAT"};
        case ParameterKind::VEC3_DOUBLE:
            return {"VEC3_DOUBLE"};
        default:
            return {};
    }
}

ParameterKind phawd::getParameterKindFromString(const std::string& str){
    if(str == "FLOAT") return ParameterKind::FLOAT;
    if(str == "DOUBLE") return ParameterKind::DOUBLE;
    if(str == "S64") return ParameterKind::S64;
    if(str == "VEC3_FLOAT") return ParameterKind::VEC3_FLOAT;
    if(str == "VEC3_DOUBLE") return ParameterKind::VEC3_DOUBLE;
    return ParameterKind::DOUBLE;
}

Parameter::Parameter() {
    std::memset(m_name, 0, sizeof(m_name));
    std::memset(&m_value, 0, sizeof(ParameterValue));
    m_set = false;
    m_kind = ParameterKind::DOUBLE;
}

Parameter::Parameter(const std::string& name, ParameterKind &kind) {
    std::memset(&m_value, 0, sizeof(ParameterValue));
    if(!this->setName(name)) return;
    m_set = true;
    m_kind = kind;
}

Parameter::Parameter(const std::string& name, ParameterKind &kind, ParameterValue &value) {
    if(!this->setName(name)) return;
    std::memcpy(&m_value, &value, sizeof(ParameterValue));
    m_kind = kind;
    m_set = true;
}

Parameter::Parameter(const std::string& name, float value) {
    if(!this->setName(name)) return;
    m_set = true;
    m_value.f = value;
    m_kind = ParameterKind::FLOAT;
}

Parameter::Parameter(const std::string& name, double value) {
    if(!this->setName(name)) return;
    m_set = true;
    m_value.d = value;
    m_kind = ParameterKind::DOUBLE;
}

Parameter::Parameter(const std::string& name, long int value) {
    if(!this->setName(name)) return;
    m_set = true;
    m_value.i = value;
    m_kind = ParameterKind::S64;
}

Parameter::Parameter(const std::string& name, const double* value) : Parameter(){
    if (value == nullptr) return;
    if(!this->setName(name)) return;
    for (int j = 0; j < 3; ++j) {
        m_value.vec3d[j] = value[j];
    }
    m_set = true;
    m_kind = ParameterKind::VEC3_DOUBLE;
}

Parameter::Parameter(const std::string& name, const float* value) : Parameter(){
    if (value == nullptr) return;
    if(!this->setName(name)) return;
    for (int j = 0; j < 3; ++j) {
        m_value.vec3f[j] = value[j];
    }
    m_set = true;
    m_kind = ParameterKind::VEC3_FLOAT;
}

Parameter::Parameter(const std::string& name, const std::vector<double>& value) : Parameter(){
    if(!this->setName(name)) return;
    auto range = value.size() > 3? 3 : value.size();
    for (size_t j = 0; j < range; ++j) {
        m_value.vec3d[j] = value[j];
    }
    m_set = true;
    m_kind = ParameterKind::VEC3_DOUBLE;
}

Parameter::Parameter(const std::string& name, const std::vector<float>& value) : Parameter(){
    if(!this->setName(name)) return;
    auto range = value.size() > 3? 3 : value.size();
    for (size_t j = 0; j < range; ++j) {
        m_value.vec3f[j] = value[j];
    }
    m_set = true;
    m_kind = ParameterKind::VEC3_FLOAT;
}

void Parameter::setValueKind(ParameterKind kind) {
    m_kind = kind;
}

ParameterKind Parameter::getValueKind() {
    return m_kind;
}

void Parameter::setValue(float value) {
    m_kind = ParameterKind::FLOAT;
    m_value.f = value;
    m_set = true;
}

void Parameter::setValue(double value){
    m_kind = ParameterKind::DOUBLE;
    m_value.d = value;
    m_set = true;
}

void Parameter::setValue(long int value){
    m_kind = ParameterKind::S64;
    m_value.i = value;
    m_set = true;
}

void Parameter::setValue(const double* value) {
    if(value == nullptr) return;
    for (int j = 0; j < 3; ++j) {
        m_value.vec3d[j] = value[j];
    }
    m_kind = ParameterKind::VEC3_DOUBLE;
    m_set = true;
}

void Parameter::setValue(const float* value) {
    if(value == nullptr) return;
    for (int j = 0; j < 3; ++j) {
        m_value.vec3f[j] = value[j];
    }
    m_kind = ParameterKind::VEC3_FLOAT;
    m_set = true;
}

void Parameter::setValue(const std::vector<double>& value) {
    auto range = value.size() > 3? 3 : value.size();
    for (size_t j = 0; j < range; ++j) {
        m_value.vec3d[j] = value[j];
    }
    m_kind = ParameterKind::VEC3_DOUBLE;
    m_set = true;
}

void Parameter::setValue(const std::vector<float>& value) {
    auto range = value.size() > 3? 3 : value.size();
    for (size_t j = 0; j < range; ++j) {
        m_value.vec3f[j] = value[j];
    }
    m_kind = ParameterKind::VEC3_FLOAT;
    m_set = true;
}

void Parameter::setValue(ParameterKind kind, const ParameterValue& value) {
    if(m_kind != kind) {
        printf("[ Parameter]: The parameter type is different with setting type.");
        throw std::runtime_error(" parameter type mismatch in set");
    }
    switch(m_kind) {
        case ParameterKind::FLOAT:
            m_value.f = value.f;
            break;
        case ParameterKind::DOUBLE:
            m_value.d = value.d;
            break;
        case ParameterKind::S64:
            m_value.i = value.i;
            break;
        case ParameterKind::VEC3_DOUBLE:{
            for(int i = 0; i < 3; i++){
                m_value.vec3d[i] = value.vec3d[i];
            }
            break;
        }
        case ParameterKind::VEC3_FLOAT:{
            for(int i = 0; i < 3; i++){
                m_value.vec3f[i] = value.vec3f[i];
            }
            break;
        }
        default:return;
    }
    m_set = true;
}

/*!
* Get the value of a  parameter.  Does type checking - you must provide
* the correct type.
* @param kind : the kind of the  parameter
* @return the value of the  parameter
*/
ParameterValue Parameter::getValue(ParameterKind kind) {
    ParameterValue value;
    if (kind != m_kind) {
        printf("[ Parameter]: The parameter type is different with setting type.");
        throw std::runtime_error(" parameter type mismatch in get");
    }
    switch (m_kind) {
        case ParameterKind::FLOAT:
            value.f = m_value.f;
            break;
        case ParameterKind::DOUBLE:
            value.d = m_value.d;
            break;
        case ParameterKind::S64:
            value.i = m_value.i;
            break;
        case ParameterKind::VEC3_FLOAT:{
            value.vec3f[0] = m_value.vec3f[0];
            value.vec3f[1] = m_value.vec3f[1];
            value.vec3f[2] = m_value.vec3f[2];
            break;
        }
        case ParameterKind::VEC3_DOUBLE:{
            value.vec3d[0] = m_value.vec3d[0];
            value.vec3d[1] = m_value.vec3d[1];
            value.vec3d[2] = m_value.vec3d[2];
            break;
        }
        default:
        throw std::runtime_error(" parameter invalid kind in get");
    }
    return value;
}

template<typename T>
T Parameter::getValue() {
    if(std::is_same<T, float>::value && m_kind == ParameterKind::FLOAT){
        return m_value.f;
    }
    if (std::is_same<T, double>::value && m_kind == ParameterKind::DOUBLE){
        return m_value.d;
    }
    if (std::is_same<T, long int>::value && m_kind == ParameterKind::S64){
        return m_value.i;
    }
    if(std::is_same<T, std::vector<float>>::value && m_kind == ParameterKind::VEC3_FLOAT){
        return {m_value.vec3f[0], m_value.vec3f[1], m_value.vec3f[2]};
    }
    if (std::is_same<T, std::vector<float>>::value && m_kind == ParameterKind::VEC3_DOUBLE){
        return {m_value.vec3d[0], m_value.vec3d[1], m_value.vec3d[2]};
    }
    if(std::is_same<T, float*>::value && m_kind == ParameterKind::VEC3_FLOAT){
        static float result[3] = {0, 0, 0};
        for (int i = 0; i < 3; ++i) {
            result[i] = m_value.vec3f[i];
        }
        return result;
    }
    if (std::is_same<T, double*>::value && m_kind == ParameterKind::VEC3_DOUBLE){
        static double result[3] = {0, 0, 0};
        for (int i = 0; i < 3; ++i) {
            result[i] = m_value.vec3d[i];
        }
        return result;
    }
    throw std::runtime_error("Error Type When using getValue in ControlParameterValue");
}

double Parameter::getDouble() {
    if (m_kind != ParameterKind::DOUBLE){
        throw std::runtime_error("Parameter::getDouble(): type error");
    }
    return m_value.d;
}

float Parameter::getFloat() {
    if (m_kind != ParameterKind::FLOAT){
        throw std::runtime_error("Parameter::getDouble(): type error");
    }
    return m_value.f;
}

long int Parameter::getS64(){
    if (m_kind != ParameterKind::S64){
        throw std::runtime_error("Parameter::getS64(): type error");
    }
    return m_value.i;
}

bool Parameter::setName(const std::string& name) {
    if(name.length() > 16 || name.empty()){
        printf("[Parameter]: The parameter name size is invalid when construct it. should be in range[1, 16]");
        return false;
    } else {
        strcpy(m_name, name.c_str());
        return true;
    }
}

std::string Parameter::getName() {
    return m_name;
}

std::vector<double> Parameter::getVec3d() {
    if (m_kind != ParameterKind::VEC3_DOUBLE){
        throw std::runtime_error("Parameter::getVec3d(): type error");
    }
    return {m_value.vec3d[0], m_value.vec3d[1], m_value.vec3d[2]};
}

std::vector<float> Parameter::getVec3f() {
    if (m_kind != ParameterKind::VEC3_FLOAT){
        throw std::runtime_error("Parameter::getVec3f(): type error");
    }
    return {m_value.vec3f[0], m_value.vec3f[1], m_value.vec3f[2]};
}

double Parameter::getFromVec3dByIndex(int idx) {
    if (m_kind != ParameterKind::VEC3_DOUBLE || idx < 0){
        throw std::runtime_error("Parameter::getFromVec3dByIndex(): type error or index negative");
    }
    return m_value.vec3d[idx];
}

float Parameter::getFromVec3fByIndex(int idx) {
    if (m_kind != ParameterKind::VEC3_FLOAT || idx < 0){
        throw std::runtime_error("Parameter::getFromVec3fByIndex(): type error or index negative");
    }
    return m_value.vec3f[idx];
}

Parameter::Parameter(const Parameter &parameter) : Parameter() {
    *this = parameter;
}

Parameter::Parameter(Parameter &&parameter) noexcept : Parameter() {
    *this = std::move(parameter);
}

Parameter &Parameter::operator=(const Parameter &parameter) {
    if (this != &parameter) {
        m_set = parameter.m_set;
        m_kind = parameter.m_kind;
        std::memcpy(m_name, parameter.m_name, sizeof(m_name));
        std::memcpy(&m_value, &parameter.m_value, sizeof(ParameterValue));
    }
    return *this;
}

Parameter &Parameter::operator=(Parameter &&parameter) noexcept{
    if (this != &parameter) {
        m_set = parameter.m_set;
        m_kind = parameter.m_kind;
        std::memcpy(m_name, parameter.m_name, sizeof(m_name));
        std::memcpy(&m_value, &parameter.m_value, sizeof(ParameterValue));
    }
    return *this;
}

bool& Parameter::isSet(){
    return m_set;
}

void Parameter::set(bool set){
    m_set = set;
}

void ParameterCollection::addParameter(Parameter* param) {
    std::string name = param->getName();
    if (mapContains(m_map, name)) {
        // printf("[ERROR] ParameterCollection %s: tried to add parameter %s twice!\n", m_name.c_str(), name.c_str());
        return;
    }
    m_map[name] = param;
}

Parameter& ParameterCollection::lookup(const std::string& name) {
    if(mapContains(m_map, name)){
        return *m_map[name];
    } else {
        throw std::runtime_error(" parameter " + name + " wasn't found in parameter collection " + m_name);
    }
}

bool ParameterCollection::checkIfAllSet() {
    return std::all_of(m_map.begin(), m_map.end(), [](const std::pair<std::string, Parameter*>& kv){return kv.second->isSet();});
}

void ParameterCollection::clearAllSet() {
    for (auto& kv : m_map) {
        kv.second->set(false);
    }
}

void ParameterCollection::clearAllParameters() {
    m_map.clear();
}
