#ifndef LIGHTSDATABASE_H
#define LIGHTSDATABASE_H

#include <QObject>
#include <QHash>
#include "qcolor_extension.h"

struct Light
{
    QColor color;
    QString name;
};

/*
 * Theses functions can throw a std::invalid_argument exception
 *  * setCurrentId(int value)
 *  * getColor(int id, QColor defaultConstructedValue) const
 *  * getHue(int id) const
 *  * getIntensity(int id) const
 *  * getName(int id, QString defaultConstructedValue) const
 *  * setColor(QColor color, int id)
 *  * setHue(int hue, int id)
 *  * setIntensity(int intensity, int id)
 *  * setWhite(bool white, int id)
 *  * setName(QString name, int id)
 *
 */

class LightsDataBase : public QObject
{
    Q_OBJECT
public:
    explicit LightsDataBase(QObject *parent = 0);

    //general
    int addLight(); //return id of new light
    bool removeLight(int id); //return true if a light is removed
    bool isEmpty() const;
    int size() const;
    bool contains(int id) const;
    QList<int> ids() const;

    //navigation
    int  getCurrentId() const; //return -1 if the list is empty or no one light is selected
    void setCurrentId(int value); //doesnt edit a light, just select another
    void selectFirst();
    void selectNext();
    bool isEnd();

    //get lights properties
    QColor  getColor(int id = currentDataIndex, QColor defaultConstructedValue = Qt::black) const;
    int     getHue(int id = currentDataIndex) const; //return 0 if id is invalid
    int     getIntensity(int id = currentDataIndex) const; //meanless if white //return 0 if id is invalid
    bool    isWhite(int id = currentDataIndex) const;
    QString getName(int id = currentDataIndex, QString defaultConstructedValue = QString()) const;

public slots:
    //set lights properties
    void setColor(QColor color, int id = currentDataIndex);      //color must be valid else noop
    void setHue(int hue, int id = currentDataIndex);             //0 <= hue < 256 else noop
    void setIntensity(int intensity, int id = currentDataIndex); //0 <= intensity < 256 else noop
    void setWhite(bool white, int id = currentDataIndex);
    void setName(QString name, int id = currentDataIndex);

signals:
    void lightAdded(int id);
    void lightRemoved(int id);
    void lightIntensityEdited(int id); //usefull for KeysDataBase
    void lightEdited(int id);

private:
    QHash<int,Light> data; //<id,Light>
    int currentDataIndex; //to use like iterator
                          //technically -1 is the index of the imaginary item after the last item
    int nextNewId;
};

#endif // LIGHTSDATABASE_H
