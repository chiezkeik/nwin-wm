/*
    NWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2024 Xaver Hugl <xaver.hugl@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include "colorlut3d.h"
#include "colorspace.h"
#include "colortransformation.h"
#include "nwin_export.h"

namespace NWin
{

enum class ColorspaceType {
    LinearRGB = 0,
    NonLinearRGB,
    ICtCp,
    AnyNonRGB,
};

class IccProfile;

class NWIN_EXPORT ValueRange
{
public:
    double min = 0;
    double max = 1;

    bool operator==(const ValueRange &) const = default;
    ValueRange operator*(double mult) const;
};

class NWIN_EXPORT ColorTransferFunction
{
public:
    explicit ColorTransferFunction(TransferFunction tf);

    bool operator==(const ColorTransferFunction &) const = default;

    TransferFunction tf;
};

class NWIN_EXPORT InverseColorTransferFunction
{
public:
    explicit InverseColorTransferFunction(TransferFunction tf);

    bool operator==(const InverseColorTransferFunction &) const = default;

    TransferFunction tf;
};

class NWIN_EXPORT ColorMatrix
{
public:
    explicit ColorMatrix(const QMatrix4x4 &mat);

    bool operator==(const ColorMatrix &) const = default;

    QMatrix4x4 mat;
};

class NWIN_EXPORT ColorMultiplier
{
public:
    explicit ColorMultiplier(double factor);
    explicit ColorMultiplier(const QVector3D &factors);

    bool operator==(const ColorMultiplier &) const = default;

    QVector3D factors;
};

class NWIN_EXPORT ColorTonemapper
{
public:
    explicit ColorTonemapper(double referenceLuminance, double maxInputLuminance, double maxOutputLuminance);

    double map(double pqEncodedLuminance) const;
    bool operator==(const ColorTonemapper &) const = default;

    double m_referenceLuminance;
    double m_inputRange;
    double m_outputRange;
    double m_v;
};

class NWIN_EXPORT ColorClamp
{
public:
    explicit ColorClamp(ValueRange range);
    explicit ColorClamp(double minValue, double maxValue);

    bool operator==(const ColorClamp &) const = default;

    double m_minValue;
    double m_maxValue;
};

class NWIN_EXPORT ColorOp
{
public:
    using Operation = std::variant<ColorTransferFunction, InverseColorTransferFunction, ColorMatrix, ColorMultiplier, ColorTonemapper, std::shared_ptr<ColorTransformation>, std::shared_ptr<ColorLUT3D>, ColorClamp>;
    ValueRange input;
    ColorspaceType inputSpace = ColorspaceType::AnyNonRGB;
    Operation operation;
    ValueRange output;
    ColorspaceType outputSpace = ColorspaceType::AnyNonRGB;

    bool operator==(const ColorOp &) const = default;
    QVector3D apply(const QVector3D input) const;
    static QVector3D applyOperation(const ColorOp::Operation &operation, const QVector3D &input);
};

class NWIN_EXPORT ColorPipeline
{
public:
    /**
     * matrix calculations with floating point numbers can result in very small errors
     * this value is the minimum difference we actually care about; everything below
     * can and should be optimized out
     */
    static constexpr float s_maxResolution = 0.00001;

    explicit ColorPipeline();
    explicit ColorPipeline(const ValueRange &inputRange, ColorspaceType inputType);

    enum class InputType {
        FixedPoint,
        FloatingPoint,
    };
    static ColorPipeline create(const std::shared_ptr<ColorDescription> &from, const std::shared_ptr<ColorDescription> &to, RenderingIntent intent, InputType inputType = InputType::FixedPoint);

    ColorPipeline merged(const ColorPipeline &onTop) const;

    bool isIdentity() const;
    bool operator==(const ColorPipeline &other) const;
    const ValueRange &currentOutputRange() const;
    ColorspaceType currentOutputSpace() const;
    QVector3D evaluate(const QVector3D &input) const;

    void addMultiplier(double factor);
    void addMultiplier(const QVector3D &factors);
    void addTransferFunction(TransferFunction tf, ColorspaceType outputType);
    void addInverseTransferFunction(TransferFunction tf, ColorspaceType outputType);
    void addMatrix(const QMatrix4x4 &mat, const ValueRange &output, ColorspaceType outputType);
    void addTonemapper(const Colorimetry &containerColorimetry, double referenceLuminance, double maxInputLuminance, double maxOutputLuminance);
    void add(const ColorOp &op);
    void add(const ColorPipeline &pipeline);
    void add1DLUT(const std::shared_ptr<ColorTransformation> &transform, ColorspaceType outputType);
    void addClamp(const ValueRange &range);

    ValueRange inputRange;
    ColorspaceType inputSpace;
    std::vector<ColorOp> ops;
};

NWIN_EXPORT bool isFuzzyIdentity(const QMatrix4x4 &mat);

}

NWIN_EXPORT QDebug operator<<(QDebug debug, const NWin::ColorOp &op);
NWIN_EXPORT QDebug operator<<(QDebug debug, const NWin::ColorPipeline &pipeline);
NWIN_EXPORT QDebug operator<<(QDebug debug, const NWin::ValueRange &value);
