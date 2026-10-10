/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2011 Thomas Lübking <thomas.luebking@web.de>
    SPDX-FileCopyrightText: 2018 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "effect/anidata_p.h"
#include "effect/effecthandler.h"

namespace NWin
{

QDebug operator<<(QDebug dbg, const NWin::AniData &a)
{
    dbg.nospace() << a.debugInfo();
    return dbg.space();
}

FullScreenEffectLock::FullScreenEffectLock(Effect *effect)
{
    effects->setActiveFullScreenEffect(effect);
}

FullScreenEffectLock::~FullScreenEffectLock()
{
    effects->setActiveFullScreenEffect(nullptr);
}

AniData::AniData()
    : attribute(AnimationEffect::Opacity)
    , customCurve(0) // Linear
    , meta(0)
    , frozenTime(-1)
    , startTime(0)
    , waitAtSource(false)
    , keepAlive(true)
{
}

AniData::AniData(AnimationEffect::Attribute a, int meta_, const FPx2 &to_,
                 int delay, const FPx2 &from_, bool waitAtSource_,
                 const std::shared_ptr<FullScreenEffectLock> &fullScreenEffectLock, bool keepAlive,
                 GLShader *shader)
    : attribute(a)
    , from(from_)
    , to(to_)
    , meta(meta_)
    , frozenTime(-1)
    , startTime(AnimationEffect::clock() + delay)
    , fullScreenEffectLock(fullScreenEffectLock)
    , waitAtSource(waitAtSource_)
    , keepAlive(keepAlive)
    , shader(shader)
{
}

bool AniData::isActive() const
{
    if (!timeLine.done()) {
        return true;
    }

    if (timeLine.direction() == TimeLine::Backward) {
        return !(terminationFlags & AnimationEffect::TerminateAtSource);
    }

    return !(terminationFlags & AnimationEffect::TerminateAtTarget);
}

static QString attributeString(NWin::AnimationEffect::Attribute attribute)
{
    switch (attribute) {
    case NWin::AnimationEffect::Opacity:
        return QStringLiteral("Opacity");
    case NWin::AnimationEffect::Brightness:
        return QStringLiteral("Brightness");
    case NWin::AnimationEffect::Saturation:
        return QStringLiteral("Saturation");
    case NWin::AnimationEffect::Scale:
        return QStringLiteral("Scale");
    case NWin::AnimationEffect::Translation:
        return QStringLiteral("Translation");
    case NWin::AnimationEffect::Rotation:
        return QStringLiteral("Rotation");
    case NWin::AnimationEffect::Position:
        return QStringLiteral("Position");
    case NWin::AnimationEffect::Size:
        return QStringLiteral("Size");
    case NWin::AnimationEffect::Clip:
        return QStringLiteral("Clip");
    default:
        return QStringLiteral(" ");
    }
}

QString AniData::debugInfo() const
{
    return (QLatin1StringView("Animation: ") + attributeString(attribute)
            + QLatin1StringView("\n     From: ") + from.toString()
            + QLatin1StringView("\n       To: ") + to.toString()
            + QLatin1StringView("\n  Started: ") + QString::number(AnimationEffect::clock() - startTime) + QLatin1StringView("ms ago\n")
            + QLatin1StringView(" Duration: ") + QString::number(timeLine.duration().count()) + QLatin1StringView("ms\n")
            + QLatin1StringView("   Passed: ") + QString::number(timeLine.elapsed().count()) + QLatin1StringView("ms\n"));
}

} // namespace NWin
