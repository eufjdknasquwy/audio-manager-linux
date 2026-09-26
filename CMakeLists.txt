cmake_minimum_required(VERSION 3.16)
project(audio-manager CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Включить compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# gtkmm3
find_package(PkgConfig REQUIRED)
# pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)
pkg_check_modules(PULSE REQUIRED IMPORTED_TARGET libpulse)

set(SOURCES
    src/main.cpp

    src/gui/AudioWindow.cpp
    src/gui/CssHelper.cpp
    src/gui/MicrophoneWindow.cpp

    src/modules/AudioManager.cpp
    src/modules/MicrophoneManager.cpp
    src/modules/Parser.cpp
    src/modules/PulseClient.cpp
    src/modules/Tray.cpp
)

# add_executable(audio-manager src/main.cpp)
add_executable(audio-manager ${SOURCES})
target_include_directories(audio-manager PRIVATE
	src
	# ${GTKMM_INCLUDE_DIRS}
)
# target_link_libraries(audio-manager PRIVATE ${GTKMM_LIBRARIES})
target_link_libraries(audio-manager PRIVATE
    PkgConfig::GTKMM
    PkgConfig::PULSE
)
target_compile_options(audio-manager PRIVATE ${GTKMM_CFLAGS_OTHER})
