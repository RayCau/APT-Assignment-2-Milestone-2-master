#include <iostream>
#include <fstream>
#include <limits>
#include "Game.h"

Game::Game(){
    bag = new TileBag();
    boxLid = new BoxLid();
    this->quit = false;
}

Game::~Game(){
    delete bag;
    delete boxLid;
}

void Game::startGame(unsigned int seed) {    
    numFactoriesFromCenters = 0;

    bag->setSeed(seed);
    bag->initialiseTileBag();
    bag->randomise();
    // initFactories();

    /*
    * ADDED METHOD TO DECLUTTER THE CODE, AND REDUCES CODE DUPLICATION WHEN HELP IS CALLED LATER ON.
    */
    printHelp();
    
    //PROMPTS FOR NUMBER OF PLAYERS, AND SETS THE INPUTTEDPLAYERNUM VARIABLE
    std::cout<< "Please enter the number of" << BOLDRED <<" PLAYERS "<< COLOURENDING << "you wish to play with."<<std::endl;
    std::cin>>inputtedPlayerNum;
    
    //CHECKS IF THE INPUTTED PLAYER NUMBER IS A VALID NUMBER
    if(inputtedPlayerNum >= 2 && inputtedPlayerNum <= 4){
        //IF THE INPUTTED PLAYER NUMBER IS VALID, WE MOVE TO THE CENTRE FACTORIES
        std::cout<< "Please enter the number of" << BOLDRED <<" CENTRE FACTORIES "<< COLOURENDING << "you wish to play with"<<std::endl;
        std::cin>>inputtedNumCenters;
        //CALCULATE THE TOTAL NUMBER OF FACTORIES
        numFactoriesFromCenters = 5 + inputtedNumCenters;

        //CHECKS FOR VALID NUMBER OF CENTRES
        if(inputtedNumCenters >= 1 && inputtedNumCenters <= 2){
            //MOVED INIT FACTORIES DOWN HERE
            initFactories();
            // INITIALISE THE PLAYERS
            for(int i = 0; i < inputtedPlayerNum; i++) { 
                std::string name;
                std::cout << "Enter a name for player " <<  i + 1 << " (If you want to add an AI, type AI):" << std::endl;
                std::cout << "> ";
                std::cin >> name;
                if(name != "AI"){
                    Player* player = new Player(i, name);
                    players.insert(players.end(), player);
                }else{
                    Player* player = new AIPlayer(i, "AI");
                    players.insert(players.end(), player);
                }
            }
            //Start the match
            match(); 
        }else{
            //GIVE INVALID MESSAGE AND CLEAR INPUT
            std::cout<< underline << BOLDRED<< "That is an invalid number of CENTRE FACTORIES. " <<
                "Please enter a valid number."<< COLOURENDING<<std::endl;
            std::cout << std::endl;
            std::cin.clear();
            std::cin.ignore(1024,'\n');
            //PROMPT AGAIN
            startGame(seed);
        }
    }else{
        //GIVE INVALID MESSAGE AND CLEAR INPUT
        std::cout << underline << BOLDRED <<"That is an invalid number of PLAYERS. Please enter a valid number."<< COLOURENDING <<std::endl;
        std::cout << std::endl;
        std::cin.clear();
        std::cin.ignore (1024,'\n');
        //PROMPT AGAIN
        startGame(seed);
    }
   
    //end match
}

void Game::match() {
    std::cout<< "1" << std::endl;
    while(!gameEnd()){
        //std::cout << "Game End: " << gameEnd() << std::endl;
        std::cout << "=== " << "Start Round" << BOLDGREEN <<" \u25BA " << COLOURENDING << "==="<<std::endl;
        //check if player has 1st turn token
        int playerToGoFirst = 0;
        for(int j = 0; j < inputtedPlayerNum; j++) {
                if(players.at(j)->getHasFirstPlayerToken()) {
                playerToGoFirst = j;
                players.at(j)->setHasFirstPlayerToken(false);
            }
        }

        while(!checkIfAllFactoriesEmpty() && quit == false){          
            for(int i = playerToGoFirst; i< inputtedPlayerNum; ++i){
                if(!checkIfAllFactoriesEmpty() && quit == false){
                    std::string pname = players.at(i)->getName();std::cout<<std::endl;

                    std::cout<< "TURN FOR PLAYER: " << pname <<std::endl;
                    std::cout<< "Factories: "<<std::endl;
                    
                    printFactories(); std::cout<<std::endl;
                    printBoardsForAllPlayers();
                    playerTurn(players.at(i));
                } 
                if(playerToGoFirst != 0) {
                    playerToGoFirst = 0;
                }                
            }
            
        }            

        if(quit == false) {
            printBoardsForAllPlayers();
            for(int i = 0; i<inputtedPlayerNum; ++i){
                moveAllPatternLinesPerPlayer(players.at(i));
            }
            printBoardsForAllPlayers();

            //Move tiles from floor line (except F tile) to box lid.
            for(int i = 0; i < inputtedPlayerNum; ++i){
                moveTilesFromFloorLineToBoxLid(players.at(i));
            }
            std::cout << "=== " << "END OF ROUND" << " ==="<<std::endl;   

            //repopulate the factories after the round
            repopulateFactoriesAfterRound();
        }
        
    }
    std::cout << "=== " << "END OF MATCH" << " ==="<<std::endl;
    std::cout << "Final Scores: " <<std::endl;
    for(int i = 0; i < inputtedPlayerNum; ++i) {
        std::cout << players.at(i)->getName() << ": " << players.at(i)->getScore() <<std::endl;
    }
    std::cout << "Press ENTER to return to main menu" << std::endl;
}

