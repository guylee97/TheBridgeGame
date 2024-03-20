/*
제목 : Bridge Game
Written by : 이주엽
*/





#include <iostream>
#include <string>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <vector>
#include <filesystem>
#include <io.h>
#include <direct.h>
#include <fstream>
#include <sstream>
#include <random>

using namespace std;


#pragma warning(disable: 4996)                                                 //define for keycontrol and gui draw
#define WIDTH 80
#define HEIGHT 40
#define UP 0
#define RIGHT 1
#define LEFT 2
#define DOWN 3
#define ENTER 4

int numOfPlayers = 2;                                                         //game's critical settings (플레이어 수,선택된 맵, 현재 플레이어.. etc
string selectMap = "default.map";
int display = 0;                 //0.main 1.setting 2.Ingame 3.result
char curDir[1000];
int selectedPlayer = 1;
int dice = 1;
int goaledPlayer = 0;
string direction = "";
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dis(1, 6);


void gotoXY(int x, int y, string s) {                                       //used for draw console
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    cout << s;
};

class player {                                                             //player class
 public:
     int index=0;
     int score = 0;
     int pos = 0;
     int cards = 0;
     boolean goalin = false;
     player(int i) {
         index = i;
     }


};

class mapCell {                                                           //map cell class
public:
    string cell[3] = {"","",""}; //if input line is C U D then cell[0]==C,cell[1]==U... etc
    int pos;
    int disBridge=0;             //distance from bridge end,start cell
    boolean connectedBridge = false;  //is this bridge end cell?
    char ch[1];

    mapCell(string line,int i) {
        this->pos = i;
        vector<std::string> parse;
        string token;
        istringstream ss(line);      
        while (std::getline(ss, token, ' ')) parse.push_back(token);
            cell[0]=parse[0];
            for (int i = 1; i < parse.size(); i++) {
                if (parse[i] == "U") cell[i] = "U";
                else if (parse[i] == "D") cell[i] = "D";
                else if (parse[i] == "R") cell[i] = "R";
                else if (parse[i] == "L") cell[i] = "L";
                
            }   
   }

    mapCell(int i) {
        this->pos = i;
        cell[0] = "=";
    }
    
    void drawcell(vector<player> players,int x,int y) {        //method to draw cell using x,y coordinate
        gotoXY(x, y, "[0000");
        strcpy(ch, cell[0].c_str());
        cout << ch << "]";
        for (int i = 1; i <= players.size(); i++) {
            if (players[i-1].index == i&&this->pos== players[i - 1].pos) {
                gotoXY(x+i, y, "");
                cout << i;
            }
            else if (players[i - 1].index == i && players[i - 1].goalin == true && this->cell[0][0]=='E') {
                gotoXY(x + i, y, "");
                cout << "@";
            }
        }
    }
};

vector<player> playerInfo;        //players vector
vector<mapCell> cells;           //cell vector


void goalIn() {                                          //goalin function
    int j = 0;
    gotoXY(50, 8, "                                   ");
    gotoXY(50, 9, "                                   ");
    gotoXY(50, 10, "                                  ");
    gotoXY(50, 11, "                                  ");
    playerInfo[selectedPlayer-1].goalin = true;
    switch (goaledPlayer) {
    case 0:playerInfo[selectedPlayer - 1].score += 7; break;
    case 1:playerInfo[selectedPlayer - 1].score += 3; break;
    case 2:playerInfo[selectedPlayer - 1].score += 1; break;
    }
    goaledPlayer++;
};

int calPPosition(int pos) {
    int j = 0;
    while (playerInfo[j].pos != pos) {
        j++;
    }
    return j;
}

int calCPosition(int pos) {
    int j = 0;
    while (cells[j].pos != pos) {
        j++;
    }
    return j;
}


