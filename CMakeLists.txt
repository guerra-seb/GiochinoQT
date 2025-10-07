cmake_minimum_required(VERSION 3.16)
project(GiochinoQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Per segnali/slot e (se serviranno .ui/.qrc) automazione
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Prova Qt6, altrimenti Qt5
find_package(Qt6 QUIET COMPONENTS Widgets)
if (Qt6_FOUND)
  set(QT_PACKAGE Qt6)
else()
  find_package(Qt5 REQUIRED COMPONENTS Widgets)
  set(QT_PACKAGE Qt5)
endif()

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

target_link_libraries(Giochiamo PRIVATE ${QT_PACKAGE}::Widgets)

# alla fine del file
install(TARGETS Giochiamo
    BUNDLE DESTINATION .        # macOS .app
    RUNTIME DESTINATION .       # Windows/Linux exe
)
