add_definitions(-DKWIN_UNIT_TEST)
remove_definitions(-DQT_USE_QSTRINGBUILDER)
add_subdirectory(effect)
add_subdirectory(integration)
add_subdirectory(libinput)
add_subdirectory(wayland)
# drm autotests are broken on FreeBSD for yet unknown reasons
# As the test isn't doing anything platform specific, only run it on Linux
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_subdirectory(drm)
endif()

########################################################
# Test WindowPaintData
########################################################
set(testWindowPaintData_SRCS test_window_paint_data.cpp)
add_executable(testWindowPaintData ${testWindowPaintData_SRCS})
target_link_libraries(testWindowPaintData nwin Qt::Widgets Qt::Test )
add_test(NAME nwin-testWindowPaintData COMMAND testWindowPaintData)
ecm_mark_as_test(testWindowPaintData)

########################################################
# Test VirtualDesktopManager
########################################################
set(testVirtualDesktops_SRCS
    test_virtual_desktops.cpp
)
add_executable(testVirtualDesktops ${testVirtualDesktops_SRCS})

target_link_libraries(testVirtualDesktops
    nwin

    Qt::Test
    Qt::Widgets

    KF6::ConfigCore
    KF6::GlobalAccel
    KF6::I18n
    KF6::WindowSystem

    Plasma::Activities
)
add_test(NAME nwin-testVirtualDesktops COMMAND testVirtualDesktops)
ecm_mark_as_test(testVirtualDesktops)

########################################################
# Test ClientMachine
########################################################
if(NWIN_BUILD_X11)
    set(testClientMachine_SRCS
        ../src/client_machine.cpp
        test_client_machine.cpp
        xcb_scaling_mock.cpp
    )
    add_executable(testClientMachine ${testClientMachine_SRCS})
    set_target_properties(testClientMachine PROPERTIES COMPILE_DEFINITIONS "NO_NONE_WINDOW")

    target_link_libraries(testClientMachine
        Qt::Concurrent
        Qt::GuiPrivate
        Qt::Test
        Qt::Widgets

        KF6::ConfigCore
        KF6::WindowSystem

        XCB::XCB
        XCB::XFIXES

        ${X11_X11_LIB} # to make jenkins happy
    )
    add_test(NAME nwin-testClientMachine COMMAND testClientMachine)
    ecm_mark_as_test(testClientMachine)

    ########################################################
    # Test XcbWrapper
    ########################################################
    add_executable(testXcbWrapper test_xcb_wrapper.cpp xcb_scaling_mock.cpp)

    target_link_libraries(testXcbWrapper
        Qt::GuiPrivate
        Qt::Test
        Qt::Widgets

        KF6::ConfigCore
        KF6::WindowSystem

        XCB::XCB
    )
    add_test(NAME nwin-testXcbWrapper COMMAND testXcbWrapper)
    ecm_mark_as_test(testXcbWrapper)

    add_executable(testXcbSizeHints test_xcb_size_hints.cpp xcb_scaling_mock.cpp)
    set_target_properties(testXcbSizeHints PROPERTIES COMPILE_DEFINITIONS "NO_NONE_WINDOW")
    target_link_libraries(testXcbSizeHints
        Qt::GuiPrivate
        Qt::Test
        Qt::Widgets

        KF6::ConfigCore
        KF6::WindowSystem

        XCB::ICCCM
        XCB::XCB
    )
    add_test(NAME nwin-testXcbSizeHints COMMAND testXcbSizeHints)
    ecm_mark_as_test(testXcbSizeHints)

    ########################################################
    # Test XcbWindow
    ########################################################
    add_executable(testXcbWindow test_xcb_window.cpp xcb_scaling_mock.cpp)

    target_link_libraries(testXcbWindow
        Qt::GuiPrivate
        Qt::Test
        Qt::Widgets

        KF6::ConfigCore
        KF6::WindowSystem

        XCB::XCB
    )
    add_test(NAME nwin-testXcbWindow COMMAND testXcbWindow)
    ecm_mark_as_test(testXcbWindow)
endif()

########################################################
# Test OnScreenNotification
########################################################
set(testOnScreenNotification_SRCS
    ../src/input_event_spy.cpp
    ../src/onscreennotification.cpp
    onscreennotificationtest.cpp
)
add_executable(testOnScreenNotification ${testOnScreenNotification_SRCS})

target_link_libraries(testOnScreenNotification
    Qt::DBus
    Qt::Quick
    Qt::Test
    Qt::Widgets # QAction include

    KF6::ConfigCore
)

add_test(NAME nwin-testOnScreenNotification COMMAND testOnScreenNotification)
ecm_mark_as_test(testOnScreenNotification)

########################################################
# Test Gestures
########################################################
set(testGestures_SRCS
    ../src/gestures.cpp
    test_gestures.cpp
)
add_executable(testGestures ${testGestures_SRCS})

target_link_libraries(testGestures
    Qt::Test
    Qt::Widgets
)

add_test(NAME nwin-testGestures COMMAND testGestures)
ecm_mark_as_test(testGestures)

set(testOpenGLContextAttributeBuilder_SRCS
    ../src/opengl/abstract_opengl_context_attribute_builder.cpp
    ../src/opengl/egl_context_attribute_builder.cpp
    opengl_context_attribute_builder_test.cpp
)

