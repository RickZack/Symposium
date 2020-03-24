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
#include "Color.h"

using namespace Symposium;

Color::Color(uint8_t r, uint8_t g, uint8_t b) : rgb_dec(std::make_tuple(r,g,b)){}

Color::Color() : Color(0, 0, 0){}

colorGen::colorGen() {
    //TODO: use C++11 or Boost library for randomness
    srand(time(NULL));
    token=rand();
}

Color colorGen::operator()() {
    token+=grc;
    token=fmod(token, 1);
    return Color(hsv_to_rbg(token, 0.5, 0.95));
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
    std::tie(r,g,b)=rgb_dec;
    out<<"#"<<std::hex<<r<<g<<b;
    return out.str();
}

bool Color::operator==(const Color &rhs) const {
    return rgb_dec == rhs.rgb_dec;
}

bool Color::operator!=(const Color &rhs) const {
    return !(rhs == *this);
}
