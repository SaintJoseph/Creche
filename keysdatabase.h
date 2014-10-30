#ifndef KEYSDATABASE_H
#define KEYSDATABASE_H

#include <QObject>

#include "lightsdatabase.h"
#include "qlist_sort.h"
#include "autilities.h"

struct Key
{
    int id;
    int time;
    int lightId;
    int relaIntensityPct; //between 0 and
};

bool operator<(const Key &k1, const Key &k2)
{
    int const id1 = k1.lightId;
    int const id2 = k2.lightId;

    return id1 < id2 || ( id1 == id2 && k1.time < k2.time );
}

bool operator>(const Key &k1, const Key &k2)
{
    int const id1 = k1.lightId;
    int const id2 = k2.lightId;

    return id1 > id2 || ( id1 == id2 && k1.time > k2.time );
}

bool operator<=(const Key &k1, const Key &k2)
{
    int const id1 = k1.lightId;
    int const id2 = k2.lightId;

    return id1 < id2 || ( id1 == id2 && k1.time <= k2.time );
}

bool operator>=(const Key &k1, const Key &k2)
{
    int const id1 = k1.lightId;
    int const id2 = k2.lightId;

    return id1 > id2 || ( id1 == id2 && k1.time >= k2.time );
}

class KeysDataBase : public QObject
{
    //A Key is A time AND A Light
    //Keys are sorted by lights next by time.
    //
    //The intensity of a light at one time
    //is given by relaIntensityPct and is between 0 and 100.
    //To compute rendered color we should use
    //relaIntensity which is between 0 and light intensity
    //and is given by static method getRelaIntensity(int relaIntensityPct , int lightIntensity)
    //or use relaColor given by getRelaColor methods.

    //the functions which take keyId as argument are used by others and use functions that receive give dataIndex

    Q_OBJECT
public:
    explicit KeysDataBase(LightsDataBase lights, int totalTime = -1, QObject *parent = 0);
        //totalTime is used to not have key after a given time
        //-1 means no restrictions

    //general
    int addKey(int time, int lightId, int relaIntensityPct);
        //returns key's id or -1 if time is invalid
        //relaIntensity must be between 0 and 100
        //else 0 or 100 will be choose
        //if light has already a key at this time, it will be overwrited but id will not be changed
    int autoAddKey(int time, int lightId);
        //same of addKey(...) but evaluate relaIntensity from previous and next key for this light
        //if there is no key before or no key after relaIntensity will be take the value of the nearest key
        //if there is no one other key relaIntensity will be 0

    bool removeKey(int id);
        //return true if a key is removed

    bool removeKeyAt(int time, int lightId = -1);
        //return true if a key is removed
        //if lightId == -1 all the keys for this time will be removed
        //else only the key for the light with the given id will be removed

    void setTotalTime(int time); //time must be greather than 0
    int getTotalTime() const;
    bool isValidTime(int time) const; //true if (0 <= time < totalTime) or (0 <= time && totalTime == -1)

    bool isEmpty() const; //return true if no one key exists
    int count(int lightId = -1) const;
    bool contains(int time, int lightId) const;
    bool isImaginary(int time, int lightId) const;
    QList<int> ids() const;
    QList<int> idsForLight(int lightId) const;
    QList<int> lightsIdsForTime(int time) const;
    QList<int> idsForTime(int time) const;

    //navigation
    int getCurrentId() const; //return -1 if no one key is selected
    void setCurrentId(int value);
    void selectFirst();
    void selectNext();
    bool isEnd(); //use it also to know if one key is selected

    int getCurrentLightId() const; //return -1 if no one light is selected
    void selectFirstKey(); //select first key of current light
    void selectNextKey();  //select next key of current light
    //void isEndOfKeys(); //use isEnd()

    int getCurrentTime() const; //return -1 if no one light is selected
    void selectFirstLight(); //select first light even if this light has no key at current time
    void selectNextLight();  //select next light even if this light has no key at current time
    //void isEndOfLights(); //use isEnd()

    //get keys properties
    int getRelaIntensityPct() const;            //return -1 if no one key is selected
    int getRelaIntensityPct(int keyId) const;   //return -1 if no one key has keyId id.
    int getRelaIntensityPct(int time, int lightId) const;
        //if the key doesn't exists, the relaIntensityPct will be computed but no one key will be added
        //see contains(...), isImaginary(...) and autoAddKey(...)

    QColor getRelaColor() const;          //return -1 if no one key is selected
    QColor getRelaColor(int keyId) const; //return -1 if no one key is selected
    QColor getRelaColor(int time, int lightId) const;
        //if the key doesn't exists, the relaColor will be computed but no one key will be added
        //see contains(...), isImaginary(...) and autoAddKey(...)

    //statics
    static int getRelaIntensity(int relaIntensityPct , int lightIntensity);
    static QColor getRelaColor(int relaIntensityPct , QColor lightColor);

public slots:
    //set keys properties
    bool setRelaIntensityPct(int relaIntensityPct);
    bool setRelaIntensityPct(int relaIntensityPct, int time, int lightId);
    bool setRelaIntensityPct(int relaIntensityPct, int keyId);
        //these functions are restrictive:
        //return true if a key is edited
        //no add a new key
        //no edit key if relaIntensity is invalid
        //see addKey(...)

    int setTime(int newTime);
    int setTime(int keyId, int newTime);
    int setTime(int oldTime, int lightId, int newTime);
        //these functions remove key and create a new key (=> new id)
        //the new key is selected
        //return the new id
        //if an error occurs (eg. a key already exists at newTime) these functions no op and -1 is returned

signals:
    void keyAdded(keyId);
    void keyRemoved(keyId);
    //@TODO for key edited and selection edited

private:
    int currentKeyId() const;
    int currentLightId() const;
    int currentTime() const;

    int relaIntensityPct() const;
    int relaIntensityPct(int time, int lightId) const;
    int relaIntensityPct(int keyId) const;

    int intermediateRelaIntensityPct(int time, int timeBefore, int intensityBefore, int timeAfter, int intensityAfter) const;
        //should be used by getRelaIntensityPct only
    int keyIdAfter(int time, int lightId, bool returnKeyIdAtTimeIfExists = false) const;  //return -1 if no one key found
    int keyIdBefore(int time, int lightId, bool returnKeyIdAtTimeIfExists = false) const; //return -1 if no one key found
    int findKeyId(int time, int lightId) const; //return -1 if no one key has these parameters (this function use contains)
    QList<int> findKeysIdAt(int time) const;
    QList<int> findKeysIdFor(int light) const;
    int findDataIndex(int time, int lightId) const;
    int equivDataIndex(int keyId) const; //return -1 if no one key has keyId id.


    QList<Key> data;
    LightsDataBase *lightsData;
    int currentDataIndex;
    int totalTime;
    int nextNewId;

};

#endif // KEYSDATABASE_H
