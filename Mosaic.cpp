#include "Mosaic.h"

Mosaic::Mosaic() {
    initWall();
    initPatternLines();
    initFloorLine();
}

Mosaic::~Mosaic() {   
    delete wall;
    delete patternLines;
}

void Mosaic::initFloorLine() {
    this->floorLine = new Tile[FLOORLINESIZE];
    floorLine[0] = *new Tile(0);
}

void Mosaic::initPatternLines() {
    /*
    *   ----T
    *   ---TT
    *   --TTT
    *   -TTTT
    *   TTTTT
    */

    this->patternLines = new Tile*[WALLSIZE];


    for(int row = 0; row < WALLSIZE; ++row) {
        this->patternLines[row] = new Tile[row + 1];
    }

    //SET EACH TILE TO BE 'EMPTY'   
    for(int row = 0; row < WALLSIZE; ++row) {
        for(int col = 0; col < (row + 1); ++col) {
            patternLines[row][col] = *new Tile(0);
        }
    }
}

void Mosaic::addTileToPatternLines(Tile* tile, int row) {
    //this->patternLines[row]
    bool tileAdded = false;
    for(int i = row; i >= 0; --i) {
        if(this->patternLines[row][i].getColour() == E && tileAdded == false) {
            //this->patternLines[row][i].setColour(tile->getColour());
            this->patternLines[row][i] = *tile;
            tileAdded = true;
        }       
    }
    
}

bool Mosaic::addTileToFloorLine(Tile* tile) {
    bool added = false;
    for(int i = 0; i < FLOORLINESIZE; i++) {
        if(floorLine[i].getColour() == E && added == false) {
            floorLine[i].setColour(tile->getColour());
            added = true;
        }
    }
    return added;
}

int Mosaic::getColourOfTilesInPatternLineRow(int row) {
    int length = row + 1;
    int colour = 0;
    for(int i = 0; i < length; i++) {
        if(patternLines[row][i].getColour() != E) {
            colour = patternLines[row][i].getColour();
        }
    }
    return colour;
}

bool Mosaic::doesWallRowContainColour(int colour, int row) {
    bool doesContain = false;
    for(int col = 0; col < WALLSIZE; ++col) {
        if(wall[row][col].getColour() == colour) {
            doesContain = true;
        }
    }
    return doesContain;
}

int Mosaic::numberOfFreeSpacesOnPatternLineRow(int row) {
    int length = row + 1;
    int count = 0;
    for(int i = 0; i < length; i++) {
        if(patternLines[row][i].getColour() == E) {
            count++;
        }
    }
    return count;
}

bool Mosaic::allTilesAroundTileEmpty(int row, int col){
    bool isAllEmpty = true;

    if(col-1 >= 0 && wall[row][col-1].getColour() != E){
        isAllEmpty = false;
        //check up
    }
    if(col + 1 < 5 && wall[row][col+1].getColour() != E){
        isAllEmpty = false;
        //check down
    }
    if(row-1 >= 0 && wall[row-1][col].getColour() != E){
        isAllEmpty = false;
        //check left
    }
    if(row + 1 < 5 && wall[row+1][col].getColour() != E){
        isAllEmpty = false;
        //check right
    }

    return isAllEmpty;
}

int Mosaic::numberOfFreeSpacesOnFloorLine() {
    int count = 0;

    for(int i = 0; i < FLOORLINESIZE; i++) {
        if(floorLine[i].getColour() == E) {
            count++;
        }
    }

    return count;
}

void Mosaic::initWall() {
    
    //CREATE WALL WHICH HOLDS AN ARRAY OF 5 TILE POINTERS (5 Cols)
    this->wall = new Tile*[WALLSIZE];

    //MAKE EACH ROW HOLD AN ARRAY OF 5 TILES (1 Row)
    for(int row = 0; row < WALLSIZE; ++row) {
        this->wall[row] = new Tile[WALLSIZE];
    }

    //SET EACH TILE TO BE 'EMPTY'
    for(int row = 0; row < WALLSIZE; ++row) {
        for(int col = 0; col < WALLSIZE; ++col) {
            wall[row][col] = *new Tile(0);
        }
    }
}

void Mosaic::printWallRow(int row) {
    for(int col = 0; col < WALLSIZE; ++col) {
        //std::cout << wall[row][col].getColour();
        int colour = wall[row][col].getColour();
        if(colour == E){
            std::cout << convertEmptyToColouredString(row, col);
        }else{
            std::cout << convertIntToColouredString(colour);
        }  
        std::cout << " ";
    }
    std::cout << std::endl;
}

