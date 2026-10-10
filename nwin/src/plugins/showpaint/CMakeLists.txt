#######################################
# Effect

set(showpaint_SOURCES
    main.cpp
    showpaint.cpp
)

nwin_add_builtin_effect(showpaint ${showpaint_SOURCES})
target_link_libraries(showpaint PRIVATE
    nwin
)