void Game::playerTurn(Player* player) {
    int factoryId;
    int colourInt;
    int row;

    //checks if the player is an AI
    if(player->getName() != "AI"){
        //User input
        userInputForPlayerTurn(player, &factoryId, &colourInt, &row);
    }else{
        //if the player is an AI, run the AITurn for the player
        AITurn(player, &factoryId, &colourInt, &row);
    }
        if(quit == false){
            //Checking if selected values is valid / allowed.
            int freeSpacesOnPatternLinesRow = 0;
            int numTilesOfColourInFactory = factories.at(factoryId)->getNumberTilesOfColour(colourInt);
            int colourOfTilesInRow = 0;  
            int numFreeSpacesOnFloorLine = player->getMosaic()->numberOfFreeSpacesOnFloorLine();
            bool doesSelectedRowOnWallContainColour = false;

            //If the player hasnt selected the floorLine as their 'row'
            if(row != WALLSIZE) {
                doesSelectedRowOnWallContainColour = player->getMosaic()->doesWallRowContainColour(colourInt, row);
                freeSpacesOnPatternLinesRow = player->getMosaic()->numberOfFreeSpacesOnPatternLineRow(row);
                colourOfTilesInRow = player->getMosaic()->getColourOfTilesInPatternLineRow(row);
            }  
            //If the row they selected is empty
            if(colourOfTilesInRow == 0) {
                colourOfTilesInRow = colourInt;
            }

            if((row != WALLSIZE) && freeSpacesOnPatternLinesRow > 0 
                && numTilesOfColourInFactory > 0 && colourInt == colourOfTilesInRow && doesSelectedRowOnWallContainColour == false) {
                //move selected tiles from factory to correct patternLine
                moveTilesFromFactoryToPlayerPatternLine(player, colourInt, factoryId, row);
                //if the user hasnt chosent the centre factory,
                //move left overs (if any) in factory to the centre factory
                
                if(!factories.at(factoryId)->isCentre() && numFactoriesFromCenters == 6){
                    moveTilesFromFactoryToCentre(factoryId, 0, colourInt);
                    
                }else if(factories.at(factoryId)->isCentre() && numFactoriesFromCenters == 6) {
                    moveTilesFromFactoryToCentre(factoryId, 0, colourInt);
                }else if(player->getName() == "AI" && numFactoriesFromCenters == 7){
                    unsigned int amountTilesInFactory = 100;
                    int centreIdOfLeastTiles = 0;
                    for(int i = 0; i < 2; ++i){
                        if(factories[i]->getTiles()->size() < amountTilesInFactory){
                            amountTilesInFactory = factories[i]->getTiles()->size();
                            centreIdOfLeastTiles = i;
                        }
                    }
                    moveTilesFromFactoryToCentre(factoryId, centreIdOfLeastTiles, colourInt);
                }else{
                    std::cout << "Please enter the centre factory you want to send the leftover tiles to."<< std::endl;
                    int centerId;
                    std::cin>>centerId;
                    moveTilesFromFactoryToCentre(factoryId, centerId, colourInt);
                }
            } else if(row == WALLSIZE && numTilesOfColourInFactory > 0) {
                //Move tiles from factory to floor line if there is enough space.
                if(numFreeSpacesOnFloorLine >= numTilesOfColourInFactory) {
                moveTilesFromFactoryToFloorLine(player, colourInt, factoryId);

                if(!factories.at(factoryId)->isCentre() && numFactoriesFromCenters == 6){
                    moveTilesFromFactoryToCentre(factoryId, 0, colourInt);
                    
                }else if(factories.at(factoryId)->isCentre() && numFactoriesFromCenters == 6){
                    moveTilesFromFactoryToCentre(factoryId, 0, colourInt);
                }else if(player->getName() == "AI" && numFactoriesFromCenters == 7){
                    unsigned int amountTilesInFactory = 100;
                    int centreIdOfLeastTiles = 0;
                    for(int i = 0; i < 2; ++i){
                        if(factories[i]->getTiles()->size() < amountTilesInFactory){
                            amountTilesInFactory = factories[i]->getTiles()->size();
                            centreIdOfLeastTiles = i;
                        }
                    }
                    moveTilesFromFactoryToCentre(factoryId, centreIdOfLeastTiles, colourInt);
                }else{
                    std::cout << "Please enter the center factory you want to send the leftover tiles to."<< std::endl;
                    int centerId;
                    std::cin>>centerId;
                    moveTilesFromFactoryToCentre(factoryId, centerId, colourInt);
                }

                } else {
                std::cout << "You are trying to add too many tiles to the floor line" << std::endl;
                playerTurn(player);
                }
            } 
            else if (freeSpacesOnPatternLinesRow == 0) {
                std::cout << "There is no space left on the pattern line row you selected." << std::endl;
                playerTurn(player);
            } else if (numTilesOfColourInFactory == 0) {
                std::cout << "You either chose a non existent factory, or the factory does not contain any of the colours you selected." << std::endl;
                // playerTurn(player);
            } else if (colourInt != colourOfTilesInRow) {
                std::cout << "Each row can only contain one colour." << std::endl;
                playerTurn(player);
            } else if(doesSelectedRowOnWallContainColour == true) {
                std::cout << "The colour of tile you tried to put into row " << row << " already exists on the wall in row" << row << std::endl;
                playerTurn(player);
            }
        
        }
}   