boolean throwDice(player pl, vector<mapCell> cl,int j) { //throw dice function. return ==true then,throwing dice and move perfectly.false means go back to action select
    
    int pos = pl.pos;
    int cards = pl.cards;
    boolean flag = false;
    int k = 0;
   
    gotoXY(50, 8, "Q입력시, 행동 선택으로 돌아가기"); 
    gotoXY(50, 9, "주사위 눈: "); cout << dice;
    gotoXY(50, 10, "주사위 눈-브릿지 카드: "); dice = dice - pl.cards;  cout << dice;
    while (1) {
        direction = "";
        pos = pl.pos;
        cards = pl.cards;

        gotoXY(50, 11, "방향 명령(UDLR 입력): "); getline(cin, direction);
        if (direction[0] == 'Q') {
            system("cls");
            return false;
        }
        if (direction.length() != dice) {
            cout << "length error";
            gotoXY(50, 11, "                                  ");
            continue;
        }
        
        for (int i = 0; i < direction.length(); i++) {
            j = 0;
            while (cl[j].pos != pos) {
                j++;
            }

            if (direction[i] != cl[j].cell[1][0] && direction[i] != cl[j].cell[2][0]) {
                if ((cl[j].cell[0][0] == 'B'|| cl[j].cell[0][0] == '=') && direction[i] == 'R' && cl[j].pos != 0) {
                    if (cl[j].cell[0][0] == 'B') { pos += 1000; cards++; continue; }
                    else if (cl[j].cell[0][0] == '=') { pos = pos - 1000 + cl[j].disBridge; continue;
                    }
                   
                }

                else if ((cl[j].cell[0][0] == 'b' || cl[j].cell[0][0] == '=') && direction[i] == 'L' && cl[j].pos != 0) {
                    if (cl[j].cell[0][0] == 'b') { pos = pos - cl[j].disBridge + 1000; cards++; continue;
                    }
                    else if (cl[j].cell[0][0] == '=') { pos = pos - 1000; continue;
                    }
                }

                else if (cl[j].cell[0][0] =='E') {
                    k = 0;
                    while (cl[k].pos != pos-1) {
                        k++;
                    }
                    
                    switch (cl[k].cell[2][0]) {
                    case 'U': if (direction[i] == 'D') { pos--; continue; }
                            else { goalIn();  playerInfo[selectedPlayer - 1].pos = 800; playerInfo[selectedPlayer - 1].cards = 0; return true; }
                    case 'R': if (direction[i] == 'D') { pos--; continue;}
                            else { goalIn();  playerInfo[selectedPlayer - 1].pos = 800; playerInfo[selectedPlayer - 1].cards = 0; return true; }
                    case 'L': if (direction[i] == 'D') { pos--; continue;                   }
                            else { goalIn();  playerInfo[selectedPlayer - 1].pos = 800; playerInfo[selectedPlayer - 1].cards = 0; return true;}
                    case 'D': if (direction[i] == 'D') { pos--; continue;                   }
                            else { goalIn();  playerInfo[selectedPlayer - 1].pos = 800; playerInfo[selectedPlayer - 1].cards = 0; return true; }
                    }
                 
                }

                else {
                    flag = true;
                    cout << "invalid order"; gotoXY(50, 11, "                                  ");
                    break;
                }
            }

            if (direction[i] == cl[j].cell[1][0]&&cl[j].pos!=0){
                if (goaledPlayer > 0) {
                    flag = true;
                    cout << "invalid order"; gotoXY(50, 11, "                                  ");
                    break;
                }

                switch (direction[i]) {
                case 'U': pos--; break;
                case 'R': pos--; break;
                case 'L':if (cl[j].cell[0][1] == 'b') { pos = pos - cl[j].disBridge + 1000; cards++; }
                        else if (cl[j].cell[0][1] == '=') { pos = pos - 1000; }
                        else pos--;
                         break;
                case 'D': pos--; break;
                }
                
            }

            else if (direction[i] == cl[j].cell[2][0]|| cl[j].pos == 0) {
                
                switch (direction[i]) {
                case 'U': pos++; break;
                case 'R': if (cl[j].cell[0][1] == 'B') { pos += 1000; cards++;}
                        else if (cl[j].cell[0][1] == '=') { pos = pos - 1000 + cl[j].disBridge; }
                        else pos++;
                         break;
                case 'L': pos++; break;
                case 'D': pos++; break;
                }
               
            }

       }   
        if (flag == true) {
            flag = false;
            continue;
        }
        flag = false;
        break;
    }
    playerInfo[selectedPlayer-1].pos = pos;
    playerInfo[selectedPlayer - 1].cards = cards;
    j = 0;
    while (cells[j].pos != pos) {
        j++;
    }

    if (cells[j].cell[0] == "S") {
        playerInfo[selectedPlayer - 1].score += 3;
        cells[j].cell[0] = "C";
    }
    else if (cells[j].cell[0] == "H") {
        playerInfo[selectedPlayer - 1].score += 2;
        cells[j].cell[0] = "C";
    }
    else if (cells[j].cell[0] == "P") {
        playerInfo[selectedPlayer - 1].score += 1;
        cells[j].cell[0] = "C";
    }
    
    gotoXY(50, 8, "                                      ");
    gotoXY(50, 9, "                                      ");
    gotoXY(50, 10, "                                     "); 
    gotoXY(50, 11, "                                     "); 

    
    return true;
};

