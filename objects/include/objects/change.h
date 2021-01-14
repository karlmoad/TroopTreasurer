#ifndef TROOPTREASURER_CHANGERECORD_H
#define TROOPTREASURER_CHANGERECORD_H

enum class Action{UNDEFINED, CREATE, UPDATE, DELETE};

template <class T>
class Change
{
public:
    Change();
    explicit Change(Action action, T object = nullptr);
    virtual ~Change();

    Action action() const;
    void setAction(Action action);
    T object() const;
    void setObject(T object);
    int reference() const;
    void setReference(int value);

private:
    Action _action;
    T _obj;
    int _ref;
};

template<class T>
Change<T>::Change(): _action(Action::UNDEFINED), _obj(nullptr)
{}

template<class T>
Change<T>::Change(Action action, T object): Change()
{
    _action = action;
    _obj = object;
}

template<class T>
Change<T>::~Change()
{}

template<class T>
Action Change<T>::action() const
{
    return _action;
}

template<class T>
void Change<T>::setAction(Action action)
{
    _action = action;
}

template<class T>
T Change<T>::object() const
{
    return _obj;
}

template<class T>
void Change<T>::setObject(T object)
{
    _obj = object;
}

template<class T>
int Change<T>::reference() const
{
    return _ref;
}

template<class T>
void Change<T>::setReference(int value)
{
    _ref = value;
}

#endif //TROOPTREASURER_CHANGERECORD_H