void Game::AITurn(Player* AIPlayer, int* factoryId, int* colourInt, int* row){
    *row = 6;
    //instead of using break to stop the loop, turn is used to check if the AI can make a move
    bool turn = false;
    //loops through the number of factories (including the center)
    for(int selectedFactory = 0; selectedFactory < numFactoriesFromCenters; ++selectedFactory){
        //checks if the factory size is not 0
        if(factories.at(selectedFactory)->getTiles()->size() != 0 ){
            int tileColourSelected = factories.at(selectedFactory)->findColourOfGreatestAmountTile();
            if(tileColourSelected > 0 && turn == false){
                int numTiles = factories.at(selectedFactory)->getNumberTilesOfColour(tileColourSelected);
                *factoryId = selectedFactory;
                *colourInt = tileColourSelected;
                //iterates through the pattern lines
                for(int i = 0; i < NUMPATTERNLINEROWS; ++i){
                    //get the number of empty tiles in the row that we have found
                    int numEmpty = AIPlayer->getMosaic()->countEmptyTilesInPatternLineRow(i);           
                    
                    //checks if the number of number of empty tiles in the pattern line minus the number of tiles we selected is less than or equal to difference (5)
                    //checks if the row contains empty tiles
                    //checks if the wall doesnt contain the colour or if the pattern line
                    if(AIPlayer->getMosaic()->doesWallRowContainColour(tileColourSelected, i) == false && numEmpty - numTiles <= 5 
                        && AIPlayer->getMosaic()->checkPatternLineFull(i) == false && (AIPlayer->getMosaic()->getColourOfTilesInPatternLineRow(i) == tileColourSelected 
                        || AIPlayer->getMosaic()->getColourOfTilesInPatternLineRow(i) == E)) {
                            turn = true;     
                            *row = i;             
                    }
                }   
            }          
        }
    } 
    if(*row == 6){
        *row = 5;
    }
}



int Game::getColourFromWallRow(int row, int col){
    int coloursFromWall[5][5] = {{1, 2, 3, 4, 5},
                                 {5, 1, 2, 3, 4},
                                 {4, 5, 1, 2, 3},
                                 {3, 4, 5, 1, 2},
                                 {2, 3, 4, 5, 1}};
    int colour = coloursFromWall[row][col];
    return colour;
}


void Game::moveAllPatternLinesPerPlayer(Player* player){
    for(int row = 0; row < WALLSIZE; ++row){
            player->addScore(row);
            moveTileFromPatternLineToWall(row, player);
    }
    player->floorLineDeduct();
}

void Game::moveTileFromPatternLineToWall(int row, Player* player){
    if(player->getMosaic()->checkPatternLineFull(row)){
        int colour = player->getMosaic()->getPatternLines()[row][row].getColour();
        //SS THE CORRECT PLACE TO PUT THE COLOUR ON THE WALL
        int col = (colour + row - 1)%5;
        //SETS THE RESPECTIVE COLOURED TILE AS THE RIGHTMOST TILE IN THE PATTERNLINES
            
            //This was originally done by creating a new tile. This has been changed to just changing the colour of the tile
            //on the wall.
            //player->getMosaic()->getWall()[row][col] = *new Tile(player->getMosaic()->getPatternLines()[row][row].getColour());
        player->getMosaic()->getWall()[row][col].setColour(player->getMosaic()->getPatternLines()[row][row].getColour());
        //SETS THE COLOUR OF THE RIGHTMOST TILE TO EMPTY
        player->getMosaic()->getPatternLines()[row][row].setColour(E);
        //MOVES OTHER TILES TO THE BOXLID
        (*boxLid).addTiles(player->getMosaic()->getPatternLines()[row], row+1);
    }
}

