//
// Created by Karl Moad on 12/16/20.
//

#include <bitset>

#ifndef TROOPTREASURER_ITEMSTATE_H
#define TROOPTREASURER_ITEMSTATE_H


enum class ItemAction
{
    ADD,
    EDIT,
    DELETE,
    SAVE
};

class ItemState
{
public:
    //Get
    bool saveEnabled() const {return _state[0];}
    bool addEnabled() const {return _state[1];}
    bool editEnabled() const {return _state[2];}
    bool deleteEnabled() const {return _state[3];}

    //Set
    void setSaveEnabled(bool state) {_state[0] = state;}
    void setAddEnabled(bool state) {_state[1] = state;}
    void setEditEnabled(bool state) {_state[2] = state;}
    void setDeleteEnabled(bool state) {_state[3] = state;}

private:
    std::bitset<5> _state;
};

#endif //TROOPTREASURER_ITEMSTATE_H
