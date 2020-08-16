#ifndef COSC_ASS_TWO_PLAYER
#define COSC_ASS_TWO_PLAYER
#include <string>
#include "Mosaic.h"

class Player{

    public:
        /*
        * The player constructor
        * @param playerId The id for the player
        * @param std::string playerName The name of the player.
        */
        Player();
        Player(int playerId, std::string playerName);
        ~Player();

        int getPlayerId();    
        
        int getScore();
        void addScore(int row);
        void setScore(int playerScore);
        void floorLineDeduct();

        void setHasFirstPlayerToken(bool hasToken);
        bool getHasFirstPlayerToken();
        void checkDirectionsAndAdd(int row, int col);

        std::string getName();
        Mosaic* getMosaic();

        std::string getColouredName();
        
    protected:
        int playerId;
        int score;
        std::string playerName;
        std::string colouredName;
        Mosaic* mosaic;
        bool hasFirstPlayerToken = false;
        
};



#endif