void Game::moveTilesFromFactoryToPlayerPatternLine(Player* player, int colour, int factoryId, int row) {
    int freeSpacesOnPatternLinesRow = player->getMosaic()->numberOfFreeSpacesOnPatternLineRow(row);
    int numTilesOfColourInFactory = factories.at(factoryId)->getNumberTilesOfColour(colour); 
    int freeSpacesOnFloorLine = player->getMosaic()->numberOfFreeSpacesOnFloorLine();  

    int numOfTilesToBeAddedToFloorLine = numTilesOfColourInFactory - freeSpacesOnPatternLinesRow; 

    /*
    * If the factory chosen isnt the centre
    * Check if there are actually tiles within the selected factory
    * Check if there is actually free space on the patternlines row 
    * Check if there will be enough room on the floor line
    */
    if(!factories.at(factoryId)->isCentre()) {
        if(numTilesOfColourInFactory > 0 && freeSpacesOnPatternLinesRow > 0 && numOfTilesToBeAddedToFloorLine <= freeSpacesOnFloorLine) {
            Tile* tilesToAdd = factories.at(factoryId)->getTilesOfColour(colour);
            int tilesToAddLength = numTilesOfColourInFactory;

            for(int i = 0; i < tilesToAddLength; i++) {
                if(freeSpacesOnPatternLinesRow > 0) {
                    player->getMosaic()->addTileToPatternLines(&tilesToAdd[i], row);
                    freeSpacesOnPatternLinesRow--;
                } else {
                    //Check if there is enough space before adding it to floor line
                    player->getMosaic()->addTileToFloorLine(&tilesToAdd[i]);
                }                
            }
        }
    }

    /*
    * If the factory chosen was the centre
    */
    else if (factories.at(factoryId)->isCentre()) { 
        /*
        * If centre factory is chosen,
        * check if it has first player token. If so, add it to the players floor line
        */
        addFirstPlayerTokenToFloorLine(player, factoryId);
        removeFromOtherCentre(player);

        //move the tiles form the centre factory to the selected row
        for(unsigned int i = 0; i < factories.at(factoryId)->getTiles()->size(); i++) {
            int freeSpaces = player->getMosaic()->numberOfFreeSpacesOnPatternLineRow(row);
            int numOfTilesOfcolour = factories.at(0)->getNumberTilesOfColour(colour);
            
            //If there is an available free space in the patternline to put the tile, put it in the pattern line
            //else, put it on the floor line. 
            if(factories.at(factoryId)->getTiles()->get(i)->getColour() == colour) {
                if(freeSpaces > 0) {
                    player->getMosaic()->addTileToPatternLines(factories.at(factoryId)->getTiles()->get(i), row);
                    numOfTilesOfcolour--;
                } else if(player->getMosaic()->numberOfFreeSpacesOnFloorLine() > numOfTilesOfcolour) {
                    player->getMosaic()->addTileToFloorLine(factories.at(factoryId)->getTiles()->get(i));
                } else {
                    std::cout << "invalid turn. floor line does not have enough room" << std::endl;
                    playerTurn(player);
                }                
            }            
        }        
        //Remove all the tiles of colour that were taken from the centre factory
        factories.at(factoryId)->removeTilesOfColour(colour);  
    } 
    else {
        std::cout << "ERROR. Try again." << std::endl;
        playerTurn(player);
    }
}

/*
* This method assumes that there is enough space on the floor line 
* and therefore, there is no need to check for it.
*/
void Game::moveTilesFromFactoryToFloorLine(Player* player, int colour, int factoryId) {
    int numberOfTilesSelected = factories.at(factoryId)->getNumberTilesOfColour(colour);
    Tile* tilesToAdd = factories.at(factoryId)->getTilesOfColour(colour);
    for(int i = 0; i < numberOfTilesSelected; ++i) {
        player->getMosaic()->addTileToFloorLine(&tilesToAdd[i]);
    }

    /*
    * If the centre factory was chosen, remove the tiles from the centre factory.
    */
    if(factories.at(factoryId)->isCentre()) {
        //Move first player token to floor line
        addFirstPlayerTokenToFloorLine(player, factoryId);
        //Remove tiles from the other centre
        removeFromOtherCentre(player);
        //Remove all the tiles of colour that were taken from the centre factory
        factories.at(factoryId)->removeTilesOfColour(colour);
    }
}

