#include <iostream>
#include <conio.h>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <vector>

using namespace std;
//Map
const int WIDTH = 10, HEIGHT = 16;
const char OCCUPIED = '#', EMPTY = ' ';

//ANSI escape code colors
const string ANSI_CODES[12] = {
    "\033[33m",  //yellow
    "\033[36m",  //cyan
    "\033[32m",  //green
    "\033[31m",  //red
    "\033[34m",  //blue
    "\033[38;5;214m",    //orange
    "\033[35m",  //purple
    "\033[90m",  //gray
    "\033[94m",  //light blue
    "\033[m",    //white
    "\033[H",    //reset cursor position
    "\033[?25l"  //hide cursor
};
const int  YELLOW = 0, CYAN = 1, GREEN = 2, RED = 3, BLUE = 4, ORANGE = 5, PURPLE = 6, GRAY = 7, LIGHT_BLUE = 8, DEFAULT_COLOR = 9, RESET_CURSOR_POSITION = 10, HIDE_CURSOR = 11;

//Inputs
const int ARROW_RIGHT = 77, ARROW_LEFT = 75, ARROW_UP = 72, ARROW_DOWN = 80, SPACEBAR = 32, SHIFT = 54;

const char shapes[7] = {'O', 'I', 'S', 'Z', 'L', 'J', 'T'};

struct Block{
    int x;
    int y;
    int color;
};

struct Board{
    char displayed;
    int color;
};

void drawBlock(char block){
    switch(block){
        case 'O':
            cout << ANSI_CODES[YELLOW] << "##  \n##  \n";
            break;
        case 'I':
            cout << ANSI_CODES[CYAN] << "####\n    \n";
            break;
        case 'S':
            cout << ANSI_CODES[GREEN] << " ## \n##  \n";
            break;
        case 'Z':
            cout << ANSI_CODES[RED] << "##  \n ## \n";
            break;
        case 'L':
            cout << ANSI_CODES[ORANGE] << "  # \n### \n";
            break;
        case 'J':
            cout << ANSI_CODES[BLUE] << "#   \n### \n";
            break;
        case 'T':
            cout << ANSI_CODES[PURPLE] << " #  \n### \n";
            break;
        default:
            cout << "    \n    \n";
            break;
    }
    cout << ANSI_CODES[DEFAULT_COLOR];
}

void drawBoard(Board board[HEIGHT][WIDTH], Block fallingBlock[4], int score, char blockHeld, vector<char> bag, Block predictedBlock[4]){
    //Flytta musmarkör till övre vänster hörn
    cout << ANSI_CODES[RESET_CURSOR_POSITION] << "NEXT\n";
    if(!bag.empty()) drawBlock(bag[bag.size()-1]);
    else drawBlock(' ');
    //byt färg och skriv ut övre raden
    cout << ANSI_CODES[CYAN] << "== TETRIS ==\n";
    //loopa genom varje rad i spelplanen
    for(int i = 0; i < HEIGHT; i++){
        //skriv ut ram och ändra tillbaka färgen till vitt
        cout << '|' << ANSI_CODES[DEFAULT_COLOR];
        //Loopa genom varje index i raden
        for(int j = 0; j < WIDTH; j++){
            bool fallingBlockPrinted = false;
            //Om positionen är samma som en del av fallande blockets positioner
            for(int k = 0; k < 4; k++){
                if(i == fallingBlock[k].y && j == fallingBlock[k].x){
                    //skriv ut fallande blockets färg och själva blocket
                    cout << ANSI_CODES[fallingBlock[k].color];
                    cout << '#';
                    //Byt tillbaka till vit färg
                    cout << ANSI_CODES[DEFAULT_COLOR];
                    fallingBlockPrinted = true;
                }
            }
            //Kollar om positionen är samma som den predictade positionen
            for(int k = 0; k < 4; k++){
                //Om fallande block inte blivit printat och platsen inte är occupied, printar den det predictade blocket 
                if(i == predictedBlock[k].y && j == predictedBlock[k].x && !fallingBlockPrinted && board[i][j].displayed != OCCUPIED){
                    cout << ANSI_CODES[predictedBlock[k].color]; 
                    cout << '#';
                    cout << ANSI_CODES[DEFAULT_COLOR];
                    fallingBlockPrinted = true;
                }
            }
            //Om ett fallande block inte har skrivit ut skrivs brädet ut istället, med färg
            if(!fallingBlockPrinted){
                cout << ANSI_CODES[board[i][j].color] << board[i][j].displayed << ANSI_CODES[DEFAULT_COLOR];
            }
        }
        //Skriv ut höger sida av ramen
        cout << ANSI_CODES[CYAN] << "|\n";
    }
    //Skriv ut botten ramen
    cout << "============\n";

    cout << ANSI_CODES[DEFAULT_COLOR] << "HOLD\n";
    drawBlock(blockHeld);

    cout << ANSI_CODES[YELLOW] << "  SCORE: " << score << ANSI_CODES[DEFAULT_COLOR] << endl;
}

