/*
 * Copyright (c) 2019 Riccardo Zaccone, Ksenia Del Conte Akimova, Alice Morano, Martina Bellissimo
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
 * File:   uri.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 17 Giugno 2019, 10.40
 */
#include "uri.h"

uri::uri(uriPolicy activePolicy) : activePolicy(activePolicy) {}

uriPolicy uri::getActivePolicy() const {
    return activePolicy;
}

int uri::getSharesLeft() const {
    return sharesLeft;
}

privilege uri::getGranted() const {
    return granted;
}

bool uri::operator==(const uri &rhs) const {
    return uriId == rhs.uriId &&
           activePolicy == rhs.activePolicy &&
           sharesLeft == rhs.sharesLeft &&
           stopTime == rhs.stopTime &&
           granted == rhs.granted;
}

bool uri::operator!=(const uri &rhs) const {
    return !(rhs == *this);
}

const privilege uri::getDefaultPrivilege() {
    return defaultPrivilege;
}

void uri::activateAlways(privilege newPrivilege) {
    //TODO:implement
}

void uri::activateCount(int shares, privilege newPrivilege) {
    //TODO:implement
}

void uri::activateTimer(std::chrono::system_clock::time_point endTime, privilege newPrivilege) {
    //TODO:implement
}

void uri::deactivate() {
    //TODO:implement
}

privilege uri::getShare(privilege requested) {
    //TODO:implement
    return granted;
}
