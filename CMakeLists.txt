cmake_minimum_required(VERSION 3.16)
project(GiochinoQT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automazioni Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Trova Qt5 o Qt6 in modo idiomatico
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Eseguibile (usa WIN32 su Windows per evitare la console nera)
if (WIN32)
  add_executable(Giochiamo WIN32
    Sources/main.cpp
    Headers/mainwindow.h        Sources/mainwindow.cpp
    Headers/homepage.h          Sources/homepage.cpp
    Headers/secondpage.h        Sources/secondpage.cpp
    Headers/optionspage.h       Sources/optionspage.cpp
    Headers/anagrampage.h       Sources/anagrampage.cpp
    Headers/hangmanpage.h       Sources/hangmanpage.cpp
    Headers/memoryconfigpage.h  Sources/memoryconfigpage.cpp
    Headers/memorypage.h        Sources/memorypage.cpp
    Headers/sudokupage.h        Sources/sudokupage.cpp
    Headers/words.h
  )
else()
  add_executable(Giochiamo
    Sources/main.cpp
    Headers/mainwindow.h        Sources/mainwindow.cpp
    Headers/homepage.h          Sources/homepage.cpp
    Headers/secondpage.h        Sources/secondpage.cpp
    Headers/optionspage.h       Sources/optionspage.cpp
    Headers/anagrampage.h       Sources/anagrampage.cpp
    Headers/hangmanpage.h       Sources/hangmanpage.cpp
    Headers/memoryconfigpage.h  Sources/memoryconfigpage.cpp
    Headers/memorypage.h        Sources/memorypage.cpp
    Headers/sudokupage.h        Sources/sudokupage.cpp
    Headers/words.h
  )
endif()

target_link_libraries(Giochiamo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Install per il packaging (usato dal workflow)
install(TARGETS Giochiamo
  RUNTIME DESTINATION .
  BUNDLE  DESTINATION .
)