bool isCollision(Block block[4], Board gameboard[HEIGHT][WIDTH]){
    //Loopar genom varje position i blocket
    for(int i = 0; i < 4; i++){
        //Kollar om någon av de positionerna är upptagna på spelplanen
        if(gameboard[block[i].y][block[i].x].displayed == OCCUPIED || block[i].y >= HEIGHT) return true;
    }
    return false;
}

bool spawnNewBlock(Block newBlock[4], Board gameboard[16][10], vector<char> &bag, bool &holdingLastBlock){
    //Väljer ett random tecken i listan som motsvarar en form
    char blockToSpawn;
    //Hämtar det sista elementet i bagen för att spawna and tar bort det från bagen
    blockToSpawn = bag[bag.size()-1];
    bag.pop_back();
    //om bag är tom fylls den igen
    if(bag.empty()){
        for(int i = 0; i < 7; i++){
            //Väljer ett random block
            char randomBlock = shapes[rand() % 7];
            bool blockInBag = false;
            //Kollar om det redan finns i bagen
            for(auto block : bag){
                if(block == randomBlock){
                    blockInBag = true;
                }
            }
            //blocket inte finns redan läggs det till
            if(!blockInBag){
                bag.push_back(randomBlock);
            } else {
                //Annars loopar den samma igen eftersom bagen inte fylldes
                i--;
            }
        }
    }
    

    switch(blockToSpawn){
        //Beroende på vilken form som ska spawnas
        //så sätts positionerna i det fallande blocket till olika värden
        case 'O':
            // ** 01
            // ** 23
            newBlock[0] = {4, 0, YELLOW};
            newBlock[1] = {5, 0, YELLOW};
            newBlock[2] = {4, 1, YELLOW};
            newBlock[3] = {5, 1, YELLOW};
            break;
        case 'I':
            // **** 1023
            newBlock[0] = {4, 0, CYAN};
            newBlock[1] = {3, 0, CYAN};
            newBlock[2] = {5, 0, CYAN};
            newBlock[3] = {6, 0, CYAN};
            break;
        case 'S':
            //  ** 01
            // ** 23
            newBlock[0] = {4, 0, GREEN};
            newBlock[1] = {5, 0, GREEN};
            newBlock[2] = {3, 1, GREEN};
            newBlock[3] = {4, 1, GREEN};
            break;
        case 'Z':
            // ** 10
            //  ** 23 
            newBlock[0] = {4, 0, RED};
            newBlock[1] = {3, 0, RED};
            newBlock[2] = {4, 1, RED};
            newBlock[3] = {5, 1, RED};
            break;
        case 'L':   
            //   *   3
            // *** 102
            newBlock[0] = {4, 1, ORANGE};
            newBlock[1] = {3, 1, ORANGE};
            newBlock[2] = {5, 1, ORANGE};
            newBlock[3] = {5, 0, ORANGE};
            break;
        case 'J':
            // *   3
            // *** 102
            newBlock[0] = {4, 1, BLUE};
            newBlock[1] = {3, 1, BLUE};
            newBlock[2] = {5, 1, BLUE};
            newBlock[3] = {3, 0, BLUE};
            break;  
        case 'T':
            //  *   3
            // *** 102
            newBlock[0] = {4, 1, PURPLE};
            newBlock[1] = {3, 1, PURPLE};
            newBlock[2] = {5, 1, PURPLE};
            newBlock[3] = {4, 0, PURPLE};
            break;
    }
    //Om det blir kollision med redan liggande block direkt returneras false
    if(isCollision(newBlock, gameboard)) return false;
    holdingLastBlock = false;
    //annars true
    return true;
}

