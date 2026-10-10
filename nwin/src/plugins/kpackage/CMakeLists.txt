# SPDX-FileCopyrightText: 2023 Alexander Lohnau <alexander.lohnau@gmx.de>
# SPDX-License-Identifier: BSD-2-Clause

function(nwin_add_kpackage_structure dir file)
    kcoreaddons_add_plugin(nwin_${dir} SOURCES ${file} INSTALL_NAMESPACE kf6/packagestructure)
    target_link_libraries(nwin_${dir} KF6::Package)
endfunction()

nwin_add_kpackage_structure(aurorae aurorae/aurorae.cpp)
nwin_add_kpackage_structure(decoration decoration/decoration.cpp)
nwin_add_kpackage_structure(effect effect/effect.cpp)
nwin_add_kpackage_structure(scripts scripts/scripts.cpp)
nwin_add_kpackage_structure(windowswitcher windowswitcher/windowswitcher.cpp)