void Game::checkForMultipleCenterAndMove(Player* player, int factoryId, int centerId, int colourInt){
    
}

void Game::moveTilesFromFactoryToCentre(int factoryId, int centerId, int colour) {
    if(factoryId != 0) {
        LinkedList* leftOverTiles = factories.at(factoryId)->getTiles();
        for(unsigned int i = 0; i < factories.at(factoryId)->getTiles()->size(); ++i) {
            if(leftOverTiles->get(i) != nullptr && leftOverTiles->get(i)->getColour() != colour) {
                factories.at(centerId)->getTiles()->addFront(leftOverTiles->get(i)); 
            }               
        }
        leftOverTiles->clear();
    }
}

void Game::moveTilesFromFloorLineToBoxLid(Player* player) {
    Tile* floor = player->getMosaic()->getFloorLine();
    for(int i = 0; i < FLOORLINESIZE; ++i) {
        if(floor[i].getColour() != E) {
            if(floor[i].getColour() == F) {
                floor[i].setColour(E);
            } else {
                boxLid->addTiles(&floor[i], 1);
            }
        } 
    }
}

void Game::printHelp(){
    std::cout << BOLDBLUE <<"---------------------------INPUT INSTRUCTIONS---------------------------" <<COLOURENDINGNEWLINE<<std::endl;
    std::cout << "For each turn, the expected input is: turn" << BOLDMAGENTA << "[number1]"<< COLOURENDING 
        << BOLDGREEN <<"[character]"<< COLOURENDING << BOLDCYAN<< "[number2]" << COLOURENDING << std::endl;
    std::cout << "Where... " << std::endl;
    std::cout << BOLDMAGENTA << " [number1] "<< COLOURENDING << "corresponds to: factory" << std::endl;
    std::cout << BOLDGREEN <<" [character] "<< COLOURENDING << "corresponds to: colour of tiles within previously selected \n \tfactory." << std::endl;
    std::cout << BOLDCYAN<< " [number2] " << COLOURENDING << "corresponds to: row on your pattern lines or the floor line" << std::endl << std::endl;
    std::cout << BOLDBLUE <<"------------------------OTHER INPUT INSTRUCTIONS------------------------"<<COLOURENDINGNEWLINE<<std::endl;
    std::cout << "Type" << BOLDYELLOW << "\'help\'" << COLOURENDING << " to print this page."<<std::endl;
    std::cout << "Type" << BOLDYELLOW << "\'rules\'" << COLOURENDING << "to print the rules of the game"<<std::endl;
    std::cout << "Type" << BOLDRED <<"\'quit\'" << COLOURENDING << "at any time to quit to return to the main menu."<<std::endl;
    std::cout << "Type" << BOLDGREEN <<"\'save\'" << COLOURENDING << "at any time to save a game to a file. After typing save, the \n \tconsole will" 
    << " wait for you to input your name for the file."<<std::endl;
    std::cout << BOLDBLUE <<"------------------------------------------------------------------------"<<COLOURENDINGNEWLINE<<std::endl;
    std::cout << BOLDRED <<"Type AI (with capitals) as a player name if you want to play with an AI."<<COLOURENDINGNEWLINE<<std::endl;
    std::cout << BOLDRED <<"You can play 2-4 player modes by typing '2', '3', or '4' \n \t when prompted for number of players."<<COLOURENDINGNEWLINE<<std::endl;
    std::cout << BOLDRED <<"You can play with 1-2 centre factories by typing '1' or '2' \n \t when prompted for number of centres."<<COLOURENDINGNEWLINE<<std::endl;
    std::cout << BOLDBLUE <<"------------------------------------------------------------------------"<<COLOURENDINGNEWLINE<<std::endl;
    std::cout<<std::endl;
}

void Game::printRules(){
    std::cout << BOLDBLUE <<"--------------------------------------------AZUL GAME RULES---------------------------------------------"<<COLOURENDINGNEWLINE<<std::endl;
    std::cout<< BOLDWHITE <<"Please see the following link for the rules of AZUL: https://www.ultraboardgames.com/azul/game-rules.php" << COLOURENDING <<std::endl;
    std::cout << BOLDBLUE <<"--------------------------------------------------------------------------------------------------------"<<COLOURENDINGNEWLINE<<std::endl;
}

void Game::printBoard(Player* player, int i) {
    const char* colours[4] = {BOLDGREEN, BOLDMAGENTA, BOLDRED, BOLDBLUE};

    std::cout << colours[i] << player->getName() << "'s BOARD: " << COLOURENDING <<std::endl;
    std::cout << "------------------------------" << std::endl;
    std::cout << BOLDGREEN <<"Score: "<< player->getScore()<< COLOURENDING <<std::endl;
    for(int i = 0; i < WALLSIZE; ++i) {
        player->getMosaic()->printPatternLineRow(i);
        std::cout << " || ";
        player->getMosaic()->printWallRow(i);
    }
    std::cout << "5: Floor Line: ";
    player->getMosaic()->printFloorLine();

    std::cout << std::endl;
}

