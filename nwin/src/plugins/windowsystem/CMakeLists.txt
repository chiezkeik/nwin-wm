set(nwindowsystem_plugin_SRCS
    plugin.cpp
    windoweffects.cpp
    windowshadow.cpp
    windowsystem.cpp
)

add_library(KF6WindowSystemNWinPlugin OBJECT ${nwindowsystem_plugin_SRCS})
target_compile_definitions(KF6WindowSystemNWinPlugin PRIVATE QT_STATICPLUGIN)
target_link_libraries(KF6WindowSystemNWinPlugin nwin)
