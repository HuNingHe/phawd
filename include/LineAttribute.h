/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-2-24
 * @version 0.2
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file LineAttribute.h
 * @brief definition of line attribute in phawd waveform displayer
 */
#pragma once
#include <QMetaType>
#include <utility>

struct LineAttribute {
    char name[24] = "";
    int dataIndex = -1;
    int width = -1;
    Qt::PenStyle style{};
    QColor color = QColor::Invalid;

    void init(const std::string& _name, int _dataIndex, int _width, const std::string& _style, QColor _color){
        if(_name.length() <= 24) {
            strcpy(name, _name.c_str());
        }else {
            qWarning("Error when init a LineAttribute : name size is bigger than 24!");
            return;
        }
        width = _width;
        dataIndex = _dataIndex;
        style = getPenStyleFromString(_style);
        color = std::move(_color);
    }

    void clear(){
        memset(name, 0, sizeof(name));
        width = -1;
        dataIndex = -1;
        style = Qt::PenStyle::NoPen;
        color = QColor::Invalid;
    }

    bool isInit(){
        if(strlen(name) <= 0){
            return false;
        }
        if(width <= 0 || dataIndex < 0){
            return false;
        }
        if(style == Qt::PenStyle::NoPen){
            return false;
        }
        if(color == QColor::Invalid){
            return false;
        }
        return true;
    }
    
    static Qt::PenStyle getPenStyleFromString(const std::string& style_){
        if (style_ == "DashDotDotLine"){
            return Qt::PenStyle::DashDotDotLine;
        }else if(style_ == "DashLine"){
            return Qt::PenStyle::DashLine;
        }else if(style_ == "DotLine"){
            return Qt::PenStyle::DotLine;
        }else if(style_ == "DashDotLine"){
            return Qt::PenStyle::DashDotLine;
        }else{
            return Qt::PenStyle::SolidLine;
        }
    }
};
// This custom type is declared here for signaling
Q_DECLARE_METATYPE(LineAttribute)
