#ifndef TROOPTREASURER_CHANGEQUEUE_H
#define TROOPTREASURER_CHANGEQUEUE_H

#include <Qlist>
#include "change.h"

template<class T>
class ChangeQueue
{
public:
    ChangeQueue();
    ChangeQueue(const ChangeQueue&&) = delete;
    virtual ~ChangeQueue();
    ChangeQueue& operator=(ChangeQueue&&) = delete;

    int depth();
    bool more();
    void purge();
    void purgeRecordsFor(T obj);
    bool push(Change<T> change);
    Change<T> pop();
    Change<T> peek();

private:
    QList<Change<T>> _queue; //Using a list inplace of queue as base to allow future needs to eval contents in later stages
};

template<class T>
ChangeQueue<T>::ChangeQueue()
{}

template<class T>
ChangeQueue<T>::~ChangeQueue()
{}

template<class T>
int ChangeQueue<T>::depth()
{
    return _queue.length();
}

template<class T>
bool ChangeQueue<T>::more()
{
    return depth() > 0;
}

template<class T>
bool ChangeQueue<T>::push(Change<T> change)
{
    _queue.append(change);
    return true;
}

template<class T>
Change<T> ChangeQueue<T>::pop()
{
    if(_queue.empty())
    {
        Change<T> c;
        c.setReference(-1);
        return c;
    }
    return _queue.takeAt(0);
}

template<class T>
void ChangeQueue<T>::purge()
{
    _queue.clear();
}

template<class T>
Change<T> ChangeQueue<T>::peek()
{
    if(_queue.empty())
    {
        Change<T> c;
        c.setReference(-1);
        return c;
    }
    return _queue.at(0);
}

template<class T>
void ChangeQueue<T>::purgeRecordsFor(T obj)
{
    for(int i=0; i<_queue.length(); i++)
    {
        if(_queue[i].object()==obj)
        {
            _queue.removeAt(i);
        }
    }
}

#endif //TROOPTREASURER_CHANGEQUEUE_H
