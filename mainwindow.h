#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QMouseEvent>
#include <QDebug>
#include <QMessageBox>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    const int CHECK_NUM = 14;
    const int CHECK_WIDTH = 40;
    const int X = 50, Y = 50;//offset
    const int SIZE = CHECK_NUM * CHECK_WIDTH + 2 * X;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *mouseEvent);
    int is_win();//Implementing method: searching the substring


private:
    int xPressed;
    int yPressed;
    int chessCounts;
    int counts[225][3];
    int chessBoard[15][15];//Simulated chessboard; 1:Black,-1:White 0:None
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
