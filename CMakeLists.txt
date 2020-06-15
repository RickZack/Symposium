cmake_minimum_required(VERSION 3.14)
project(Symposium)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(WIN32)
    set(Boost_INCLUDE_DIR c:/mingw/include/)
    set(Boost_LIBRARY_DIR c:/mingw/lib/)
    include_directories(${Boost_INCLUDE_DIR})
    if (CMAKE_COMPILER_IS_MINGW)
        add_compile_options(-Wa,-mbig-obj)   # Note: new - fixes "file too big"
    endif (CMAKE_COMPILER_IS_MINGW)
    #set(CMAKE_PREFIX_PATH "C:/Qt/5.12.2/minGW73_64/lib/cmake")
    #set(CMAKE_PREFIX_PATH "D:/akimo/app/Qt/5.12.2/mingw73_64/lib/cmake")
    set(CMAKE_PREFIX_PATH "C:/Qt/5.12.2/mingw73_64/lib/cmake")
else()
    set(CMAKE_PREFIX_PATH "/home/riccardo/Qt/5.12.3/gcc_64/lib/cmake")
endif(WIN32)

# Global compiler options
add_compile_options(-Werror=conversion-null)
# Subdirectory of Google Test suite and path of header files to include in SymposiumTest target
add_subdirectory(lib/googletest-master)
include_directories(SymposiumTest lib/googletest-master/googletest/include)
include_directories(SymposiumTest lib/googletest-master/googlemock/include)

# Path of header files to include in SymposiumGUI target
include_directories(SymposiumGUI SymposiumServer GUI/SymposiumGui)
include_directories(SymposiumGUI SymposiumServer GUI/SymposiumGui/Dispatcher)

#Packages required by SymposiumGUI
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

# Enable MOC, UIC and RCC for SymposiumGUI target
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Library for sha256 algorithm in password handling
include_directories(lib/hash)

# Dependency from Boost library for serialization and generator of random numbers
link_libraries(boost_serialization boost_wserialization boost_random)

