#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QMouseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QVector>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTimer>
#include <QTime>
#include <QFileDialog>
#include <qdatetime.h>
#include "game.h"
#include<iostream>
#include <cstdlib>
namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow{
    Q_OBJECT

public:

    const int CHECK_NUM = 14;
    const int CHECK_WIDTH = 40;
    const int X = 80, Y = 80;//offset
    const int SIZE = CHECK_NUM * CHECK_WIDTH + 2*X;
    // size = 660
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *mouseEvent);//player
    //friend void Player::fall();
    void computer();
    //friend void Computer::fall();
    int is_win();//Implementing method: searching the substring

// define slot
protected slots:
    void newGame(void);
    void quitGame(void);
    void openGame(void);
    void saveGame(void);
    void setEasyMode(void);
    void setMiddleMode(void);
    void setDifficultMode(void);
    void showAbout(void);
    void timeUpdate(void);
    void updateTimer1(const QString);
    void updateTimer2(const QString);
    void on_radioType_clicked();
    void on_radioType_2_clicked();
    void setDisplay();


private:
    //performance test
    /*
    long max_time=0;
    long sum_time=0;
    long average_time;
    int max_memory=0;
    int sum_memory = 0;
    int average_memory;
    QVector<long> time_collection;
    QVector<int> memory_collection;
    int mb = 1024*1024;
    */

    int xPressed;
    int yPressed;
    int chessCounts;
    int counts[225][3];
    int chessboard[15][15];//Simulated chessboard; 1:Black,-1:White 0:None
    Game game;
    // menu
    void setupMenuBar(void);
    void setupToolBar(void);
    void setupStatus();
    int StatusSize; // the size of status
    QTimer* timer;

    QTime* timeValueComputer;
    QTime* timeValuePlayer;


    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
