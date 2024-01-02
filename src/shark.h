#include "utils.h"

#ifndef SHARK_H_   
#define SHARK_H_
#define ENERGY_S 10


planer_c moveShark(Creature * creature,vector * fishes,vector * sharks);

void drawShark(planer_c coords);

planer_c reproduceShark(Creature * creature,vector * sharks,planer_c next_coordinate);

Creature * newShark(planer_c coords);

#endif // FISH_H_