bool moveTetrimino(Block block[4], int delta_x, int delta_y, Board gameboard[HEIGHT][WIDTH]){
    //array för de nya positionerna eftersom orignal blockets riktiga positioner inte 
    //vill ändras innan man kollar om det är giltigt
    Block newPositions[4];
    //Sätter de nya positionerna
    for(int i = 0; i < 4; i++){
        newPositions[i].x = block[i].x + delta_x;
        newPositions[i].y = block[i].y + delta_y;
    }
    //Loopar genom varje position i de nya
    for(auto position : newPositions){
        //Kollar om positionen hamnar utanför kartan eller i en annan tetromino
        if(position.x < 0 || position.x >= WIDTH || isCollision(newPositions, gameboard)){
            return false;
        }
    }
    //Annars sätts det fallande blockets positioner till de nya
    for(int i = 0; i < 4; i++){
        block[i].x = newPositions[i].x;
        block[i].y = newPositions[i].y;
    }
    return true;
}

void rotateTetrimino(Block block[4], Board gameboard[HEIGHT][WIDTH]){
    //Blocket O blir samma om den roteras så inget händer om den formen försöker roteras
    if(block[0].color == YELLOW) return;
    
    //Använda en ruta i blocket som referens de andra snurrar runt
    Block reference = block[0];
    //Sparar blockens placering i förhållande till referens rutan
    Block blocksInReference[4];
    for(int i = 0; i < 4; i++){
        blocksInReference[i] = {block[i].x - reference.x, block[i].y - reference.y};
    }
    //Sätt varje bit i blocket till den roterade positionen i förhållande till referensen
    Block newBlock[4];
    for(int i = 0; i < 4; i++){
        //Sparar positionen för den roterade biten
        newBlock[i] = {reference.x - blocksInReference[i].y, reference.y + blocksInReference[i].x, block[i].color};
        
    }
    //MÅSTE FIXAS
    //Kollar om de roterade positionerna leder till att biten är i en annan bit eller utanför brädet
    
    int delta_y = 0;
    bool invalidPosition = true;
    //Loopar tills det inte är någon invalid positon längre
    while(invalidPosition && delta_y > -16){
        invalidPosition = false;
        //Loopar genom varje bit i blocket
        for(int i = 0; i < 4; i++){
            //Kollar om biten hamnat utanför planen
                //om den är det flyttas den in 
                //men om det inte går sätts invalid position till true
            if(newBlock[i].x < 0){
                if(!moveTetrimino(newBlock, -newBlock[i].x, delta_y, gameboard)){
                    invalidPosition = true;
                }
            }
            if(newBlock[i].x >= WIDTH){
                if(!moveTetrimino(newBlock, -1, delta_y, gameboard)){
                    if(!moveTetrimino(newBlock, -2, delta_y, gameboard)){  
                        invalidPosition = true;
                    }
                }
            }
            if(newBlock[i].y < 0){
                if(!moveTetrimino(newBlock, 0, -newBlock[i].y, gameboard)){
                    invalidPosition = true;
                }
            }
            //kollar om det är kollision (samma position som redan liggande block 
            //är eller under botten av brädet)
            if(isCollision(newBlock, gameboard)){
                //Arrat för vilken ordning x koordinaten ska testas
                int moveOrder[5] = {0, 1, -1, 2, -2};
                bool validMoveFound = false;
                //testar varje ändring i x led
                for(int j = 0; j < 5; j++){
                    if(moveTetrimino(newBlock, moveOrder[i], delta_y, gameboard)){
                        //Om den hittar en förflyttning som går sätts är validMoveFound
                        validMoveFound = true;
                        break;
                    }
                }
                //Om inget validMoveFound sätts invalidPosition till true
                if(!validMoveFound){
                    invalidPosition = true;
                }
            }
        }
        //Om det är invalidPosition ökar minskningen i delta_y. 
        //Loopar igen med förflyttingar uppåt också. Inte bara x led 
        if(invalidPosition) delta_y--;
    }

    //Om ingen position hittades är delta_y -16
    if(delta_y != -16){
        //Annars om en pos hittades ska block få samma koordinater som newBlock
        for(int i = 0; i < 4; i++){
            block[i].x = newBlock[i].x;
            block[i].y = newBlock[i].y;
        }
    }
    
}

