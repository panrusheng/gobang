#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    setFixedSize(SIZE,SIZE);
    this->xPressed = 0;
    this->yPressed = 0;
    this->chessCounts = 0;
    for(int i=0;i<15;i++)
    {
        for(int j=0;j<15;j++)
        {
            chessBoard[i][j] = 0;
        }
    }
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter paint(this);
    paint.setRenderHint(QPainter::Antialiasing,true);
    //background
    paint.setPen(QPen(QColor::fromRgbF(1, 211.0/255.0, 0,0.7),2,Qt::SolidLine));
    paint.setBrush(QBrush(QColor::fromRgbF(1, 211.0/255.0, 0,0.7),Qt::SolidPattern));
    paint.drawRect(0,0,SIZE,SIZE);
    paint.setPen(QPen(QColor::fromRgbF(0,0,0,1),1,Qt::SolidLine));
    float factor = 0.3;
    paint.drawLine(X*factor,Y*factor,-X*factor+SIZE,Y*factor);
    paint.drawLine(X*factor,-Y*factor+SIZE,-X*factor+SIZE,-Y*factor+SIZE);
    paint.drawLine(X*factor,Y*factor,X*factor,-Y*factor+SIZE);
    paint.drawLine(-X*factor+SIZE,Y*factor,-X*factor+SIZE,-Y*factor+SIZE);
    //horizonal line * (CHECK_NUM-1)
    paint.setPen(QPen(QColor::fromRgbF(0,0,0,0.7),2,Qt::SolidLine));
    for(int i=1;i<CHECK_NUM;i++){
      paint.drawLine(X,Y+CHECK_WIDTH*i,X+CHECK_WIDTH*(CHECK_NUM),Y+CHECK_WIDTH*i);//draw line: from (x1,y1) to (x2,y2)
    }
    //vertical line * (CHECK_NUM-1)
    for(int i=1;i<CHECK_NUM;i++){
      paint.drawLine(X+CHECK_WIDTH*i,Y,X+CHECK_WIDTH*i,Y+CHECK_WIDTH*(CHECK_NUM));
    }
    //frames * 4
    paint.setPen(QPen(QColor::fromRgbF(0,0,0,1),4,Qt::SolidLine));
    paint.drawLine(X,Y,X+CHECK_WIDTH*CHECK_NUM,Y);
    paint.drawLine(X,Y+CHECK_WIDTH*CHECK_NUM,X+CHECK_WIDTH*CHECK_NUM,Y+CHECK_WIDTH*CHECK_NUM);
    paint.drawLine(X,Y,X,Y+CHECK_WIDTH*CHECK_NUM);
    paint.drawLine(X+CHECK_WIDTH*CHECK_NUM,Y,X+CHECK_WIDTH*CHECK_NUM,Y+CHECK_WIDTH*CHECK_NUM);
    //text
    paint.setPen(QPen(QColor::fromRgbF(0,0,1,1),4,Qt::SolidLine));
    if(1){
        int num = 1;
        char str[3],char_a[2]="A";
        for(short i = 0;i < 15;i++){
            itoa(num++,str,10);
            paint.drawText(X*0.4,Y*1.1+i*CHECK_WIDTH,str);
            paint.drawText(X*0.95+i*CHECK_WIDTH,Y*0.6,char_a);
            char_a[0]++;
        }
    }
    //dots * 5
    int dot = CHECK_WIDTH/8;
    paint.setPen(QPen(QColor::fromRgbF(0,0,0,1),4,Qt::SolidLine));
    paint.setBrush(QBrush(Qt::black,Qt::SolidPattern));
    paint.drawEllipse(X-dot/2+3*CHECK_WIDTH,Y-dot/2+3*CHECK_WIDTH,dot,dot);//draw ellipse: start(X,Y),CHECK_WIDTH,height
    paint.drawEllipse(X-dot/2+3*CHECK_WIDTH,Y-dot/2+11*CHECK_WIDTH,dot,dot);
    paint.drawEllipse(X-dot/2+11*CHECK_WIDTH,Y-dot/2+3*CHECK_WIDTH,dot,dot);
    paint.drawEllipse(X-dot/2+11*CHECK_WIDTH,Y-dot/2+11*CHECK_WIDTH,dot,dot);
    paint.drawEllipse(X-dot/2+7*CHECK_WIDTH,Y-dot/2+7*CHECK_WIDTH,dot,dot);
    //chess pieces
    int radix = CHECK_WIDTH*0.9;
    QImage black_chess,white_chess;
    black_chess.load("../Gobang/black.png");
    white_chess.load("../Gobang/white.png");
    black_chess = black_chess.scaled(radix,radix,Qt::KeepAspectRatio);
    white_chess = white_chess.scaled(radix,radix,Qt::KeepAspectRatio);
    if(1){
        int pos_x,pos_y;
        for(short i = 0; i < 15; i++){
            for(short j = 0; j < 15; j++){
                pos_x = X-radix/2+CHECK_WIDTH*i;
                pos_y = Y-radix/2+CHECK_WIDTH*j;
                if(chessBoard[i][j]==1){
                    paint.drawImage(pos_x,pos_y,black_chess);
                }
                else if(chessBoard[i][j]==-1){
                    paint.drawImage(pos_x,pos_y,white_chess);
                }
            }
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *mouseEvent)
{
    int x = mouseEvent->x();
    int y = mouseEvent->y();

    if(x>0&&x<SIZE&&y>0&&y<SIZE&&this->chessBoard[int((x-X)*1.0/CHECK_WIDTH+0.5)][int((y-Y)*1.0/CHECK_WIDTH+0.5)]==0)
    {
        this->chessCounts += 1;
        this->chessBoard[int((x-X)*1.0/CHECK_WIDTH+0.5)][int((y-Y)*1.0/CHECK_WIDTH+0.5)] = (this->chessCounts%2 == 1)?1:-1;
        this->update();      //update the window
    }
    if(is_win() ==1){
        QMessageBox::information(NULL, "Game Over", "Black Win!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        this->close();
    }
    else if(is_win()==-1){
        QMessageBox::information(NULL, "Game Over", "White Win!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        this->close();
    }
}
int MainWindow::is_win(){
    int stat = 0;//0: not ended; 1: black win; -1: white win;
    const QString black_win("11111");
    const QString white_win("00000");
    QString line;//-- or | or \ or /
    const int len = 15;
    bool left_right = false;
    bool up_down = false;
    bool leftUp_rightDown = false;
    bool leftDown_rightUp = false;
    //  left-right
    for(int i = 0;i<len;i++){
        for(int j=0;j<len;j++){
            switch(chessBoard[j][i]){
            case 1:line[j]='1';break;
            case 0:line[j]='2';break;
            case -1:line[j]='0';break;
            }
        }
        if(line.contains(black_win)){
            stat = 1;
            left_right = true;
            continue;
        }
        else if(line.contains(white_win)){
            stat = -1;
            left_right = true;
            continue;
        }
    }
    //  up-down
    if(!left_right){
        for(int i = 0;i<len;i++){
            for(int j=0;j<len;j++){
                switch(chessBoard[i][j]){
                case 1:line[j]='1';break;
                case 0:line[j]='2';break;
                case -1:line[j]='0';break;
                }
            }
            if(line.contains(black_win)){
                stat = 1;
                left_right = true;
                continue;
            }
            else if(line.contains(white_win)){
                stat = -1;
                left_right = true;
                continue;
            }
        }
    }
    //  leftUp-rightDown
    if((!left_right)||(!up_down)){
        for(int i = 4;i<len;i++){
            for(int j = 0;j<i+1;j++){
                switch(chessBoard[len-(i-j)][j]){
                case 1:line[j]='1';break;
                case 0:line[j]='2';break;
                case -1:line[j]='0';break;
                }
            }
            if(line.contains(black_win)){
                stat = 1;
                leftDown_rightUp = true;
                continue;
            }
            else if(line.contains(white_win)){
                stat = -1;
                leftDown_rightUp = true;
                continue;
            }

            for(int j = 0;j<i+1;j++){
                switch(chessBoard[i-j][len-j]){
                case 1:line[j]='1';break;
                case 0:line[j]='2';break;
                case -1:line[j]='0';break;
                }
            }
            if(line.contains(black_win)){
                stat = 1;
                leftDown_rightUp = true;
                continue;
            }
            else if(line.contains(white_win)){
                stat = -1;
                leftDown_rightUp = true;
                continue;
            }
        }
    }

    //  leftDown-rightUp
    if((!left_right)||(!up_down)||(!leftUp_rightDown)){
        for(int i = 4;i<len;i++){
            for(int j = 0;j<i+1;j++){
                switch(chessBoard[i-j][j]){
                case 1:line[j]='1';break;
                case 0:line[j]='2';break;
                case -1:line[j]='0';break;
                }
            }
            if(line.contains(black_win)){
                stat = 1;
                leftDown_rightUp = true;
                continue;
            }
            else if(line.contains(white_win)){
                stat = -1;
                leftDown_rightUp = true;
                continue;
            }

            for(int j = 0;j<i+1;j++){
                switch(chessBoard[len-(i-j)][len-j]){
                case 1:line[j]='1';break;
                case 0:line[j]='2';break;
                case -1:line[j]='0';break;
                }
            }
            if(line.contains(black_win)){
                stat = 1;
                leftDown_rightUp = true;
                continue;
            }
            else if(line.contains(white_win)){
                stat = -1;
                leftDown_rightUp = true;
                continue;
            }
        }
    }
    return stat;
}

