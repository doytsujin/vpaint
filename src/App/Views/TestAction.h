// Copyright (C) 2012-2016 The VPaint Developers.
// See the COPYRIGHT file at the top-level directory of this distribution
// and at https://github.com/dalboris/vpaint/blob/master/COPYRIGHT
//
// This file is part of VPaint, a vector graphics editor. It is subject to the
// license terms and conditions in the LICENSE.MIT file found in the top-level
// directory of this distribution and at http://opensource.org/licenses/MIT

#ifndef TESTACTION_H
#define TESTACTION_H

#include "Views/View2DMouseAction.h"

#include <QtDebug>

class TestAction: public View2DMouseAction
{
public:
    TestAction();

protected:
    bool acceptPMREvent(const View2DMouseEvent * event)
    {
        return true;
    }

    void pressEvent(const View2DMouseEvent * event)
    {
        qDebug() << "TestAction pressEvent" << event->viewPos();
    }

    void moveEvent(const View2DMouseEvent * event)
    {
        qDebug() << "TestAction moveEvent" << event->viewPos();
    }

    void releaseEvent(const View2DMouseEvent * event)
    {
        qDebug() << "TestAction releaseEvent" << event->viewPos();
    }
};

#endif // TESTACTION_H