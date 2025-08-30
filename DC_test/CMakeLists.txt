cmake_minimum_required(VERSION 3.20.0)


project(DC_test)
set(DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boards/app.overlay")
find_package(Zephyr REQUIRED)

target_sources(app PRIVATE src/main.c)
set(DTC_EXTRA_BINDING_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/dts/bindings")


target_link_libraries(app PRIVATE drivers__pwm)
