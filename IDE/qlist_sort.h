#ifndef QLIST_SORT_H
#define QLIST_SORT_H

#include <QList>

//Sort QList<T> assuming T implemant operator>(const T& , const T&)
template <class T>
void insertionSort(QList<T> &list)
{
    for (int i = 1; i < list.size(); ++i)
    {
        T elem = list.at(i);

        int j;
        for (j = i; j > 0 && list.at(j-1) > elem; j--)
            list[j] = list[j-1];
        list[j] = elem;
    }
}

template <class T>
int insertionSort(QList<T> &list, int followId)
{
    for (int i = 1; i < list.size(); ++i)
    {
        T elem = list.at(i);

        int j;
        for (j = i; j > 0 && list.at(j-1) > elem; j--)
        {
            list[j] = list[j-1];
        }
        list[j] = elem;


        if (j <= followId)
        {
            if (followId < i)
                ++followId;
            else
            {
                if (followId == i)
                    followId = j;
            }
        }
    }
    return followId;
}

//Sort QList<T> assuming T implemant operator>(const T& , const T&)
template <class T>
void bubbleSort(QList<T> &list)
{
    bool ordonned = false;
    int size = list.size();
    while(!ordonned)
    {
        ordonned = true;
        for (int i = 0; i < size-1 ; i++)
        {
            if (list.at(i) > list.at(i+1))
            {
                T tmp = list.at(i);
                list[i] = list.at(i+1);
                list[i+1] = tmp;
                ordonned = false;
            }
        }
        size--;
    }
}

//Sort QList<T> assuming T implemant operator>(const T& , const T&)
template <class T>
void shellSortPhase(QList<T> &list, unsigned int gap)
{
    int size = list.size();
    for (int i = gap; i < size; ++i)
    {
        T value = list.at(i);
        int tmp = i - gap;
        int j = 0;

        for (j = tmp; (j >= 0) && (list.at(j) > value); j -= gap)
            list[j + gap] = list[j];

        list[j + gap] = value;
    }
}

template<class T>
void shellSort(QList<T> &list)
{
    static const unsigned int gaps[9]
            = {1, 4, 10, 23, 57, 132, 301, 701, 1750};

    unsigned int tmp = 9 -1;

    for (unsigned int i = tmp; i != -1; --i)
        shellSortPhase(list, gaps[i]);
}


#endif // QLIST_SORT_H
