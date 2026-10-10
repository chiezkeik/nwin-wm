#######################################
# Effect

# Source files
set(screenedge_SOURCES
    main.cpp
    screenedgeeffect.cpp
)

nwin_add_builtin_effect(screenedge ${screenedge_SOURCES})
target_link_libraries(screenedge PRIVATE
    nwin

    KF6::Svg
    KF6::ConfigCore
)
