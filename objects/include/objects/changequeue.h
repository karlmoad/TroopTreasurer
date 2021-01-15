#ifndef TROOPTREASURER_CHANGEQUEUE_H
#define TROOPTREASURER_CHANGEQUEUE_H

#include <queue>
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
    bool pushChange(Change<T> change);
    void purge();
    Change<T> popChange();


private:
    std::queue<Change<T>> _queue;
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
    return _queue.size();
}

template<class T>
bool ChangeQueue<T>::more()
{
    return depth() > 0;
}

template<class T>
bool ChangeQueue<T>::pushChange(Change<T> change)
{
    _queue.push(change);
    return true;
}

template<class T>
Change<T> ChangeQueue<T>::popChange()
{
    if(_queue.empty())
    {
        Change<T> c;
        c.setReference(-1);
        return c;
    }
    Change<T> change = _queue.front();
    _queue.pop();
    return change;
}

template<class T>
void ChangeQueue<T>::purge()
{
    std::queue<T> empty;
    std::swap(_queue,empty);
}

#endif //TROOPTREASURER_CHANGEQUEUE_H
