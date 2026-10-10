#######################################
# Effect

set(touchpoints_SOURCES
    main.cpp
    touchpoints.cpp
)

nwin_add_builtin_effect(touchpoints ${touchpoints_SOURCES})
target_link_libraries(touchpoints PRIVATE
    nwin

    KF6::GlobalAccel
)
