#include "lightsdatabase.h"

#include <QColor>
#include "settings.h"

#include <cassert>
#include <stdexcept>

LightsDataBase::LightsDataBase(QObject *parent) :
    QObject(parent)
{
    nextNewId = 0;
    currentDataIndex = -1;
}

int LightsDataBase::addLight()
{
    Light l;
    l.color = QColor(DEFAULT_LIGHT_COLOR);
    l.name = DEFAULT_LIGHT_NAME;

    data.insert(nextNewId,l);

    emit lightAdded(nextNewId);

    newId = nextNewId++;
    assert(newId + 1 == newNextId);

    if (!data.contains(currentDataIndex))
        currentDataIndex = newId;

    return newId;
}

bool LightsDataBase::removeLight(int id)
{
    if (currentDataIndex == id)
        selectNext();

//WARNING for modify:
//the code below is writed to time
//for DEBUG and RELEASE
#ifdef QT_DEBUG
    int numOfLightsDeleted = data.remove(id);

    assert(numOfLightsDeleted <= 1);

    if (numOfLightsDeleted < 1)
        return false;
    else
    {
        emit lightRemoved(id);
        return true;
    }
#else
    if (data.remove(id) < 1)
        return false;
    else
    {
        emit lightRemoved(id);
        return true;
    }
#endif
}

bool LightsDataBase::isEmpty() const
{
    return data.isEmpty();
}

int LightsDataBase::size() const
{
    return data.size();
}

bool LightsDataBase::contains(int id) const
{
    return data.contains(id);
}

QList<int> LightsDataBase::ids() const
{
    return data.keys();
}

int LightsDataBase::getCurrentId() const
{
    return currentDataIndex;
}

void LightsDataBase::setCurrentId(int value)
{
    if (data.contains(value))
        currentDataIndex = value;
    else
    {
        throw std::invalid_argument("LightsDataBase::setCurrentId(int value) : " + value + "is not a valid lightId");
    }
}

void LightsDataBase::selectFirst()
{
    if (data.isEmpty())
        return;

    currentDataIndex = data.begin().key();
}

void LightsDataBase::selectNext()
{
    if (data.isEmpty() || data.size() == 1)
        return;

    QHash::const_iterator<int,Light> dataIterator = data.constFind(currentDataIndex);

    ++dataIterator;
    if (dataIterator == data.constEnd())
        currentDataIndex = -1;
    else
        currentDataIndex = dataIterator.key();
}

void LightsDataBase::isEnd()
{
    if (currentDataIndex == -1)
        return true;
    else
        return false;
}

QColor LightsDataBase::getColor(int id, QColor defaultConstructedValue) const
{
    if (!data.contains(id))
    {
        throw std::invalid_argument("LightsDataBase::getColor(int id) const : " + id + " is not a valid lightId");
        return defaultConstructedValue;
    }

#ifdef QT_DEBUG
    assert(data.value(id).color.isValid());
#endif

    return data.value(id).color;
}

int LightsDataBase::getHue(int id) const
{
    return getColor(id).hsvHue();
}

int LightsDataBase::getIntensity(int id) const
{
    return getColor(id).value();
}

bool LightsDataBase::isWhite(int id) const
{
#ifdef QT_DEBUG
    assert(getColor(id).hsvSaturation() == 0 || getColor(id).hsvSaturation() == 255);
#endif

    return (getColor(id).hsvSaturation() == 0);
}

QString LightsDataBase::getName(int id, QString defaultConstructedValue) const
{
    if (!data.contains(id))
    {
        throw std::invalid_argument("LightsDataBase::getName(int id) const : " + id + " is not a valid lightId");
        return defaultConstructedValue;
    }

    return data.value(id).name;
}

void LightsDataBase::setColor(QColor color, int id)
{
    if (!color.isValid())
        return;

    if (!data.contains(id))
    {
        throw std::invalid_argument("LightsDataBase::setColor(QColor color, int id) : " + id + " is not a valid lightId");
        return;
    }

    data[id].color = color;
    emit lightEdited(id);
}

void LightsDataBase::setHue(int hue, int id)
{
//    if ( !( (0 <= hue) && (hue  < 256) ) )
    if ( (hue < 0) || (256 <= hue) )
        return;

    if (!data.contains(id))
    {
        throw std::invalid_argument("LightsDataBase::setHue(int hue, int id) : " + id + " is not a valid lightId");
        return;
    }

    asetHsvHue(data[id].color,hue);
    emit lightEdited(id);
}

void LightsDataBase::setIntensity(int intensity, int id)
{
    if ( !( (0 <= intensity) && (intensity  < 256) ) )
        return;

    if (!data.contains(id))
    {
        throw std::invalid_argument("LightsDataBase::setIntensity(int intensity, int id) : " + id + " is not a valid lightId");
        return;
    }

    asetValue(data[id].color,intensity);
    emit lightIntensityEdited(id);
    emit lightEdited(id);
}

void LightsDataBase::setWhite(bool white, int id)
{
    if (!data.contains(id))
    {
        throw std::invalid_argument("LightsDataBase::setWhite(bool white, int id) : " + id + " is not a valid lightId");
        return;
    }

    if (white)
        asetHsvSaturation(data[id].color,0);
    else
        asetHsvSaturation(data[id].color,255);

    emit lightEdited(id);
}

void LightsDataBase::setName(QString name, int id)
{
    if (!data.contains(id))
    {
        throw std::invalid_argument("LightsDataBase::setWhite(bool white, int id) : " + id + " is not a valid lightId");
        return;
    }

    data[id].name = name;

    emit lightEdited(id);
}