void Mosaic::printWall() {
    for(int row = 0; row < WALLSIZE; ++row) {
        for(int col = 0; col < WALLSIZE; ++col) {
            std::cout << wall[row][col].getColour();
        }
        std::cout << std::endl;
    }
}

void Mosaic::printPatternLineRow(int row) {
        //PRINT OUT SPACES
        std::cout << row << ": ";
        for(int i = (WALLSIZE - row - 1); i > 0; --i) {
            std::cout << " ";
        }
        
        //PRINTING OUT TILE
        for(int col = 0; col < (row + 1); ++col) {     
            //std::cout << patternLines[row][col].getColour();
            int colour = patternLines[row][col].getColour();
            std::cout << convertIntToColouredString(colour);
        }
}

void Mosaic::printPatternLines() {
    for(int row = 0; row < WALLSIZE; ++row) {
        //PRINT OUT SPACES
        for(int i = (WALLSIZE - row - 1); i > 0; --i) {
            std::cout << " ";
        }
        
        //PRINTING OUT TILE
        for(int col = 0; col < (row + 1); ++col) {     
            std::cout << patternLines[row][col].getColour();
        }
        std::cout << std::endl;
    }
}

void Mosaic::printFloorLine() {
    for(int i = 0; i < FLOORLINESIZE; i++) {
        //std::cout << floorLine[i].getColour();
        int colour = floorLine[i].getColour();
        std::cout << convertIntToColouredString(colour);
    }   
    std::cout << std::endl;
}

Tile** Mosaic::getPatternLines() {
    return this->patternLines;
}

char Mosaic::convertIntToColourChar(int colour) {
    char colourChar = '.';
    if(colour == E) {
        colourChar = '.';
    } else if(colour == B) {
        colourChar = 'B';
    } else if(colour == Y) {
        colourChar = 'Y';
    } else if(colour == R) {
        colourChar = 'R';
    } else if(colour == U) {
        colourChar = 'U';
    } else if(colour == L) {
        colourChar = 'L';
    } else if(colour == F) {
        colourChar = 'F';
    }
    return colourChar;
}
std::string Mosaic::convertEmptyToColouredString(int row, int col){
    int coloursFromWall[5][5] = {{1, 2, 3, 4, 5},
                                 {5, 1, 2, 3, 4},
                                 {4, 5, 1, 2, 3},
                                 {3, 4, 5, 1, 2},
                                 {2, 3, 4, 5, 1}};

    std::string colouredEmpty = "";

    int colour = coloursFromWall[row][col];

    if(colour == R) {
        colouredEmpty += BOLDRED;
        colouredEmpty += "\u25CB";
        colouredEmpty += COLOURENDING;
    } else if(colour == Y) {
        colouredEmpty += BOLDYELLOW;
        colouredEmpty += "\u25CB";
        colouredEmpty += COLOURENDING;
    } else if(colour == B) {
        colouredEmpty += BOLDBLUE;
        colouredEmpty += "\u25CB";
        colouredEmpty += COLOURENDING;
    } else if(colour == L) {
        colouredEmpty += BOLDCYAN;
        colouredEmpty += "\u25CB";
        colouredEmpty += COLOURENDING;
    }else if(colour == U) {
        colouredEmpty += BOLDBLACK;
        colouredEmpty += "\u25CB";
        colouredEmpty += COLOURENDING;
    }

    return colouredEmpty;
}


std::string Mosaic::convertIntToColouredString(int colour){
        std::string colouredString = "";
        if(colour == E){    
            colouredString = "\u25CB";
        } else if(colour == R) {
            colouredString += BOLDRED;
            colouredString += "R";
            colouredString += COLOURENDING;
        } else if(colour == Y) {
            colouredString += BOLDYELLOW;
            colouredString += "Y";
            colouredString += COLOURENDING;
        } else if(colour == B) {
            colouredString += BOLDBLUE;
            colouredString += "B";
            colouredString += COLOURENDING;
        } else if(colour == L) {
            colouredString += BOLDCYAN;
            colouredString += "L";
            colouredString += COLOURENDING;
        }else if(colour == U) {
            colouredString += BOLDBLACK;
            colouredString += "U";
            colouredString += COLOURENDING;
        } else if(colour == F) {
            colouredString += 'F';
            colouredString += "";
        }
        return colouredString;
}