void Game::printBoardsForAllPlayers() {
    for(int i = 0; i < inputtedPlayerNum; i++) {
        printBoard(players.at(i), i);
    }
}

void Game::initFactories(){
    if(numFactoriesFromCenters == 6){
        for(int i = 0; i<numFactoriesFromCenters; ++i){
            if(i == 0) {
                Factory* factory = new Factory(i);
                factory->setCentre(true);
                factories.insert(factories.end(), factory);
                factories.at(i)->populateCentreFactory();
            }
            else {
                Factory* factory = new Factory(i);
                factories.insert(factories.end(), factory);
                factories.at(i)->populateFactory(bag);
            }
        }
    }else{
        for(int i = 0; i<numFactoriesFromCenters; ++i){
            if(i == 0 || i == 1) {
                Factory* factory = new Factory(i);
                factory->setCentre(true);
                factories.insert(factories.end(), factory);
                factories.at(i)->populateCentreFactory();
            }
            else {
                Factory* factory = new Factory(i);
                factories.insert(factories.end(), factory);
                factories.at(i)->populateFactory(bag);
            }
        }
    }
}

void Game::repopulateFactoriesAfterRound(){
    //Check if there are enough tiles in the tile bag before refilling
    if(numFactoriesFromCenters == 6){
        if(bag->getSize() >= 25){
            for(int i = 0; i<numFactoriesFromCenters; ++i){
                if(i == 0){
                    factories.at(i)->populateCentreFactory();
                }else{
                    factories.at(i)->populateFactory(bag);
                }
            }
        // if there arent enough tiles, move tiles from boxlid back to tile bag and shuffle.
        }
        else {
            bag->refilTileBag(boxLid);
            repopulateFactoriesAfterRound();
        }  
    }else{
       if(bag->getSize() >= 25){
            for(int i = 0; i<numFactoriesFromCenters; ++i){
                if(i == 0 || i == 1){
                    factories.at(i)->populateCentreFactory();
                }else{
                    factories.at(i)->populateFactory(bag);
                }
            }
        // if there arent enough tiles, move tiles from boxlid back to tile bag and shuffle.
        }
        else {
            bag->refilTileBag(boxLid);
            repopulateFactoriesAfterRound();
        } 
    }
}

bool Game::gameEnd() {
    bool endGame = false;

    for(int i = 0; i < inputtedPlayerNum; ++i) {
       if(players.at(i)->getMosaic()->checkIfWallRowIsFull()) {
           endGame = true;
       }
    }  

    if(quit == true) {
        endGame = true;
    }  
    
    return endGame;
}

void Game::printFactories(){
    for(int i = 0; i<numFactoriesFromCenters; ++i){
        factories.at(i)->printFactory();
    }
}


void Game::userInputForPlayerTurn(Player* player, int* factoryId, int* colourInt, int* row) {
    bool invalidInput = true;   
    char colour;
    //ask for input
    while(invalidInput){
        std::cout <<player->getName() << " \u25BA ";
        if(std::cin.good()) {
            std::string argument;
            std::cin >> argument;
            if(argument == "turn"){
                std::cin >> *factoryId;
                std::cin >> colour;
                *colourInt = convertCharToColourInt(colour);

                std::cin >> *row;

                /*
                * row is less than wallsize + 1 because if the player inputs a row of 7, this indicates that the tiles 
                * will go to the floor line 
                */
                if((*factoryId < numFactoriesFromCenters && *factoryId >= 0) && (*row <= (WALLSIZE) && *row >= 0) && *colourInt > 0 && *colourInt  < 6){
                    invalidInput = false;
                }else{
                    std::cout << "factoryID must be 0-5 for 1 center factory mode and 0-6 for 2 center factory, row must be 0-5, colour must be 1-5" << std::endl;
                    std::cin.clear();
                }
            }
            else if (argument == "save"){
                std::string name;
                std::cin >> name;
                saveGameFile(name);
            }
            else if(argument == "quit") {
                this->quit = true;
                invalidInput = false;
            }else if(argument == "help"){
                printHelp();
            }else if(argument == "rules"){
                printRules();
            }
            else {
                std::cout << "Invalid Input" << std::endl;
            }
        } else {
            std::cout << "The input format is [int which corresponds to desired factory] [char which corresponds to colour of tiles in factory] [int which corresponds to desired row]" << std::endl;
            std::cin.clear();
            invalidInput = false;
            userInputForPlayerTurn(player, factoryId, colourInt, row);
        }
        //clear input stream before asking for input again.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');        
    }
}

