#ifndef TROOPTREASURER_UI_H
#define TROOPTREASURER_UI_H

#define REM_COPY_MOVE(TypeName) \
TypeName(TypeName&) = delete;   \
void operator=(TypeName) = delete;

namespace UI
{
    enum class Action{NONE, ADD, EDIT, DELETE, SAVE};
}

#endif //TROOPTREASURER_UI_H
