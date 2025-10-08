cmake_minimum_required(VERSION 3.16)
project(GiochinoQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt automazioni
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Trova Qt5/Qt6 in modo idiomatico
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Elenco file: sono TUTTI nella radice (niente src/)
set(APP_SOURCES
  main.cpp
  mainwindow.h mainwindow.cpp
  homepage.h homepage.cpp
  secondpage.h secondpage.cpp
  optionspage.h optionspage.cpp
  anagrampage.h anagrampage.cpp
  hangmanpage.h hangmanpage.cpp
  memoryconfigpage.h memoryconfigpage.cpp
  memorypage.h memorypage.cpp
  sudokupage.h sudokupage.cpp
  game2048page.h game2048page.cpp
  words.h
  resources.qrc
)

# Su Windows evita la console
if (WIN32)
  add_executable(Giochiamo WIN32 ${APP_SOURCES})
else()
  add_executable(Giochiamo ${APP_SOURCES})
endif()

target_link_libraries(Giochiamo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Install per il packaging (usato dal workflow)
install(TARGETS Giochiamo
  RUNTIME DESTINATION .
  BUNDLE  DESTINATION .
)