void placeFallingBlock(Block block[4], Board gameboard[16][10]){
    //Loopar genom varje placering på blocket 
    //och sätter därefter spelplanens värden på de platserna
    for(int i = 0; i < 4; i++){
        gameboard[block[i].y][block[i].x].displayed = OCCUPIED;
        gameboard[block[i].y][block[i].x].color = block[0].color;
    }
}

void clearLines(Board board[16][10], int &score, int &linesCleared, int &fallingDelay){
    //Loopa genom varje rad
    vector<int> fullLines;
    for(int i = 0; i < HEIGHT; i++){
        //Kollar om det finns en tom ruta i den raden
        bool isLineFull = true;
        for(int j = 0; j < WIDTH; j++){
            if(board[i][j].displayed == EMPTY){
                isLineFull = false;
            }
        }
        if(isLineFull){
            fullLines.push_back(i);
        }
            
    }
    linesCleared += fullLines.size();
    switch(fullLines.size()){
        case 1:
            score += 100;
            break;
        case 2:
            score += 300;
            break;
        case 3:
            score += 500;
            break;
        case 4:
            score += 800;
            break;
    }
    //Loopa genom index varje rad
    for(int l = 0; l < fullLines.size(); l++){
        //Flytta alla ovanliggande rader ett steg neråt
        for(int i = fullLines[l]; i > 0; i--){
            //Loopa från raden precis ovanför den fulla linjen till toppen
            for(int j = 0; j < WIDTH; j++){
                //Raden är lika med den ovanliggande raden
                board[i][j] = board[i-1][j];
            }
        }
    }
    if(!fullLines.empty()){
        system("cls");
        //MÅSTE FIXAS
        //Om linjer rensades kollar den om antalet lines som blivit rensade
        //sedan fallingDelay minskade senast är mer än 1000/fallingDelay
        //Detta gör att linesCleared behöver vara mer ju snabbare spelet går
        //Falling delay får inte vara mindre än 8
        if(linesCleared > 1000/fallingDelay && fallingDelay > 8){
            fallingDelay -= 4;
            linesCleared = 0;
        }
    }
}

void holdBlock(Block block[4], char &blockHeld, vector<char> &bag){
    //Om det finns ett block i Hold läggs det blocket först i bagen
    if(blockHeld != ' ') bag.push_back(blockHeld);
    //Blocket som faller blir holdat
    blockHeld = shapes[block[0].color];
}

void predictPosition(Block predictedBlock[4], Block fallingBlock[4], Board gameboard[16][10]){
    //Loopar genom varje bit i blocket
    for(int i = 0; i < 4; i++){
        //Sätter färg och position
        predictedBlock[i].color = GRAY;
        predictedBlock[i].x = fallingBlock[i].x;
        predictedBlock[i].y = fallingBlock[i].y;
    }
    //Flyttar blocket så långt som möjligt
    while(moveTetrimino(predictedBlock, 0, 1, gameboard));
}
    