void stay(player pl) {                                 //stay function
    if (pl.cards > 0) {
        playerInfo[selectedPlayer - 1].cards--;
    }
   
    return;
};


int keyControl() {                                     //used to key control
    char temp=_getch();

    if (temp == 224) {
        temp = _getch();
        switch (temp) {
        case 72: return UP; break;
        case 75: return LEFT; break;
        case 77: return RIGHT; break;
        case 80: return DOWN; break;
        }
    }
    if (temp == 'w' || temp == 'W') return UP;
    else if(temp == 'a' || temp == 'A') return LEFT;
    else if (temp == 'd' || temp == 'D')return RIGHT;
    else if (temp == 's' || temp == 'S')return DOWN;
    else if (temp == 13)return ENTER;
};






void drawMain() {                                          //below functions are used to draw
    int opt;
    int x = WIDTH / 2 ;
    int y = HEIGHT / 2 - 2 + 10;
    gotoXY(WIDTH / 2, HEIGHT / 2 - 2, "BRIGE GAME");
    gotoXY(x-2, y, "> 게임 시작");
    gotoXY(x,y+ 1, "종료");

    while (1) {
        opt = keyControl();
        if (y == HEIGHT / 2 - 2 + 10) {
            if (opt == DOWN) {
                gotoXY(x-2, y, " ");
                y++;
                gotoXY(x-2, y, ">");
            }

            else if (opt == ENTER) {
                display = 1;
                system("cls");
                break;
            }

        }

        else if (y == HEIGHT / 2 - 2 + 11) {
            if (opt == UP) {
                gotoXY(x-2, y, " ");
                y--;
                gotoXY(x-2, y, ">");
            }

            else if (opt == ENTER) {
                exit(1);
            }
        }
    }
};

