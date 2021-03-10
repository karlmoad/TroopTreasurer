#ifndef TROOPTREASURER_OBJECTS_H
#define TROOPTREASURER_OBJECTS_H

#define REM_COPY_MOVE(TypeName) \
TypeName(TypeName&) = delete;   \
void operator=(TypeName) = delete;

#endif //TROOPTREASURER_OBJECTS_H
