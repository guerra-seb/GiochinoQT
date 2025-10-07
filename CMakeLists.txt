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
    src/main.cpp
    src/MainWindow.h src/MainWindow.cpp
    src/pages/homepage.h src/pages/homepage.cpp
    src/pages/secondpage.h src/pages/secondpage.cpp
    src/pages/optionspage.h src/pages/optionspage.cpp
    src/pages/anagrampage.h src/pages/anagrampage.cpp
    src/pages/hangmanpage.h src/pages/hangmanpage.cpp
    src/pages/memoryconfigpage.h src/pages/memoryconfigpage.cpp
    src/pages/memorypage.h src/pages/memorypage.cpp
    src/pages/sudokupage.h src/pages/sudokupage.cpp
    src/data/words.h
  )
else()
  add_executable(Giochiamo
    src/main.cpp
    src/MainWindow.h src/MainWindow.cpp
    src/pages/homepage.h src/pages/homepage.cpp
    src/pages/secondpage.h src/pages/secondpage.cpp
    src/pages/optionspage.h src/pages/optionspage.cpp
    src/pages/anagrampage.h src/pages/anagrampage.cpp
    src/pages/hangmanpage.h src/pages/hangmanpage.cpp
    src/pages/memoryconfigpage.h src/pages/memoryconfigpage.cpp
    src/pages/memorypage.h src/pages/memorypage.cpp
    src/pages/sudokupage.h src/pages/sudokupage.cpp
    src/data/words.h
  )
endif()

target_link_libraries(Giochiamo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Install per il packaging (usato dal workflow)
install(TARGETS Giochiamo
  RUNTIME DESTINATION .
  BUNDLE  DESTINATION .
)
