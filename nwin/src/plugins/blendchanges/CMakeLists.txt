#######################################
# Effect

set(blendchanges_SOURCES
    main.cpp
    blendchanges.cpp
)

nwin_add_builtin_effect(blendchanges ${blendchanges_SOURCES})
target_link_libraries(blendchanges PRIVATE
    nwin
    Qt::DBus
)
