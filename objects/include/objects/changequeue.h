#ifndef TROOPTREASURER_CHANGEQUEUE_H
#define TROOPTREASURER_CHANGEQUEUE_H

#include <QList>
#include "change.h"


template<class T>
class ChangeQueue
{
public:
    ChangeQueue();
    ChangeQueue(const ChangeQueue&&) = delete;
    virtual ~ChangeQueue();
    ChangeQueue& operator=(ChangeQueue&&) = delete;

    bool contains(Change<T> change) const;
    int depth();
    bool more();
    bool pushChange(Change<T> change);
    void purge();
    Change<T> popChange();


private:
    QList<Change<T>*> _queue;
};

template<class T>
ChangeQueue<T>::ChangeQueue()
{}

template<class T>
ChangeQueue<T>::~ChangeQueue()
{
    qDeleteAll(_queue);
    _queue.clear();
}

template<class T>
int ChangeQueue<T>::depth()
{
    return _queue.count();
}

template<class T>
bool ChangeQueue<T>::more()
{
    return depth() > 0;
}

template<class T>
bool ChangeQueue<T>::pushChange(Change<T> change)
{
    if(!contains(change))
    {
        _queue.append(change);
    }
    return true;
}

template<class T>
Change<T> ChangeQueue<T>::popChange()
{
    if(depth() <= 0)
    {
        ObjectError err("End of Queue",static_cast<int>(ObjectErrorCode::GENERAL_OBJECT_ERROR));
        err.raise();
    }

    return _queue.takeAt(0);
}

template<class T>
void ChangeQueue<T>::purge()
{
    _queue.clear();
}

template<class T>
bool ChangeQueue<T>::contains(Change<T> change) const
{
    return _queue.contains(change);
}

#endif //TROOPTREASURER_CHANGEQUEUE_H
