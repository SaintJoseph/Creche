#include "qcolor_extension.h"

#include <QColor>

#ifdef QT_DEBUG
#include <cassert>
#endif

void asetHsvHue(QColor &color, int hue)
{
    int saturation = color.hsvSaturation();
    int value = color.value();
    color.setHsv(hue,saturation,value);
#ifdef QT_DEBUG
    assert(color.isValid());
#endif
}

void asetHsvSaturation(QColor &color, int saturation)
{
    int hue = color.hsvHue();
    int value = color.value();
    color.setHsv(hue,saturation,value);
#ifdef QT_DEBUG
    assert(color.isValid());
#endif
}

void asetValue(QColor &color, int value)
{
    int hue = color.hsvHue();
    int saturation = color.hsvSaturation();
    color.setHsv(hue,saturation,value);
#ifdef QT_DEBUG
    assert(color.isValid());
#endif
}
