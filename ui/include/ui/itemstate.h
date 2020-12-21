/*
 * TroopTreasurer UI
 * Author: Karl Moad
 * ---------------------------------
 * ItemAction is an application enum identifying
 * base action identifier of MainWindow defined context actions
 *
 * ItemState is a bitset used to communicate to the MainWindow the current viable
 * panel state based on the panel objects current context
 */
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
