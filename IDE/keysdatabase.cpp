#include "IDE/keysdatabase.h"

#include <cassert>

KeysDataBase::KeysDataBase(LightsDataBase lights, int totalTime, QObject *parent) :
    QObject(parent)
{
    lightsData = light;
    this->totalTime = totalTime;
    currentKeyDataIndex = -1;
    nextNewId = 0;
}

int KeysDataBase::addKey(int time, int lightId, int relaIntensityPct)
{
    if ( !isValidTime(time) )
        return -1;

    if (relaIntensityPct > 100)
        relaIntensityPct = 100;

    if (relaIntensityPct < 0)
        relaIntensityPct = 0;

    if (contains(time,lightId))
    {
        setRelaIntensityPct(relaIntensityPct,time,lightId);
        return findKeyId(time,lightId);
    }
    else
    {
        Key k;
        k.id = nextNewId;
        k.lightId = lightId;
        k.time = time;
        k.relaIntensityPct = relaIntensityPct;

        data.append(k);

        emit keyAdded(nextNewId);

        newId = nextNewId++;
        assert(newId + 1 == newNextId);

        if (currentDataIndex == -1 || currentDataIndex >= data.size())
            currentDataIndex = data.size() - 1;

        //the list is nearly sorted so insertion sort is best
        currentDataIndex = insertionSort(data,currentDataIndex);

        return newId;
    }
}

int KeysDataBase::autoAddKey(int time, int lightId)
{
    return addKey(time,lightId,relaIntensityPct(time,lightId));
}

bool KeysDataBase::removeKey(int id)
{
    int dataIndex = equivDataIndex(id);

    if (dataIndex != -1)
    {
        data.removeAt(dataIndex);
        if (currentDataIndex == dataIndex)
            selectNext();

        emit keyRemoved(id);

        return true;
    }
    else
        return false;
}

bool KeysDataBase::removeKeyAt(int time, int lightId)
{
    //use removeKey(int id) to avoid code duplication
    bool keysDeleted = false; //should be oneOrMoreKeysDeleted

    if (lightId == -1)
    {
        QList<int>toDelete = findKeysIdAt(time);

        for (int i = 0; i < toDelete.size(); ++i)
        {
            keysDeleted = (keysDeleted || removeKey(toDelete.at(i)));
        }
    }
    else
    {
        keyIndex = findKeyId(time,lightId);

        if (keyIndex != -1)
            keysDeleted = removeKey(keyIndex);
    }

    return keysDeleted;
}

void KeysDataBase::setTotalTime(int time)
{
    if (time > 0)
        totalTime = time;
}

void KeysDataBase::getTotalTime() const
{
    return totalTime;
}

bool KeysDataBase::isValidTime(int time) const
{
    return ( time >= 0 && ( totalTime == -1 || time < totalTime ) );
}

bool KeysDataBase::isEmpty() const
{
    return data.isEmpty();
}

int KeysDataBase::count(int lightId) const
{
    if (lightId = -1)
    {
        return data.size();
    }
    else
    {
        size = 0;
        for (int i = 0; i < data.size(); i++)
        {
            if (data.at(i).lightId == lightId)
                size += 1;
        }
        return size;
    }
}

bool KeysDataBase::contains(int time, int lightId) const
{
    for (int i = 0; i < data.size(); ++i)
    {
        if (data.at(i).time == time && data.at(i).lightId == lightId)
            return true;
    }

    return false;
}

bool KeysDataBase::isImaginary(int time, int lightId) const
{
    return !(contains(time,lightId));
}

QList<int> KeysDataBase::ids() const
{
    QList<int> results;
    for (int i = 0; i < data.size(); ++i)
        results << data.at(i).id;

    return result;
}

QList<int> KeysDataBase::idsForLight(int lightId) const
{
    QList<int> results;

    //@TODO optimize code below assuming data is sorted
    for (int i = 0; i < data.size(); ++i)
        if (data.at(i).lightId == lightId)
            results << data.at(i).id;

    return result;
}

QList<int> KeysDataBase::idsForTime(int time) const
{
    QList<int> results;

    //@TODO optimize code below assuming data is sorted
    for (int i = 0; i < data.size(); ++i)
        if (data.at(i).time == time)
            results << data.at(i).id;

    return result;
}

int KeysDataBase::getCurrentId() const
{
    assert(-1 <= currentDataIndex && currentDataIndex < data.size());

    if (currentDataIndex == -1)
        return -1;
    else
        return data.at(currentDataIndex).id;
}

