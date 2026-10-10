target_sources(nwin PRIVATE
    common.cpp
    cursortheme.cpp
    edid.cpp
    filedescriptor.cpp
    gravity.cpp
    lightsensor.cpp
    orientationsensor.cpp
    proximitysensor.cpp
    ramfile.cpp
    realtime.cpp
    softwarevsyncmonitor.cpp
    subsurfacemonitor.cpp
    udev.cpp
    vsyncmonitor.cpp
)
if (NWIN_BUILD_X11)
    target_sources(nwin PRIVATE xcbutils.cpp)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_sources(nwin PRIVATE executable_path_proc.cpp)
elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    target_sources(nwin PRIVATE executable_path_sysctl.cpp)
else()
    message(FATAL_ERROR "Unsupported platform ${CMAKE_SYSTEM_NAME}")
endif()
