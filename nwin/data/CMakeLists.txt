add_subdirectory(icons)

########### next target ###############
add_executable(nwin5_update_default_rules update_default_rules.cpp)
target_link_libraries(nwin5_update_default_rules
    KF6::ConfigCore
    Qt::Core
    Qt::DBus
)
install(TARGETS nwin5_update_default_rules DESTINATION ${KDE_INSTALL_LIBDIR}/kconf_update_bin/)

########### install files ###############

install(FILES org_kde_nwin.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
