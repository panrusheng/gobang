#ifndef GAME_H
#define GAME_H

#include <vector>
#include <time.h>
#include <chrono>
#include <cstdlib>
//using namespace std;
using namespace std::chrono;
class Game
{
public:
    Game();
    Game(const double);
    ~Game();
    void timerStart();
    void timerStop();
    void timerDiff();
    //virtual void timer() const;
    //virtual void fall() const = 0;
    void clear();
    void setTotalTime(const double);
    void setStatus(int status[15][15]);
    void getStatus(int status[15][15]);
    double getTimer();
private:
    double mTime = 10000; // the remaining time
    int chessBoard[15][15];//Simulated chessboard; 1:Black,-1:White 0:None
    milliseconds start, stop;
};

#endif // GAME_H