add_executable(testOpenGLContextAttributeBuilder ${testOpenGLContextAttributeBuilder_SRCS})
target_link_libraries(testOpenGLContextAttributeBuilder epoxy::epoxy Qt::Test)
add_test(NAME nwin-testOpenGLContextAttributeBuilder COMMAND testOpenGLContextAttributeBuilder)
ecm_mark_as_test(testOpenGLContextAttributeBuilder)

set(testXkb_SRCS
    test_xkb.cpp
)
qt_add_dbus_interface(testXkb_SRCS ${CMAKE_SOURCE_DIR}/src/org.freedesktop.DBus.Properties.xml dbusproperties_interface)
add_executable(testXkb ${testXkb_SRCS})
target_link_libraries(testXkb
    nwin

    Qt::Gui
    Qt::GuiPrivate
    Qt::Test
    Qt::Widgets

    KF6::ConfigCore
    KF6::WindowSystem

    XKB::XKB
)
add_test(NAME nwin-testXkb COMMAND testXkb)
ecm_mark_as_test(testXkb)

########################################################
# Test FTrace
########################################################
add_executable(testFtrace test_ftrace.cpp)
target_link_libraries(testFtrace
    Qt::Test
    nwin
)
add_test(NAME nwin-testFtrace COMMAND testFtrace)
ecm_mark_as_test(testFtrace)

########################################################
# Test NWin Utils
########################################################
add_executable(testUtils test_utils.cpp)
target_link_libraries(testUtils
    Qt::Test
    nwin
)
add_test(NAME nwin-testUtils COMMAND testUtils)
ecm_mark_as_test(testUtils)

########################################################
# Test OutputTransform
########################################################
add_executable(testOutputTransform output_transform_test.cpp)
target_link_libraries(testOutputTransform
    Qt::Test
    nwin
)
add_test(NAME nwin-testOutputTransform COMMAND testOutputTransform)
ecm_mark_as_test(testOutputTransform)

########################################################
# Test Colorspace
########################################################
add_executable(testColorspaces test_colorspaces.cpp)
target_link_libraries(testColorspaces
    Qt::Test
    nwin
    lcms2::lcms2
)
add_test(NAME nwin-testColorspaces COMMAND testColorspaces)
ecm_mark_as_test(testColorspaces)

########################################################
# Test UInt32Serial
########################################################
add_executable(testSerial serial_test.cpp)
target_link_libraries(testSerial
    Qt::Test
    nwin
)
add_test(NAME nwin-testSerial COMMAND testSerial)
ecm_mark_as_test(testSerial)

########################################################
# Test Box
########################################################
add_executable(testRect test_rect.cpp)
target_link_libraries(testRect
    Qt::Test
    nwin
)
add_test(NAME nwin-testRect COMMAND testRect)
ecm_mark_as_test(testRect)

########################################################
# Test BoxF
########################################################
add_executable(testRectF test_rectf.cpp)
target_link_libraries(testRectF
    Qt::Test
    nwin
)
add_test(NAME nwin-testRectF COMMAND testRectF)
ecm_mark_as_test(testRectF)

########################################################
# Test Region
########################################################
add_executable(testRegion test_region.cpp)
target_link_libraries(testRegion
    Qt::Test
    nwin
)
add_test(NAME nwin-testRegion COMMAND testRegion)
ecm_mark_as_test(testRegion)

########################################################
# Test RegionF
########################################################
add_executable(testRegionF test_regionf.cpp)
target_link_libraries(testRegionF
    Qt::Test
    nwin
)
add_test(NAME nwin-testRegionF COMMAND testRegionF)
ecm_mark_as_test(testRegionF)

add_test(NAME kcm_animations_smoketest COMMAND kcmshell6 --smoke-test kcm_animations)
set_tests_properties(kcm_animations_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_nwindecoration_smoketest COMMAND kcmshell6 --smoke-test kcm_nwindecoration)
set_tests_properties(kcm_nwindecoration_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_nwin_virtualdesktops_smoketest COMMAND kcmshell6 --smoke-test kcm_nwin_virtualdesktops)
set_tests_properties(kcm_nwin_virtualdesktops_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_nwin_effects_smoketest COMMAND kcmshell6 --smoke-test kcm_nwin_effects)
set_tests_properties(kcm_nwin_effects_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_nwinoptions_smoketest COMMAND kcmshell6 --smoke-test kcm_nwinoptions)
set_tests_properties(kcm_nwinoptions_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_nwinrules_smoketest COMMAND kcmshell6 --smoke-test kcm_nwinrules)
set_tests_properties(kcm_nwinrules_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_nwinscreenedges_smoketest COMMAND kcmshell6 --smoke-test kcm_nwinscreenedges)
set_tests_properties(kcm_nwinscreenedges_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_nwintouchscreen_smoketest COMMAND kcmshell6 --smoke-test kcm_nwintouchscreen)
set_tests_properties(kcm_nwintouchscreen_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_nwin_scripts_smoketest COMMAND kcmshell6 --smoke-test kcm_nwin_scripts)
set_tests_properties(kcm_nwin_scripts_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_nwintabbox_smoketest COMMAND kcmshell6 --smoke-test kcm_nwintabbox)
set_tests_properties(kcm_nwintabbox_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_virtualkeyboard_smoketest COMMAND kcmshell6 --smoke-test kcm_virtualkeyboard)
set_tests_properties(kcm_virtualkeyboard_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)

add_test(NAME kcm_nwinxwayland_smoketest COMMAND kcmshell6 --smoke-test kcm_nwinxwayland)
set_tests_properties(kcm_nwinxwayland_smoketest PROPERTIES
    ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
)
