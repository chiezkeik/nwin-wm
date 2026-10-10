/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2017 Martin Flöser <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include "abstract_opengl_context_attribute_builder.h"
#include <nwin_export.h>

namespace NWin
{

class NWIN_EXPORT EglContextAttributeBuilder : public AbstractOpenGLContextAttributeBuilder
{
public:
    std::vector<int> build() const override;
};

class NWIN_EXPORT EglOpenGLESContextAttributeBuilder : public AbstractOpenGLContextAttributeBuilder
{
public:
    std::vector<int> build() const override;
};

}
