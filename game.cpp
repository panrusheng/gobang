#include "game.h"

Game::Game()
{
    for(int i=0;i<15;i++)
    {
        for(int j=0;j<15;j++)
        {
            chessBoard[i][j] = 0;
        }
    }
}

Game::Game(const double totalTime)
{
    mTime = totalTime;
    for(int i=0;i<15;i++)
    {
        for(int j=0;j<15;j++)
        {
            chessBoard[i][j] = 0;
        }
    }
}

Game::~Game()
{
}

void Game::clear()
{
    mTime = 10000;
    for (int i = 0; i < 15; i++)
        for (int j =0; j < 15; j++)
            chessBoard[i][j] = 0;
}

void Game::setStatus(int status[15][15])
{
    //status = (int **)malloc(sizeof(int *)*15);
    //for (int i = 0; i < 15; i++)
    //    status[i] = (int *)malloc(sizeof(int)*15);
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            chessBoard[i][j] = status[i][j];
}

void Game::getStatus(int status[15][15])
{
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            status[i][j] = chessBoard[i][j];
}

void Game::setTotalTime(const double totalTime)
{
    this->mTime = totalTime;
}

void Game::timerStart()
{
    //time(&start);
    start = duration_cast< milliseconds >(
        system_clock::now().time_since_epoch()
    );
}

void Game::timerStop()
{
    //time(&stop);
    stop = duration_cast< milliseconds >(
        system_clock::now().time_since_epoch()
    );
    timerDiff();
}

void Game::timerDiff()
{
    //mTime -= (double)(stop - start)/CLOCKS_PER_SEC;
    //mTime -= difftime(stop, start);
    mTime -= duration_cast<milliseconds>(stop - start).count();
}

double Game::getTimer()
{
    return this->mTime;
}

