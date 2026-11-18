# SPDX-License-Identifier: GPL-3.0-or-later
cmake_minimum_required(VERSION 3.21)
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded)
endif()
project(EarthSky VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
find_package(Qt6 6.8 REQUIRED COMPONENTS Quick)
qt_standard_project_setup(REQUIRES 6.8)
qt_add_executable(EarthSky WIN32 main.cpp zodiac.h locationinfo.h bodylabels.h screenshot.h globe.inc vendor/astronomy.c)
set_target_properties(EarthSky PROPERTIES OUTPUT_NAME "Earth-and-Sky")
if(WIN32)
    target_sources(EarthSky PRIVATE app.rc)
endif()
# Share the production QML module and its resources with the UI tests.
qt_add_qml_module(EarthSkyQml STATIC NO_PLUGIN URI EarthSky VERSION 1.0
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qml/EarthSky"
    QML_FILES Main.qml
    RESOURCES data/zodiac.json data/constellations.json data/D3-CELESTIAL-LICENSE assets/worldcities_list.csv assets/world.jpg assets/moon-full.jpg assets/earth-and-sky.ico)
target_link_libraries(EarthSkyQml PRIVATE Qt6::Quick)
target_link_libraries(EarthSky PRIVATE EarthSkyQml Qt6::Quick)
target_compile_definitions(EarthSky PRIVATE EARTH_SKY_VERSION="${PROJECT_VERSION}")
install(TARGETS EarthSky RUNTIME DESTINATION .)

# Retain licensing with CMake-installed binaries.
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../../../LICENSE"
              "${CMAKE_CURRENT_SOURCE_DIR}/../../../THIRD-PARTY-NOTICES.txt" DESTINATION licenses)
install(FILES data/D3-CELESTIAL-LICENSE DESTINATION licenses)
install(FILES vendor/LICENSE DESTINATION licenses RENAME Astronomy-Engine-MIT.txt)

# Optional integration checks use isolated settings and exercise the real QML UI.
option(EARTH_SKY_BUILD_UI_TESTS "Build date/time and location UI regression checks" OFF)
if(EARTH_SKY_BUILD_UI_TESTS)
    enable_testing()
    find_package(Qt6 6.8 REQUIRED COMPONENTS Test)
    add_executable(EarthSkyUiTests tests/features.cpp vendor/astronomy.c)

    # The test harness includes main.cpp with isolated settings and a renamed entry point.
    set(test_moc_dir "${CMAKE_CURRENT_BINARY_DIR}/ui-test-moc")
    qt_generate_moc(main.cpp "${test_moc_dir}/main.moc" TARGET EarthSkyUiTests)
    # Keep AUTOMOC enabled for main.cpp when the application compiles it.
    set_source_files_properties(main.cpp PROPERTIES SKIP_AUTOMOC OFF)
    target_sources(EarthSkyUiTests PRIVATE "${test_moc_dir}/main.moc")
    target_include_directories(EarthSkyUiTests PRIVATE "${test_moc_dir}")
    target_compile_definitions(EarthSkyUiTests PRIVATE EARTH_SKY_VERSION="${PROJECT_VERSION}")
    target_link_libraries(EarthSkyUiTests PRIVATE EarthSkyQml Qt6::Quick Qt6::Test)
    add_test(NAME location-time-ui COMMAND EarthSkyUiTests)
endif()
