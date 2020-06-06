/*
 * Copyright (c) 2019 Riccardo Zaccone, Ksenia Del Conte Akimova, Cristian Gianetto, Martina Bellissimo
 *
 * This file is part of Symposium.
 * Symposium is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symposium is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Symposium.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   Color.cpp
 * Project: Symposium
  * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 24 marzo 2020, 18:07
 */
#include <cmath>
#include <ios>
#include <sstream>
#include <boost/random/random_device.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>
#include "Color.h"

using namespace Symposium;

Color::Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b){}

Color::Color() : Color(0, 0, 0){}

template<class Archive>
void Color::serialize(Archive &ar, const unsigned int version){
    ar & r & g & b;
};
BOOST_CLASS_EXPORT(Symposium::Color)

colorGen::colorGen() {
    boost::random::random_device device;
    std::mt19937 generator(device());
    std::uniform_real_distribution<> dis(1.0, 2.0);
    token=dis(generator);
}

Color colorGen::operator()() {
    token+=grc;
    token=fmod(token, 1);
    return Color(hsv_to_rbg(token, 0.9, 0.95));
}

Color colorGen::hsv_to_rbg(double h, double s, double v) {
    double r,g,b;
    unsigned h_i=(h*6);
    double f=h*6-h_i;
    double p=v*(1-s);
    double q=v*(1-f*s);
    double t= v*(1-(1-f)*s);
    switch (h_i){
        case 0:
            r=v; g=t; b=p; break;
        case 1:
            r=q; g=v; b=p; break;
        case 2:
            r=p; g=v; b=t; break;
        case 3:
            r=p; g=q; b=v; break;
        case 4:
            r=t; g=p; b=v; break;
        case 5:
            r=v; g=p; b=q; break;
    }
    return {static_cast<uint8_t>(r * 256), static_cast<uint8_t>(g * 256), static_cast<uint8_t>(b * 256)};
}

std::string Color::rgb_hex_string() {
    unsigned r,g,b;
    std::ostringstream out;
    std::tie(r,g,b)=getRgb();
    out<<"#"<<std::hex<<r<<g<<b;
    return out.str();
}

std::tuple<uint8_t, uint8_t, uint8_t> Color::getRgb() const {
    return std::tuple<uint8_t, uint8_t, uint8_t>(r, g, b);
}

bool Color::operator==(const Color &rhs) const {
    return r == rhs.r &&
           g == rhs.g &&
           b == rhs.b;
}

bool Color::operator!=(const Color &rhs) const {
    return !(rhs == *this);
}
