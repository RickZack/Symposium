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
using namespace Symposium;

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
    if(newPrivilege==privilege::none)
        return;
    granted=newPrivilege;
    activePolicy=uriPolicy::activeAlways;
}

void uri::activateCount(int shares, privilege newPrivilege) {
    if(shares<=0)
    {
        activePolicy=uriPolicy::inactive;
        return;
    }
    activePolicy=uriPolicy::activeCount;
    sharesLeft=shares;
    granted=newPrivilege;
}

void uri::activateTimer(std::chrono::system_clock::time_point endTime, privilege newPrivilege) {
    activePolicy=uriPolicy::activeTimer;
    stopTime=endTime;
    granted=newPrivilege;
}

void uri::deactivate() {
    activePolicy=uriPolicy::inactive;
}

privilege uri::getShare(privilege requested) {
    //FIXME: valutate la possibilità di usare uno switch, diventa tutto più semplice e veloce
    if(activePolicy==uriPolicy::activeTimer)
    {
        std::chrono::system_clock::time_point ora = std::chrono::system_clock::now();
        if(ora<stopTime)
        {
            //FIXME: questo if è ripetuto uguale per tre volte
            // è possibile semplificare il metodo estraendo in una funzione
            if(requested<=granted)
                return requested;
            return granted;
        }
    }
    else if(activePolicy==uriPolicy::activeCount && sharesLeft>0)
        {
            sharesLeft--;
            if(requested<=granted)
                return requested;
            return granted;
        }
    else if(activePolicy==uriPolicy::activeAlways)
    {
        if(requested<=granted)
            return requested;
        return granted;
    }
    return privilege::none;
}
