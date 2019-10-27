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
 * File:   Symposium.h
 * Project: Symposium
 * Authors:
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 27 ottobre 2019, 14:10
 */
#ifndef SYMPOSIUM_SYMPOSIUM_H
#define SYMPOSIUM_SYMPOSIUM_H

/**
 * @brief Contains the forward declaration of classes used in the original Symposium software.
 *
 * This namespace is used as a common place where to find the names used in the original project. It's intended to
 * avoid conflicts that can be made if you want to user other classes in the context of Symposium, and to allow you
 * to extend the project without the need to choose names unique in the global context
 */
namespace Symposium{
    class SymServer;
    class SymClient;
    class message;
    class symbolMessage;
    class user;
    class document;
    class AccessStrategy;
    class symbol;
    class uri;

    class filesystem;
    class directory;
    class file;
    class symlink;

    enum class msgOutcome:char;
    enum class msgType;
    enum class privilege:char;
}





#endif //SYMPOSIUM_SYMPOSIUM_H
