#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "Player.h"
#include <string>

class AIPlayer : public Player{ //derived subclass
    public:
       AIPlayer(int playerId, std::string name);
    private:
       
};

#endif //AIPLAYER_H