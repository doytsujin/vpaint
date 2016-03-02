// Copyright (C) 2012-2016 The VPaint Developers.
// See the COPYRIGHT file at the top-level directory of this distribution
// and at https://github.com/dalboris/vpaint/blob/master/COPYRIGHT
//
// This file is part of VPaint, a vector graphics editor. It is subject to the
// license terms and conditions in the LICENSE.MIT file found in the top-level
// directory of this distribution and at http://opensource.org/licenses/MIT

#ifndef SVGPARSER_H
#define SVGPARSER_H

#include <QtGlobal>
#include <XmlStreamReader.h>
#include <QColor>
#include <QtMath>

#include <VectorAnimationComplex/EdgeSample.h>

class QString;

class SvgParser
{
public:
    SvgParser();
//private:
    bool readRect_(XmlStreamReader & xml);
    bool readLine_(XmlStreamReader & xml);
    bool readPolyline_(XmlStreamReader & xml);
    bool readPolygon_(XmlStreamReader & xml);
    bool readCircle_(XmlStreamReader & xml);
    bool readEllipse_(XmlStreamReader & xml);
    bool readPath_(XmlStreamReader & xml);
    void readSvg_(XmlStreamReader & xml);

    // Utilities
    QColor parseColor_(QString s);
    Eigen::Vector2d getNextCoordinatePair(QString & orig, bool * ok = 0);

    // Path things
    void addMoveto(QVector<VectorAnimationComplex::EdgeSample> & samplingPoints, XmlStreamReader & xml);
};

template <class T>
class PotentialPoint
{
public:
    PotentialPoint(T & index) : end_(index), left_(-1), right_(-1) {}

    double getLeftTangent() { return left_; }
    double getRightTangent() { return right_; }
    void setLeftTangent(double angle) { left_ = fmod((fmod(angle, (2 * M_PI)) + (2 * M_PI)), (2 * M_PI)); }
    void setRightTangent(double angle) { right_ = fmod((fmod(angle, (2 * M_PI)) + (2 * M_PI)), (2 * M_PI)); }

    T & getIndex() { return end_; }

    bool isSmooth();

private:
    double left_, right_;
    const double angleThreshold = 0.01 * M_PI;
    T & end_;
};

class SvgPresentationAttributes
{
public:
    SvgPresentationAttributes(XmlStreamReader & xml, SvgParser & parser);
    QColor fill, stroke;
    qreal strokeWidth;
};

#endif // SVGPARSER_H