# List of source files for testing
FILE(GLOB TEST_FILES ${CMAKE_SOURCE_DIR}/tests/*.cpp)

# List of sources and headers for the core project
FILE(GLOB CORE_FILES ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/*.h)
list(REMOVE_ITEM CORE_FILES ${CMAKE_SOURCE_DIR}/main.cpp)

# List of sources and headers for sha256 library
FILE(GLOB SHA256_LIB ${CMAKE_SOURCE_DIR}/lib/hash/*.cpp ${CMAKE_SOURCE_DIR}/lib/hash/*.h)

# List of sources and headers for clientDispatcher
FILE(GLOB CLIENT_FILES ${CMAKE_SOURCE_DIR}/GUI/SymposiumGui/Dispatcher/client*.cpp ${CMAKE_SOURCE_DIR}/GUI/SymposiumGui/Dispatcher/client*.h)
FILE(GLOB SYMPOSIUM_CLIENT ${CLIENT_FILES} ${CORE_FILES} ${SHA256_LIB})
list(REMOVE_ITEM SYMPOSIUM_CLIENT ${CMAKE_SOURCE_DIR}/SymServer.cpp ${CMAKE_SOURCE_DIR}/SymServer.h)

# List of sources and headers for serverDispatcher
FILE(GLOB SERVER_FILES ${CMAKE_SOURCE_DIR}/GUI/SymposiumGui/Dispatcher/server*.cpp ${CMAKE_SOURCE_DIR}/GUI/SymposiumGui/Dispatcher/server*.h)
FILE(GLOB SYMPOSIUM_SERVER ${SERVER_FILES} ${CORE_FILES} ${SHA256_LIB})
list(REMOVE_ITEM SYMPOSIUM_SERVER ${CMAKE_SOURCE_DIR}/SymClient.cpp ${CMAKE_SOURCE_DIR}/SymClient.h)

# Files and headers for SymposiumTest target: this is the executable that will run the tests for Symposium
add_executable(SymposiumTest main.cpp ${CORE_FILES} ${SHA256_LIB} ${TEST_FILES})

# Files and headers for SymposiumGUI target: this is the executable that will run the client with the GUI
add_executable(SymposiumGUI GUI/SymposiumGui/main.cpp ${SYMPOSIUM_CLIENT}
        GUI/SymposiumGui/about.cpp GUI/SymposiumGui/successlinks.cpp GUI/SymposiumGui/successlinks.h
        GUI/SymposiumGui/successlinks.ui GUI/SymposiumGui/about.h GUI/SymposiumGui/about.ui
        GUI/SymposiumGui/waiting.ui GUI/SymposiumGui/waiting.cpp GUI/SymposiumGui/waiting.h
        GUI/SymposiumGui/activealwayslink.cpp GUI/SymposiumGui/activealwayslink.h GUI/SymposiumGui/activealwayslink.ui
        GUI/SymposiumGui/activecounterlink.cpp GUI/SymposiumGui/activecounterlink.h GUI/SymposiumGui/activecounterlink.ui
        GUI/SymposiumGui/activenonlink.cpp GUI/SymposiumGui/activenonlink.h GUI/SymposiumGui/activenonlink.ui
        GUI/SymposiumGui/activetimerlink.cpp GUI/SymposiumGui/activetimerlink.h GUI/SymposiumGui/activetimerlink.ui
        GUI/SymposiumGui/alluser.cpp GUI/SymposiumGui/alluser.h GUI/SymposiumGui/alluser.ui
        GUI/SymposiumGui/avatarimg.qrc GUI/SymposiumGui/buttonhoverwatcher.cpp GUI/SymposiumGui/buttonhoverwatcher.h
        GUI/SymposiumGui/changeuserinfo.cpp GUI/SymposiumGui/changeuserinfo.h GUI/SymposiumGui/changeuserinfo.ui
        GUI/SymposiumGui/choosedir.cpp GUI/SymposiumGui/choosedir.h GUI/SymposiumGui/choosedir.ui
        GUI/SymposiumGui/deleteaccount.cpp GUI/SymposiumGui/deleteaccount.h GUI/SymposiumGui/deleteaccount.ui
        GUI/SymposiumGui/directory.cpp GUI/SymposiumGui/directory.h GUI/SymposiumGui/directory.ui
        GUI/SymposiumGui/errorconnection.cpp GUI/SymposiumGui/errorconnection.h GUI/SymposiumGui/errorconnection.ui
        GUI/SymposiumGui/errorlogout.cpp GUI/SymposiumGui/errorlogout.h GUI/SymposiumGui/errorlogout.ui
        GUI/SymposiumGui/exit.cpp GUI/SymposiumGui/exit.h GUI/SymposiumGui/exit.ui
        GUI/SymposiumGui/folder1.cpp GUI/SymposiumGui/folder1.h GUI/SymposiumGui/folder1.ui
        GUI/SymposiumGui/home.cpp GUI/SymposiumGui/home.h GUI/SymposiumGui/home.ui
        GUI/SymposiumGui/icon.cpp GUI/SymposiumGui/icon.h GUI/SymposiumGui/icon.ui
        GUI/SymposiumGui/inserturi.cpp GUI/SymposiumGui/inserturi.h GUI/SymposiumGui/inserturi.ui
        GUI/SymposiumGui/mainwindow.cpp GUI/SymposiumGui/mainwindow.h GUI/SymposiumGui/mainwindow.ui
        GUI/SymposiumGui/newfolderwindow.cpp GUI/SymposiumGui/newfolderwindow.h GUI/SymposiumGui/newfolderwindow.ui
        GUI/SymposiumGui/notepad.cpp GUI/SymposiumGui/notepad.h GUI/SymposiumGui/notepad.ui
        GUI/SymposiumGui/notification.cpp GUI/SymposiumGui/notification.h GUI/SymposiumGui/notification.ui
        GUI/SymposiumGui/onlineusers.cpp GUI/SymposiumGui/onlineusers.h GUI/SymposiumGui/onlineusers.ui
        GUI/SymposiumGui/qtexteditlabels.cpp GUI/SymposiumGui/qtexteditlabels.h
        GUI/SymposiumGui/resources.qrc GUI/SymposiumGui/sigin.cpp GUI/SymposiumGui/sigin.h GUI/SymposiumGui/sigin.ui
        GUI/SymposiumGui/signup.cpp GUI/SymposiumGui/signup.h GUI/SymposiumGui/signup.ui
        GUI/SymposiumGui/textedit.cpp GUI/SymposiumGui/textedit.h
        GUI/SymposiumGui/unsuccessdeleteaccount.cpp GUI/SymposiumGui/unsuccessdeleteaccount.h GUI/SymposiumGui/unsuccessdeleteaccount.ui
        GUI/SymposiumGui/winmanager.cpp GUI/SymposiumGui/winmanager.h
        GUI/SymposiumGui/symwininterface.h GUI/SymposiumGui/symwininterface.cpp
        GUI/SymposiumGui/onoff_networkinteraction.h
        GUI/SymposiumGui/option.h GUI/SymposiumGui/option.cpp GUI/SymposiumGui/option.ui
        )

# Files and headers for SymposiumServer target: this is the executable that will run the server
add_executable(SymposiumServer GUI/SymposiumGui/Dispatcher/mainServer.cpp ${SYMPOSIUM_SERVER} GUI/SymposiumGui/onoff_networkinteraction.h
        GUI/SymposiumGui/Dispatcher/gui_symserver.h GUI/SymposiumGui/Dispatcher/gui_symserver.ui GUI/SymposiumGui/Dispatcher/gui_symserver.cpp
        GUI/SymposiumGui/logoserver.qrc)

# Linking Google Test libraries for SymposiumTest target
target_link_libraries(SymposiumTest gtest gtest_main gmock gmock_main)

# Linking Qt5 libraries for SymposiumGUI and SymposiumServer
target_link_libraries(SymposiumGUI Qt5::Widgets Qt5::Network Qt5::Core Qt5::PrintSupport pthread)
target_link_libraries(SymposiumServer Qt5::Widgets Qt5::Network Qt5::Core Qt5::PrintSupport pthread)