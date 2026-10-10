set(idletime_plugin_SRCS
    poller.cpp
)

add_library(KF6IdleTimeNWinPlugin OBJECT ${idletime_plugin_SRCS})
target_compile_definitions(KF6IdleTimeNWinPlugin PRIVATE QT_STATICPLUGIN)
target_link_libraries(KF6IdleTimeNWinPlugin
    KF6::IdleTime
    nwin
)