void Game::addFirstPlayerTokenToFloorLine(Player* player, int factoryId) {
    if(factories.at(factoryId)->getNumberTilesOfColour(F) == 1) {
        for(unsigned int i = 0; i < factories.at(factoryId)->getTiles()->size(); i++) {
            if(factories.at(factoryId)->getTiles()->get(i)->getColour() == F) {
                player->getMosaic()->addTileToFloorLine(factories.at(factoryId)->getTiles()->get(i));
                factories.at(factoryId)->getTiles()->remove(i);
            }
        }
        player->setHasFirstPlayerToken(true);
    }
}

//removes first player token from other centres
void Game::removeFromOtherCentre(Player* player){
    for(int x = 0; x < numFactoriesFromCenters; ++x){
       for(unsigned int i = 0; i < factories.at(x)->getTiles()->size(); i++) {
            if(factories.at(x)->getNumberTilesOfColour(F) == 1) {
                factories.at(x)->getTiles()->remove(i);
            }
       }
    }
}

void Game::scorePlayers(){
    for(int i = 0; i<inputtedPlayerNum; ++i){
        for(int row = 0; row < WALLSIZE; ++row){
                    players.at(i)->addScore(row);
        }
    }
}

//to implement load for tiles
void Game::loadGameFile(std::string filename){
    //default file path
    std::string txtFile = "GameFiles/" + filename + ".txt";
    std::fstream readFile;

    unsigned int lineNumber = 0;
    unsigned int* lineNumberPtr = &lineNumber;

    readFile.open(txtFile);
    if(readFile.is_open()){

        //load tile bag
        std::string readTileBag = getNextLine(readFile, lineNumberPtr);
        bag->loadTileBag(readTileBag);
        

        //loads boxlid
        std::string readBoxLid = getNextLine(readFile, lineNumberPtr);
        boxLid->loadBoxLid(readBoxLid);

        //loads factories
        loadFactory(readFile, lineNumberPtr);

        //loads seed
        std::cout << "DEBUG 1" << std::endl;
        unsigned int seed = std::stoi(getNextLine(readFile, lineNumberPtr));

        bag->setSeed(seed);
        bag->randomise();

        //if both tilebag and box lid is empty, create 100 tiles and randomize
        if(readBoxLid == "_" && readTileBag == "_"){
            bag->initialiseTileBag();
            bag->randomise();
        }

        //load players
        std::cout << "LOADING PLAYERS" << std::endl;
        loadPlayer(readFile, lineNumberPtr);
        
        std::cout << players.at(0)->getName() << std::endl;

        //loads PlayerToken
        std::cout << "DEBUG 2" << std::endl;
        int readPlayerToken = std::stoi(getNextLine(readFile, lineNumberPtr));
        players.at(readPlayerToken)->setHasFirstPlayerToken(true);

        readFile.close();
        std::cout << "Successfully loaded the game" << std::endl;
        match();
    } else {
        std::cout << "File does not exist" << std::endl;
    }
}

void Game::saveGameFile(std::string filename){
    std::ofstream writeFile;
    std::string comment;

    //file path
    std::string txtFile = "GameFiles/" + filename + ".txt";


    //If filname is from input
    if(filename.find(".out") != std::string::npos)
        txtFile = "GameFiles/" + filename;

    //if file could not be opened, create a new file
    if (writeFile.fail()) {
    writeFile.open(txtFile, std::fstream::out);
    writeFile.close();
    }

    //opens the file and overwrites the content
    writeFile.open(txtFile);
    if (writeFile.is_open()){

        //save TileBag
        comment = "#TileBag";
        writeFile << comment << std::endl;
        if(bag->getAllTiles().empty())
            writeFile << '_' << std::endl;
        else
            bag->saveTileBag(writeFile);
        //save Box lid
        comment = "#BoxLid";
        writeFile << comment << std::endl;

        //checks if boxlid is empty
        if(boxLid->getTileVector().empty())
            writeFile << '_' << std::endl;
        else
            boxLid->saveBoxLid(writeFile);
        //saves factory
        comment = "#Factories";
        writeFile << comment << std::endl;
        for(int i = 0; i<NUMFACTORIES; ++i){
            factories.at(i)->saveFactory(writeFile);
        }

        //save seed
        comment = "#Seed";
        writeFile << comment << std::endl;        
        writeFile << bag->getSeed() << std::endl;

        //save the players and mosaic board
        int playerTokenId = 0;
        for(int i = 0; i < NUMPLAYERS; i++) {
            savePlayer(writeFile, players.at(i));
            if(players.at(i)->getHasFirstPlayerToken())
                playerTokenId = i;
        }
        comment = "#Player Token";
        writeFile << comment << std::endl;        
        writeFile << playerTokenId << std::endl;

        writeFile.close();
        std::cout << "Game saved successfully" << std::endl;
    }
    else 
        std::cout << "Unable to save file" << std::endl;
}