void drawSettings() {
    int opt;
    _finddata_t fd;
    long handle;
    int result = 1;
    handle = _findfirst("*.map", &fd);
    if (handle == -1)
    {
        selectMap = "there is no map!!";
        exit(1);
    }


    int x = WIDTH / 2 ;
    int y = HEIGHT / 2;
    gotoXY(WIDTH / 2 - 2, y, "> 플레이어 수 : "); cout << numOfPlayers;
    gotoXY(WIDTH / 2, y + 1, "현재 맵 : "); cout << selectMap;
    gotoXY(WIDTH / 2, y + 2, "설정 완료");
    while (1) {
        opt = keyControl();
        switch (opt) {
        case DOWN:
            if (y < HEIGHT / 2 + 2) {
                gotoXY(x - 2, y, " ");
                y++;
                gotoXY(x - 2, y, ">");
                break;
            }
            break;
        case UP:
            if (y > HEIGHT / 2) {
                gotoXY(x - 2, y, " ");
                y--;
                gotoXY(x - 2, y, ">");
                break;
            }
            break;
        case RIGHT:
            if (y == HEIGHT / 2 && numOfPlayers < 8) {
                numOfPlayers++;
                gotoXY(x + 13, y, " "); cout << numOfPlayers;
                break;
            }
            else if (y == HEIGHT / 2+1) {
                selectMap = fd.name;
                gotoXY(x + 9, y, "                      "); gotoXY(x + 9, y, " "); cout << selectMap;
                if (result == -1) {
                    handle = _findfirst("*.map", &fd);
                    if (handle == -1)
                    {
                        selectMap = "there is no map!!";
                        exit(1);
                    }
                    selectMap = fd.name;
                    gotoXY(x + 9, y, "                      "); gotoXY(x + 9, y, " "); cout << selectMap;
                }
                result = _findnext(handle, &fd);
                break;
            }
            break;

        case LEFT:
            if (y == HEIGHT / 2&&numOfPlayers>1) {
                numOfPlayers--;
                gotoXY(x + 13, y, " "); cout << numOfPlayers;
                break;
            }
            break;

        case ENTER:
            if (y == HEIGHT / 2+2) {
                display = 2;
                system("cls");
                return;
                break;
            }
            break;
        }
    }
};

void drawGame() {
    int opt;
    int x = 5;
    int y = 15;
    gotoXY(3, 2, "조작법");
    gotoXY(3, 3, "R:주사위 굴리기, T:휴식(bridge 카드 한장 반납)");
    gotoXY(3, 5, "Player1: "); cout << "score: " << playerInfo[0].score << "   cards: " << playerInfo[0].cards;
    gotoXY(3, 6, "Player2: "); cout << "score: " << playerInfo[1].score << "   cards: " << playerInfo[1].cards;
    gotoXY(WIDTH/2, 5, "Player3: "); if(numOfPlayers>2){ cout << "score: " << playerInfo[2].score << "   cards: " << playerInfo[2].cards; }
    gotoXY(WIDTH / 2, 6, "Player4: "); if (numOfPlayers>3) { cout << "score: " << playerInfo[3].score << "   cards: " << playerInfo[3].cards; }
    gotoXY(3, 9, "현재 플레이어 : player"); cout << selectedPlayer;

    cells[0].drawcell(playerInfo,x,y);
    if (cells[0].cell[1] == "U") {
        y--;
    }
    else if (cells[0].cell[1] == "R") {
        x += 7;
    }
    else if (cells[0].cell[1] == "L") {
        x -= 7;
    }
    else if (cells[0].cell[1] == "D") {
        y++;
    }


    for (int i = 1; i < cells.size()-1; i++) {

        if (cells[i].cell[0] == "B") {
            cells[i-1].drawcell(playerInfo, x+7, y);
        }

        if (cells[i].cell[2] == "U") {
            cells[i].drawcell(playerInfo, x, y);
            y--;
        }
        else if (cells[i].cell[2] == "R") {
            cells[i].drawcell(playerInfo, x, y);
            x += 7;
        }
        else if (cells[i].cell[2] == "L") {
            cells[i].drawcell(playerInfo, x, y);
            x -= 7;
        }
        else if (cells[i].cell[2] == "D") {
            cells[i].drawcell(playerInfo, x, y);
            y++;
        }

        
    }

    cells[cells.size()-1].drawcell(playerInfo, x, y);
    return;
};