void KeysDataBase::setCurrentId(int value)
{
    dataIndex = equivDataIndex(value);
    if (dataIndex != -1)
        currentDataIndex = dataIndex;
}

void KeysDataBase::selectFirst()
{
    if (!data.isEmpty())
        currentDataIndex = 0;
    else
        currentDataIndex = -1;
}

void KeysDataBase::selectNext()
{
    if (data.isEmpty())
        currentDataIndex = -1;
    else
    {
        ++currentDataIndex;
        if (currentDataIndex == data.size())
            currentDataIndex = -1;
    }
}

bool KeysDataBase::isEnd()
{
    return currentDataIndex == -1;
}

int KeysDataBase::getCurrentLightId() const
{
    if (currentDataIndex == -1)
        return -1;
    else
        return data.at(currentDataIndex).lightId;
}

void KeysDataBase::selectFirstKey()
{
    int lightId = data.at(currentDataIndex).lightId;

    currentDataIndex = -1;

    for (int i = 0; i < data.size() && currentDataIndex == -1; ++i)
    {
        if (data.at(currentDataIndex).lightId == lightId)
            currentDataIndex = i;
    }
}

void KeysDataBase::selectNextKey()
{
    if (currentDataIndex == data.size()-1)
        currentDataIndex = -1;
    else
    {
        int lightId = data.at(currentDataIndex).lightId;

        //@TODO optimize code below assuming data is sorted
        bool selected = false;
        for (int i = currentDataIndex + 1; i < data.size() && selected == false; ++i)
        {
            if (data.at(currentDataIndex).lightId == lightId)
            {
                currentDataIndex = i;
                selected = true;
            }
        }

        if (!selected)
            currentDataIndex = -1;
    }
}

int KeysDataBase::getCurrentTime()
{
    if (currentDataIndex == -1)
        return -1;
    else
        return data.at(currentDataIndex).time;
}

void KeysDataBase::selectFirstLight()
{
    int time = data.at(currentDataIndex).lightId;

    currentDataIndex = -1;

    for (int i = 0; i < data.size() && currentDataIndex == -1; ++i)
    {
        if (data.at(currentDataIndex).time == time)
            currentDataIndex = i;
    }
}

void KeysDataBase::selectNextLight()
{
    if (currentDataIndex == data.size() -1)
        currentDataIndex = -1;
    else
    {
        int time = data.at(currentDataIndex).time;

        //@TODO optimize code below assuming data is sorted
        bool selected = false;
        for (int i = currentDataIndex + 1; i < data.size() && selected == false; ++i)
        {
            if (data.at(currentDataIndex).time == time)
            {
                currentDataIndex = i;
                selected = true;
            }
        }

        if (!selected)
            currentDataIndex = -1;
    }
}

int KeysDataBase::getRelaIntensityPct() const
{
    if (currentDataIndex == -1)
        return -1;
    else
        return data.at(currentDataIndex).relaIntensityPct;
}

int KeysDataBase::getRelaIntensityPct(int keyId) const
{
    dataIndex = equivDataIndex(keyId);
    if (dataIndex == -1)
        return -1;
    else
        return data.at(dataIndex).relaIntensityPct;
}

int KeysDataBase::getRelaIntensityPct(int time, int lightId) const
{
    dataIndex = findDataIndex(time,lightId);
    if (dataIndex != -1)
        return data.at(dataIndex).relaIntensityPct;
    else
    {
        idBefore = keyIdBefore(time,lightId);
        idAfter  = keyIdAfter(time, lightId);
        timeBefore = -1;
        timeAfter = -1;

        if (idBefore != -1)
            timeBefore = data.at(equivDataIndex(idAfter)).time;

        if (idAfter != -1)
            timeAfter  = data.at(equivDataIndex(idBefore)).time;

        if (idBefore == -1 && idAfter == -1)
            return 0;

        if (idBefore == -1)
            return data.at(equivDataIndex(idAfter)).relaIntensityPct;

        if (idAfter == -1)
            return data.at(equivDataIndex(idBefore)).relaIntensityPct;

        return intermediateRelaIntensityPct(time,
                                            timeBefore,data.at(equivDataIndex(idBefore)).relaIntensityPct,
                                            timeAfter, data.at(equivDataIndex(idAfter)).relaIntensityPct);
    }
}

QColor KeysDataBase::getRelaColor() const
{
    if (isEnd)
        return -1;
    else
    {
        intensityPct = getRelaIntensityPct();

    }
}