//returns a line of the file
//need to improve on the code, since everytime the method is called, it searches through the entire file 
//Complexity: Linear
std::string Game::getNextLine(std::fstream& file, unsigned int* num){
    (*num)++;
    std::string fileLine;
    file.seekg(std::ios::beg);
    for(unsigned int i=0; i < *num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    file >> fileLine;
    if(fileLine[0] == '#' || fileLine == ""){
        fileLine = getNextLine(file, num);
    }
    return fileLine;
}


//saves player and mosaic
void Game::savePlayer(std::ofstream& outStream, Player* player){
    std::string comment;
    comment = "#Player";
    outStream << comment << std::endl;
    outStream << player->getName() <<  std::endl;
    outStream << player->getPlayerId() <<  std::endl;

    comment = "#Pattern Line";
    outStream << comment << std::endl;
    player->getMosaic()->savePatternLines(outStream);

    comment = "#Wall";
    outStream << comment << std::endl;
    player->getMosaic()->saveWall(outStream);

    comment = "#Current Score";
    outStream << comment <<  std::endl;
    outStream << player->getScore() <<  std::endl;

    comment = "#FloorLine";
    outStream << comment << std::endl;
    player->getMosaic()->saveFloorLine(outStream);
    outStream <<  std::endl;
}

//loads player and mosaic
void Game::loadPlayer(std::fstream& readFile, unsigned int* lineNumberPtr){
    for(int i = 0; i < inputtedPlayerNum; i++) {
        std::string playerName = getNextLine(readFile, lineNumberPtr);
        int playerId = std::stoi(getNextLine(readFile, lineNumberPtr));
        players.insert(players.end(), new Player(playerId, playerName));

        //loads pattern lines
        std::string patternLines[5] = {getNextLine(readFile, lineNumberPtr), getNextLine(readFile, lineNumberPtr), 
                                    getNextLine(readFile, lineNumberPtr), getNextLine(readFile, lineNumberPtr), 
                                    getNextLine(readFile, lineNumberPtr)};
        players.at(i)->getMosaic()->loadPatternLines(patternLines);

        //loads wall
        std::string wall[5] = {getNextLine(readFile, lineNumberPtr), getNextLine(readFile, lineNumberPtr), 
                            getNextLine(readFile, lineNumberPtr), getNextLine(readFile, lineNumberPtr), 
                            getNextLine(readFile, lineNumberPtr)};
        players.at(i)->getMosaic()->loadWall(wall);

        //loads player score
        int playerScore = std::stoi(getNextLine(readFile,lineNumberPtr));
        players.at(i)->setScore(playerScore);


        //loads floorline
        std::string floorLine = getNextLine(readFile, lineNumberPtr);
        players.at(i)->getMosaic()->loadFloorLine(floorLine);
    }
}

void Game::loadFactory(std::fstream& readFile, unsigned int* lineNumberPtr){
    int count = 0;
    for(int i = 0; i < numFactoriesFromCenters; i++) {
        std::string factoryString = getNextLine(readFile, lineNumberPtr);
        if(factoryString[2] != '_'){
            Factory* factory = new Factory(i);
            factories.insert(factories.end(), factory);
            factories.at(i)->loadFactory(factoryString);
        } else {
            ++count;
        }
    }
    if(count == 6) {
        initFactories();
        
    }
}

bool Game::checkIfAllFactoriesEmpty(){
    std::cout << factories.size() << std::endl;
    bool isEmpty = true;
    for(int i = 0; i < numFactoriesFromCenters; i++) {
        if(factories.at(i)->getTiles()->getHead() != nullptr) {
            if(factories.at(i)->getTiles()->getHead()->getValue()->getColour() == E) {
                factories.at(i)->getTiles()->clear();
            }
        }
    }

    for(int i = 0; i< numFactoriesFromCenters; i++){
        if(factories.at(i)->getTiles()->getHead() != nullptr){
            isEmpty = false;
        }
    }
    return isEmpty;
}

int Game::convertCharToColourInt(char c) {
    c = toupper(c);
    int colourInt = 0;
    if(c == '.') {
        colourInt = 0;
    } else if(c == 'B') {
        colourInt = 1;
    } else if(c == 'Y') {
        colourInt = 2;
    } else if(c == 'R') {
        colourInt = 3;
    } else if(c == 'U') {
        colourInt = 4;
    } else if(c == 'L') {
        colourInt = 5;
    } else if(c == 'F') {
        colourInt = 6;
    }
    return colourInt;
}