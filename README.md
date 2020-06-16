<img src="./assets/logo.jpg" width="600">

# Symposium: the collaboration place

## Summary
Symposium is an free, real-time, collaborative text editor written in C++ language. Like in the ancient Greece, Symposium allows you to collaborate and exchange ideas with other people in a environment that suits you.

This editor supports the basic functions that a rich text editor usually offers. It's possible create lists, decide the alignment of a paragraph, edit font and style of each font and use buttons and shortcuts for common cut-copy-paste operations. It is also possible to save the document in format pdf.

One of Symposium's strengths is the sophisticated management of sharing options and privileges that each user has over a document. The following modes are supported sharing:
+ **Sharing without limits**: a link to the document is activated and remains valid as long as is not explicitly deactivated;

+ **Timer sharing**: the link is activated until a certain time, past the
which other users, even if in possession of the link, will not be able to add the document between those they have access to.

+ **Counter-controlled sharing**: in this mode only predetermined number of users who will take access to the document will be accepted.

Each user can have a different privilege on a document, and the choice of which privilege grant can be decided by the owner when sharing the document. The privileges supported are:
+ **Owner**: by default, the creator of the document is the first owner. An owner can edit the document in all its parts, including sharing options and the privileges of others users;

+ **Modifier**: can modify every aspect of the document, but not the sharing options neither user privileges associated with the document;

+ **Reader**: can only view the document, user cannot modify it. Continue to see real time changes and other users' cursors move around.

It's developed on purpose for an academic project, but it is intended to remain free for your contribution. Symposium is a client-server software that uses **Conflict-Free Replicated Data Types** (CRDT) to make sure all users stay in-sync.


## Getting Started

To build and run the project make sure you have the necessary components present in your system.

If you want to proceed with [CMakeList file](CMakeLists.txt), there are 2 main targets: one for the server (SymposiumServer) and one for the client (SymposiumGui).

If you want to proceed with .pro file, there are 2 project in the [directory](GUI/SymposiumGui): one for the server ([SymposiumGui_Server.pro](GUI/SymposiumGui/SymposiumGui_Server.pro)) and one for the client ([SymposiumGui.pro](GUI/SymposiumGui/SymposiumGui.pro))

### Prerequisites

You need the following components to be able to install and run the program:
+ boost libraries;
+ Qt libraries (version 5.12.3);
+ C++ environment (c++17);

In particular we use:
+ [Qt Creator](https://www.qt.io/download);
+ [Clion](https://www.jetbrains.com/clion/download/#section=windows);
+ [Mingw Distro compiler for boost serialization](https://nuwen.net/mingw.html)

## Authors

* **Riccardo Zaccone** - [RickZack](https://github.com/RickZack)
* **Ksenia Del Conte Akimova** - [KseniaDelConte](https://github.com/KseniaDelConte)
* **Cristian Gianetto** - [addocm](https://github.com/addocm)
* **Martina Bellissimo** - [martinaBellissimo](https://github.com/martinaBellissimo)

## License
Symposium is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. See the [COPYING](COPYING) file for details.