int main(){
    //Storlek av plan 10x16
    srand(time(0));
    //2d array som innehåller färgen får platsen och vilket tecken som ska displayas
    Board gameboard[HEIGHT][WIDTH];
    //Sätter brädet index till startvärden
    for(int i = 0; i < HEIGHT; i++){
        for(int j = 0; j < WIDTH; j++){
            gameboard[i][j].displayed = ' ';
            gameboard[i][j].color = DEFAULT_COLOR;
        }
    }

    bool gameover = false;
    int loopCount, score, linesCleared;
    loopCount = score = linesCleared = 0;
    int fallingDelay = 100;
    //Array som innehåller positionerna för varje ruta i tetriminon
    Block fallingBlock[4];
    Block predictedBlock[4];
    //Göm musmarkören
    cout << ANSI_CODES[HIDE_CURSOR];
    //Spawna nytt block och rita spelplanen
    vector<char> bag;
    //Väljer ett random block att starta med
    bag.push_back(shapes[rand() % 7]);
    char blockHeld = ' ';
    //Bool variabel som håller koll om blocket som ligger i hold hamnade där förra blocket
    bool holdingLastblock;
    spawnNewBlock(fallingBlock, gameboard, bag, holdingLastblock);
    drawBoard(gameboard, fallingBlock, score, blockHeld, bag, predictedBlock);
    while(!gameover){
        //Kolla om användaren har tryckt på någon knapp
        if(_kbhit()){
            //char input = getch();
            switch(_getch()){
                case ARROW_RIGHT:
                    //Flytta blocket höger
                    moveTetrimino(fallingBlock, 1, 0, gameboard);
                    break;
                case ARROW_LEFT:
                    //Flytta blocket vänster
                    moveTetrimino(fallingBlock, -1, 0, gameboard);
                    break;
                case ARROW_UP:
                    //Rotera blocket medurs
                    rotateTetrimino(fallingBlock, gameboard);
                    break;
                case ARROW_DOWN:
                    //Flyttar tetrimino ett steg neråt och ökar score med 2
                    if(moveTetrimino(fallingBlock, 0, 1, gameboard)){
                        score += 2;
                        //Återställer loopcount så att det inte blir dubbel nerflyttning
                        //pga automatiska
                        loopCount = 1;
                    }
                    break;
                case 'w':
                case 'W':
                    if(!holdingLastblock){
                        //Sätter att det förra blocket blev "holdat" för att inte
                        //Kunna göra det 2 gånger i rad med samma block
                        holdBlock(fallingBlock, blockHeld, bag);
                        spawnNewBlock(fallingBlock, gameboard, bag, holdingLastblock);
                        holdingLastblock = true;
                    }
                    break;
                case SPACEBAR:
                    //flyttar tetrimino neråt tills den inte kan längre
                    while(moveTetrimino(fallingBlock, 0, 1, gameboard)){
                        score += 2;
                    };
                    //placerar blocket på spelplanen
                    placeFallingBlock(fallingBlock, gameboard);
                    clearLines(gameboard, score, linesCleared, fallingDelay);
                    //Spawnar nytt fallande block 
                    //men om det inte går sätts gameover till true
                    if(!spawnNewBlock(fallingBlock, gameboard, bag, holdingLastblock)){
                        gameover = true;
                    }
                    break;
                case 'p':
                    cout << ANSI_CODES[RESET_CURSOR_POSITION] << "\033[11;4HPAUSED\nPRESS p TO CONTINUE";
                    while(getch() != 'p');
                    system("cls");
                    break;
            }
        }
        predictPosition(predictedBlock, fallingBlock, gameboard);
        //För var x:e loop ska det fallande blockets position minska ett steg
        //Lösning istället för thread eftersom vi inte lärt oss det på lektionen
        if(loopCount % fallingDelay == 0){ 
            if(!moveTetrimino(fallingBlock, 0, 1, gameboard)){
                placeFallingBlock(fallingBlock, gameboard);
                //Rensar linjer om det är fulla
                clearLines(gameboard, score, linesCleared, fallingDelay);
                //Spawnar ett nytt fallande block
                if(!spawnNewBlock(fallingBlock, gameboard, bag, holdingLastblock)){
                    //Men om den inte lyckas spawna nytt block är det gameover
                    gameover = true;
                }
            } else {
                score++;
            }
        }
        drawBoard(gameboard, fallingBlock, score, blockHeld, bag, predictedBlock);
        loopCount++;
    }
    cout << ANSI_CODES[RED] << "GAMEOVER";
    return 0;
}