int Mosaic::convertCharToColourInt(char c) {
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

//Saves wall to the save file
void Mosaic::saveWall(std::ofstream& printToFile){
    for(int row = 0; row < WALLSIZE; ++row) {
        for(int col = 0; col < WALLSIZE; ++col) {
            int colour = wall[row][col].getColour();
            printToFile << convertIntToColourChar(colour);
        }
        printToFile << std::endl;
    }
}

//Saves pattern lines to the save file
void Mosaic::savePatternLines(std::ofstream& printToFile) {
    for(int row = 0; row < WALLSIZE; ++row) {
        //PRINTING OUT TILE
        for(int col = 0; col < (row + 1); ++col) {     
             int colour = patternLines[row][col].getColour();
             printToFile << convertIntToColourChar(colour);
        }
        printToFile << std::endl;
    }
}

void Mosaic::saveFloorLine(std::ofstream& printToFile){
        for(int i = 0; i < FLOORLINESIZE; i++) {
            int colour = floorLine[i].getColour();
            printToFile << convertIntToColourChar(colour);
    } 
}

//Creates pattern lines from the save file
void Mosaic::loadPatternLines(std::string* data){
    for(int row = 0; row < WALLSIZE; ++row) {
        for(int col = 0; col < (row + 1); ++col) {
            char c = data[row][col];
            int tileColour = convertCharToColourInt(c);
            patternLines[row][col] = *new Tile(tileColour);
        }
    }
}

//Creates wall from the save file
void Mosaic::loadWall(std::string* data){
    //CREATE WALL WHICH HOLDS AN ARRAY OF 5 TILE POINTERS (5 Cols)
    this->wall = new Tile*[WALLSIZE];

    //MAKE EACH ROW HOLD AN ARRAY OF 5 TILES (1 Row)
    for(int row = 0; row < WALLSIZE; ++row) {
        this->wall[row] = new Tile[WALLSIZE];
    }

    //SET EACH TILE TO BE 'EMPTY'
    for(int row = 0; row < WALLSIZE; ++row) {
        for(int col = 0; col < WALLSIZE; ++col) {
            char c = data[row][col];
            int tileColour = convertCharToColourInt(c);
            wall[row][col] = *new Tile(tileColour);
        }
    }
}
//Creates floor line from the save file
void Mosaic::loadFloorLine(std::string data){
    this->floorLine = new Tile[FLOORLINESIZE];

    for(int col = 0; col < FLOORLINESIZE; ++col){
        char c = data[col];
        int tileColour = convertCharToColourInt(c);
        floorLine[col] = *new Tile(tileColour);
    }
}

bool Mosaic::checkIfWallRowIsFull(){
    int filledTileInt = 0;
    bool filledRow = false;
    for(int row = 0; row < WALLSIZE; row++) {
        for(int col = 0; col < WALLSIZE; col++) {
            if(wall[row][col].getColour() != E) {
                filledTileInt++;
            }
        }
        if(filledTileInt == WALLSIZE) {
            filledRow = true;
        }
        filledTileInt = 0;
    }
    return filledRow;
}

bool Mosaic::checkPatternLineFull(int row){
    bool isFull = true;
    int length = row + 1;
    for(int col = 0; col < length; ++col){
        if(patternLines[row][col].getColour() == E){
            isFull = false;
        }
    }
    return isFull;
}

int Mosaic::countEmptyTilesInWallRow(int row){
    int count = 0;
    for(int j = 0; j < WALLSIZE; ++j){
        if(wall[row][j].getColour() == E){
            count++;
        }
    }
    return count;
}

int Mosaic::countEmptyTilesInPatternLineRow(int row){
    int count = 0;
    
    for(int j = 0; j < (row + 1); ++j){
        if(patternLines[j]->getColour() == E){
            count++;
        }
    }

    return count;
}


bool Mosaic::everythingEmpty(){
    bool empty = true;

    for(int i = 0; i < WALLSIZE; ++i){
        for(int j = 0; j < WALLSIZE; ++j){
            if(wall[i][j].getColour() == E && empty == true){
                empty = true;
            }else{
                empty = false;
            }
        }
    }
    
    return empty;
}





Tile** Mosaic::getWall(){
    return this->wall;
}

Tile* Mosaic::getFloorLine() {
    return this->floorLine;
}