void drawResult() {
    int max, i,opt;
    gotoXY(3, 5, "Player1: "); cout << "score: " << playerInfo[0].score << "   cards: " << playerInfo[0].cards;
    gotoXY(3, 6, "Player2: "); cout << "score: " << playerInfo[1].score << "   cards: " << playerInfo[1].cards;
    gotoXY(WIDTH / 2, 5, "Player3: "); if (numOfPlayers > 2) { cout << "score: " << playerInfo[2].score << "   cards: " << playerInfo[2].cards; }
    gotoXY(WIDTH / 2, 6, "Player4: "); if (numOfPlayers > 3) { cout << "score: " << playerInfo[3].score << "   cards: " << playerInfo[3].cards; }

    for (i = 0; i < numOfPlayers-1; i++) {
        if (playerInfo[i].score >= playerInfo[i + 1].score)
            max = i+1;
        else max = i + 2;
    }



    gotoXY(3, 10, "Player "); cout << max << " is win!";



    while (1) {
        opt = keyControl();
     
        switch (opt) {
        case 'R':break;
           
        case 'T':break;
            
        }
    }
};



void gamePlaying() {                               //used to game playing
    int opt, i = 0;
    boolean temp = false;
    if (numOfPlayers - goaledPlayer == 1) {
        display = 3;
        system("cls");
        return;
    }
    dice = dis(gen);

    while (1) {
        opt = keyControl();
        while (cells[i].pos != playerInfo[selectedPlayer-1].pos) {
            i++;
        }
        if (opt != 'R' && opt != 'T') { continue; }
        switch (opt) {
        
        case 'R':
            temp=throwDice(playerInfo[selectedPlayer-1], cells,i);
            if (temp)break; else drawGame(); continue;
           
        case 'T':
            stay(playerInfo[selectedPlayer-1]);
            cout << numOfPlayers << "  " << selectedPlayer;
           
            break;
        }

        
        do {
            if (selectedPlayer == numOfPlayers) {
                selectedPlayer = 1;
            }
            else {
                selectedPlayer++;
            }
        } while (playerInfo[selectedPlayer - 1].goalin == true);
        return;
    }
};



void gameSetting() {                               //used to game setting
    int i = 0;

    ifstream fin(selectMap);
    if (!fin.is_open())
    {
        display = 2;
        cout << "Map opening error";
        return;
    }

    while (!fin.eof())
    {
        int distance = 0;
        
        std::string line;
        std::getline(fin, line);
        if (line.empty()) break;

        string token;
        istringstream ss(line);
        getline(ss, token, ' ');
        if (token == "B") {
            mapCell c(i+1000);
            cells.push_back(c);
        }


        mapCell s(line, i);
        i++;
        s.disBridge = distance;
        cells.push_back(s);
    }

    int bridgeCount = 0;
   
    for (i = 0; i < cells.size(); i++) {
        bridgeCount = 0;
        if (cells[i].cell[0] == "B") {
            bridgeCount++;
            int j = i;
            
            while (1) {
                j++;
                if (cells[j].cell[0] == "B") {
                    bridgeCount++;
                }

                if (cells[j].cell[0] == "b") {
                    bridgeCount--;
                }


                if (cells[j].cell[0] == "b" && cells[j].connectedBridge == false&&bridgeCount==0) {
                    cells[j].connectedBridge = true;
                    cells[i].disBridge = cells[j].pos - cells[i].pos;
                    cells[j].disBridge = cells[j].pos - cells[i].pos;
                    cells[i - 1].disBridge = cells[j].pos - cells[i].pos;
                   
                    break;
                }
            }
        }
    }


    for (int i = 1; i <= numOfPlayers; i++) {
        player pr(i);
        playerInfo.push_back(pr);
    }

};


int main(int argc, char** argv)
{
    while (1) {
        switch (display) {
        case 0: drawMain();
        case 1: drawSettings();
        case 2: gameSetting();  while (display == 2) { drawGame(); gamePlaying(); }
        case 3: drawResult();
        
        }
    }
    system("pause");
}













