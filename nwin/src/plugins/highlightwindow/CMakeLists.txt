#######################################
# Effect

# Source files
set(highlightwindow_SOURCES
    highlightwindow.cpp
    main.cpp
)

nwin_add_builtin_effect(highlightwindow ${highlightwindow_SOURCES})
target_link_libraries(highlightwindow PRIVATE
    nwin

    Qt::DBus
)
