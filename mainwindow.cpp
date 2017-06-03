#include "mainwindow.h"
#include "ui_mainwindow.h"


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
    double factor = 0.3;
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
    //frame * 4
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
    //dot * 5
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

        //2 players
        //this->chessBoard[int((x-X)*1.0/CHECK_WIDTH+0.5)][int((y-Y)*1.0/CHECK_WIDTH+0.5)] = (this->chessCounts%2 == 1)?1:-1;

        this->chessBoard[int((x-X)*1.0/CHECK_WIDTH+0.5)][int((y-Y)*1.0/CHECK_WIDTH+0.5)] = 1;
        this->update();
        computer();
        this->update();      //update the window
    }
    if(is_win() ==1){
        QMessageBox::information(NULL, "Game Over", "Black Win!", QMessageBox::Yes, QMessageBox::Yes);
        this->close();
    }
    else if(is_win()==-1){
        QMessageBox::information(NULL, "Game Over", "White Win!", QMessageBox::Yes, QMessageBox::Yes);
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
                up_down = true;
                continue;
            }
            else if(line.contains(white_win)){
                stat = -1;
                up_down = true;
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
                leftUp_rightDown = true;
                continue;
            }
            else if(line.contains(white_win)){
                stat = -1;
                leftUp_rightDown = true;
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
                leftUp_rightDown = true;
                continue;
            }
            else if(line.contains(white_win)){
                stat = -1;
                leftUp_rightDown = true;
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

/**************此处至最后为后端部分**************/

struct MatchState
{
    int state[15][15];
    MatchState()
    {
        for (int i = 0; i < 15 * 15; ++i)
        {
            state[i / 15][i % 15] = 0;
        }
    }
};

struct Action
{
    int row, col;
    Action()
    {
        row = -1;
        col = -1;
    }
    Action(int r, int c)
    {
        row = r;
        col = c;
    }
};

void getAvailableAction(const MatchState& currentState, QVector<Action>& actions)
{
    int i = 0, j = 0;
    for (i = 0; i < 15; i++)
    for (j = 0; j < 15; j++)
    {
        //这里假设电脑不会舍弃一块棋局而去另外的很远的地方下一子
        if (i >= 1 && j >= 1 && i <= 13 && j <= 13)
        {
            if (currentState.state[i][j] == 0)
            {
                if (currentState.state[i + 1][j] != 0 || currentState.state[i - 1][j] != 0 ||
                    currentState.state[i][j + 1] != 0 || currentState.state[i][j - 1] != 0 ||
                    currentState.state[i + 1][j + 1] != 0 || currentState.state[i - 1][j + 1] != 0 ||
                    currentState.state[i + 1][j - 1] != 0 || currentState.state[i - 1][j - 1] != 0)
                {
                    Action* temp = new Action;
                    temp->row = i;
                    temp->col = j;
                    actions.push_back(*temp);
                }
            }
        }
        //之后也要考虑一下在边界的地方落子的可能性
        if (i == 0 && j != 0 && j != 14)
        {
            if (currentState.state[i][j] == 0)
            {
                if (currentState.state[i + 1][j] != 0 ||currentState.state[i][j + 1] != 0 ||
                    currentState.state[i][j - 1] != 0 ||
                    currentState.state[i + 1][j + 1] != 0 ||currentState.state[i + 1][j - 1] != 0)
                {
                    Action* temp = new Action;
                    temp->row = i;
                    temp->col = j;
                    actions.push_back(*temp);
                }
            }
        }
        else if (i == 0 && j == 0)
        {
            if (currentState.state[i][j] == 0)
            {
                if (currentState.state[i + 1][j] != 0 || currentState.state[i][j + 1] != 0 ||
                    currentState.state[i + 1][j + 1] != 0)
                {
                    Action* temp = new Action;
                    temp->row = i;
                    temp->col = j;
                    actions.push_back(*temp);
                }
            }
        }
        else if (i == 0 && j == 14)
        {
            if (currentState.state[i][j] == 0)
            {
                if (currentState.state[i + 1][j] != 0 || currentState.state[i][j - 1] != 0 ||
                    currentState.state[i + 1][j - 1] != 0)
                {
                    Action* temp = new Action;
                    temp->row = i;
                    temp->col = j;
                    actions.push_back(*temp);
                }
            }
        }

        if (i == 14 && j != 0 && j != 14)
        {
            if (currentState.state[i][j] == 0)
            {
                if (currentState.state[i - 1][j] != 0 || currentState.state[i][j + 1] != 0 ||
                    currentState.state[i][j - 1] != 0 ||
                    currentState.state[i - 1][j + 1] != 0 || currentState.state[i - 1][j - 1] != 0)
                {
                    Action* temp = new Action;
                    temp->row = i;
                    temp->col = j;
                    actions.push_back(*temp);
                }
            }
        }
        else if (i == 14 && j == 0)
        {
            if (currentState.state[i][j] == 0)
            {
                if (currentState.state[i - 1][j] != 0 || currentState.state[i][j + 1] != 0 ||
                    currentState.state[i - 1][j + 1] != 0)
                {
                    Action* temp = new Action;
                    temp->row = i;
                    temp->col = j;
                    actions.push_back(*temp);
                }
            }
        }
        else if (i == 14 && j == 14)
        {
            if (currentState.state[i][j] == 0)
            {
                if (currentState.state[i - 1][j] != 0 || currentState.state[i][j - 1] != 0 ||
                    currentState.state[i - 1][j - 1] != 0)
                {
                    Action* temp = new Action;
                    temp->row = i;
                    temp->col = j;
                    actions.push_back(*temp);
                }
            }
        }

        if (j == 0  && i != 0 && i != 14)
        {
            if (currentState.state[i][j] == 0)
            {
                if (currentState.state[i - 1][j] != 0 || currentState.state[i + 1][j] != 0 ||
                    currentState.state[i][j + 1] != 0 ||
                    currentState.state[i - 1][j + 1] != 0 || currentState.state[i + 1][j + 1] != 0)
                {
                    Action* temp = new Action;
                    temp->row = i;
                    temp->col = j;
                    actions.push_back(*temp);
                }
            }
        }
        if (j == 14 && i != 0 && i != 14)
        {
            if (currentState.state[i][j] == 0)
            {
                if (currentState.state[i - 1][j] != 0 || currentState.state[i + 1][j] != 0 ||
                    currentState.state[i][j - 1] != 0 ||
                    currentState.state[i - 1][j - 1] != 0 || currentState.state[i + 1][j - 1] != 0)
                {
                    Action* temp = new Action;
                    temp->row = i;
                    temp->col = j;
                    actions.push_back(*temp);
                }
            }
        }

    }
}

MatchState* actionResult(const MatchState& currentState, Action action, const int player)
{
    MatchState* result = new MatchState;
    int i = 0;
    int j = 0;
    for (i = 0; i < 15; i++)
    for (j = 0; j < 15; j++)
    {
        result->state[i][j] = currentState.state[i][j];
    }
    int row = action.row;
    int col = action.col;
    result->state[row][col] = player;
    return result;
}

int Max_Value(MatchState& current_state, int depth, int alpha, int beta);
int Min_Value(MatchState& current_state, int depth, int alpha, int beta);

int Terminal_test(MatchState& current_state)
{
    //在这里我们假设返回1的话是计算机赢，返回-1是玩家赢，
    //返回2是平局，返回0是正在游戏
    int i = 0;
    int j = 0;
    int num = 0;
    for (i = 0; i < 15; i++)
    for (j = 0; j < 15; j++)
    {
        if (current_state.state[i][j] == 0)
            num++;
        //水平方向检测
        int cnt = 1;
        for (int k = j - 1; k >= 0; k--)
        {
            if (current_state.state[i][k] != current_state.state[i][j])
                break;
            else
                cnt++;
        }
        for (int k = j + 1; k < 15; k++)
        {
            if (current_state.state[i][k] != current_state.state[i][j])
                break;
            else
                cnt++;
        }
        if (cnt >= 5)
            return current_state.state[i][j];

        //竖直方向检测
        cnt = 1;
        for (int k = i - 1; k >= 0; k--)
        {
            if (current_state.state[k][j] != current_state.state[i][j])
                break;
            else
                cnt++;
        }
        for (int k = i + 1; k <= 15; k++)
        {
            if (current_state.state[k][j] != current_state.state[i][j])
                break;
            else
                cnt++;
        }
        if (cnt >= 5)
            return current_state.state[i][j];

        //对角线方向检测
        cnt = 1;
        int m = i + 1;
        for (int k = j - 1; k >= 0; k--)
        {
            if (m >= 15)
                break;
            if (current_state.state[m][k] != current_state.state[i][j])
                break;
            else
                cnt++;
            m++;
        }
        m = i - 1;
        for (int k = j + 1; k < 15; k++)
        {
            if (m < 0)
                break;
            if (current_state.state[m][k] != current_state.state[i][j])
                break;
            else
                cnt++;
            m--;
        }
        if (cnt >= 5)
            return current_state.state[i][j];

        cnt = 1;
        m = i - 1;
        for (int k = j - 1; k >= 0; k--)
        {
            if (m < 0)
                break;
            if (current_state.state[m][k] != current_state.state[i][j])
                break;
            else
                cnt++;
            m--;
        }
        m = i + 1;
        for (int k = j + 1; k < 15; k++)
        {
            if (m >= 15)
                break;
            if (current_state.state[m][k] != current_state.state[i][j])
                break;
            else
                cnt++;
            m++;
        }
        if (cnt >= 5)
            return current_state.state[i][j];

    }
    if (num == 0)
        return 2;
    else
        return 0;
}

int Utility(MatchState& current_state)
{
    int i = 0;
    int j = 0;
    int utility_num = 0;
    for (i = 0; i < 15; i++)
    {
        for (j = 0; j < 15; j++)
        {
            //水平方向检测
            int cnt = 1;
            int zero1 = 0;
            int zero2 = 0;
            if (current_state.state[i][j] == 0)
                continue;
            for (int k = j - 1; k >= 0; k--)
            {
                if (current_state.state[i][k] != current_state.state[i][j])
                {
                    if (current_state.state[i][k] == 0)
                        zero1 = 1;
                    break;
                }
                else
                    cnt++;
            }
            for (int k = j + 1; k < 15; k++)
            {
                if (current_state.state[i][k] != current_state.state[i][j])
                {
                    if (current_state.state[i][k] == 0)
                        zero2 = 1;
                    break;
                }
                else
                    cnt++;
            }
            if (cnt == 2 && (zero1 && zero2))
                utility_num += 2 * current_state.state[i][j];
            if (cnt == 4 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    utility_num += 10;
                else
                    utility_num += -10;
            }
            else if (cnt == 3 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    utility_num += 10;
                else
                    utility_num += -10;
            }
            //如果立刻可以赢了是最好的策略
            else if (cnt >= 5)
                return  100 * current_state.state[i][j];

            //竖直方向检测
            cnt = 1;
            zero1 = 0;
            zero2 = 0;
            for (int k = i - 1; k >= 0; k--)
            {
                if (current_state.state[k][j] != current_state.state[i][j])
                {
                    if (current_state.state[k][j] == 0)
                        zero1 = 1;
                    break;
                }
                else
                    cnt++;
            }
            for (int k = i + 1; k <= 15; k++)
            {
                if (current_state.state[k][j] != current_state.state[i][j])
                {
                    if (current_state.state[k][j] == 0)
                        zero2 = 1;
                    break;
                }
                else
                    cnt++;
            }
            if (cnt == 2 && (zero1 && zero2))
                utility_num += 2 * current_state.state[i][j];
            if (cnt == 4 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    utility_num += 10;
                else
                    utility_num += -10;
            }
            else if (cnt == 3 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    utility_num += 10;
                else
                    utility_num += -10;
            }
            else if (cnt >= 5)
                return 100 * current_state.state[i][j];

            //对角线方向检测
            cnt = 1;
            zero1 = 0;
            zero2 = 0;
            int m = i + 1;
            for (int k = j - 1; k >= 0; k--)
            {
                if (m >= 15)
                    break;
                if (current_state.state[m][k] != current_state.state[i][j])
                {
                    if (current_state.state[m][k] == 0)
                        zero1 = 1;
                    break;
                }
                else
                    cnt++;
                m++;
            }
            m = i - 1;
            for (int k = j + 1; k < 15; k++)
            {
                if (m < 0)
                    break;
                if (current_state.state[m][k] != current_state.state[i][j])
                {
                    if (current_state.state[m][k] == 0)
                        zero2 = 1;
                    break;
                }
                else
                    cnt++;
                m--;
            }
            if (cnt == 2 && (zero1 && zero2))
                utility_num += 2 * current_state.state[i][j];
            if (cnt == 4 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    utility_num += 10;
                else
                    utility_num += -10;
            }
            else if (cnt == 3 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    utility_num += 10;
                else
                    utility_num += -10;
            }
            else if (cnt >= 5)
                return  100 * current_state.state[i][j];

            cnt = 1;
            zero1 = 0;
            zero2 = 0;
            m = i - 1;
            for (int k = j - 1; k >= 0; k--)
            {
                if (m < 0)
                    break;
                if (current_state.state[m][k] != current_state.state[i][j])
                {
                    if (current_state.state[m][k] == 0)
                        zero1 = 1;
                    break;
                }
                else
                    cnt++;
                m--;
            }
            m = i + 1;
            for (int k = j + 1; k < 15; k++)
            {
                if (m >= 15)
                    break;
                if (current_state.state[m][k] != current_state.state[i][j])
                {
                    if (current_state.state[m][k] == 0)
                        zero2 = 1;
                    break;
                }
                else
                    cnt++;
                m++;
            }
            if (cnt == 2 && (zero1 && zero2))
                utility_num += 2 * current_state.state[i][j];
            if (cnt == 4 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    utility_num += 10;
                else
                    utility_num += -10;
            }
            else if (cnt == 3 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    utility_num += 10;
                else
                    utility_num += -10;
            }
            else if (cnt >= 5)
                return  100 * current_state.state[i][j];
        }
    }
    if (utility_num != 0)
        return utility_num;
    return -1;
}


int Max_Value(MatchState& current_state, int depth, int alpha, int beta)
{
    if (depth == 3)
        return Utility(current_state);
    depth++;
    if (Terminal_test(current_state))
        return Utility(current_state);

    int v = -1000;
    QVector<Action> actions;
    getAvailableAction(current_state, actions);
    int number = actions.size();
    for (int i = 0; i < number; i++)
    {
        MatchState * a = actionResult(current_state, actions[i], 1);
        int min_value = Min_Value(*a, depth, alpha, beta);
        if (min_value > v)
            v = min_value;
        if (v >= beta)
            return v;
        if (alpha < v)
            alpha = v;
    }
    return v;
}


int Min_Value(MatchState& current_state, int depth, int alpha, int beta)
{
    if (depth == 3)
        return Utility(current_state);
    depth++;
    if (Terminal_test(current_state))
        return Utility(current_state);
    int v = 1000;

    QVector<Action> actions;
    getAvailableAction(current_state, actions);
    int number = actions.size();
    for (int i = 0; i < number; i++)
    {
        MatchState * a = actionResult(current_state, actions[i], -1);
        int max_value = Max_Value(*a, depth, alpha, beta);
        if (max_value < v)
            v = max_value;
        if (v <= alpha)
            return v;
        if (beta > v)
            beta = v;
    }
    return v;
}

int guarantee(const MatchState& current_state, int& r, int& c)
{
    int i = 0;
    int j = 0;
    int priority = 0;
    for (i = 0; i < 15; i++)
    {
        for (j = 0; j < 15; j++)
        {
            //水平方向检测
            int cnt1 = 1;
            int cnt2 = 0;
            int cnt3 = 0;
            int row1 = 0;
            int col1 = 0;
            int row2 = 0;
            int col2 = 0;
            int row3 = 0;
            int col3 = 0;
            int row4 = 0;
            int col4 = 0;
            int zero1 = 0;
            int zero2 = 0;
            int zero3 = 0;
            int zero4 = 0;
            int zero5 = 0;
            int zero6 = 0;
            //if (current_state.state[i][j] == 0)
            //	continue;
            for (int k = j - 1; k >= 0; k--)
            {
                if (current_state.state[i][k] != current_state.state[i][j] && current_state.state[i][j] != 0)
                {
                    if (current_state.state[i][k] == 0)
                    {
                        row1 = i;
                        col1 = k;
                        zero1 = 1;
                    }
                    break;
                }
                else
                    cnt1++;
            }

            for (int k = j - 1; k >= 0; k--)
            {
                if (current_state.state[i][k] == 1 && current_state.state[i][j] == 0)
                {
                    cnt2++;
                }
                else
                {
                    if (current_state.state[i][k] == 0)
                    {
                        row3 = i;
                        col3 = j;
                        zero3 = 1;
                    }
                    break;
                }
            }
            for (int k = j - 1; k >= 0; k--)
            {
                if (current_state.state[i][k] == -1 && current_state.state[i][j] == 0)
                {
                    cnt3++;
                }
                else
                {
                    if (current_state.state[i][k] == 0)
                    {
                        row4 = i;
                        col4 = j;
                        zero5 = 1;
                    }
                    break;
                }
            }

            for (int k = j + 1; k < 15; k++)
            {
                if (current_state.state[i][k] == 1 && current_state.state[i][j] == 0)
                {
                    cnt2++;
                }
                else
                {
                    if (current_state.state[i][k] == 0)
                    {
                        row3 = i;
                        col3 = j;
                        zero4 = 1;
                    }
                    break;
                }
            }
            for (int k = j + 1; k < 15; k++)
            {
                if (current_state.state[i][k] == -1 && current_state.state[i][j] == 0)
                {
                    cnt3++;
                }
                else
                {
                    if (current_state.state[i][k] == 0)
                    {
                        row4 = i;
                        col4 = j;
                        zero6 = 1;
                    }
                    break;
                }
            }
            for (int k = j + 1; k < 15; k++)
            {
                if (current_state.state[i][k] != current_state.state[i][j] && current_state.state[i][j] != 0)
                {
                    if (current_state.state[i][k] == 0)
                    {
                        row2 = i;
                        col2 = k;
                        zero2 = 1;
                    }
                    break;
                }
                else
                    cnt1++;
            }
            //如果发现有冲四的现象，那么就必须拦截
            if (cnt1 == 4 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    priority = 10;
                else
                    priority = 5;
                r = row1;
                c = col1;
            }
            //虽然没有冲四，但是也不能够置之不理的情境
            else if (cnt1 == 4 && (zero1 || zero2))
            {
                if (zero1 == 1)
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row1;
                    c = col1;
                }
                else
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row2;
                    c = col2;
                }
            }
            //出现双三的情况
            else if (cnt1 == 3 && (zero1 && zero2))
            {
                r = row1;
                c = col1;
                //如果是自己的双三情况出现，应该首先攻占，优先度高一点
                if (current_state.state[i][j] == 1)
                    priority = 4;
                else
                    priority = 3;
            }
            else if (cnt2 == 4)
            {
                r = row3;
                c = col3;
                priority = 10;
            }
            else if (cnt2 == 3 && (zero3 && zero4))
            {
                r = row3;
                c = col3;
                priority = 4;
            }
            else if (cnt3 == 4)
            {
                r = row4;
                c = col4;
                priority = 5;
            }
            else if (cnt3 == 3 && (zero5 && zero6))
            {
                r = row4;
                c = col4;
                priority = 4;
            }


            //竖直方向检测
            cnt1 = 1;
            cnt2 = 0;
            cnt3 = 0;
            row1 = 0;
            col1 = 0;
            row2 = 0;
            col2 = 0;
            row3 = 0;
            col3 = 0;
            row4 = 0;
            col4 = 0;
            zero1 = 0;
            zero2 = 0;
            zero3 = 0;
            zero4 = 0;
            zero5 = 0;
            zero6 = 0;
            for (int k = i - 1; k >= 0; k--)
            {
                if (current_state.state[k][j] != current_state.state[i][j] && current_state.state[i][j] != 0)
                {
                    if (current_state.state[k][j] == 0)
                    {
                        row1 = k;
                        col1 = j;
                        zero1 = 1;
                    }
                    break;
                }
                else
                    cnt1++;
            }
            for (int k = i - 1; k >= 0; k--)
            {
                if (current_state.state[k][j] == 1 && current_state.state[i][j] == 0)
                {
                    cnt2++;
                }
                else
                {
                    if (current_state.state[k][j] == 0)
                    {
                        row3 = i;
                        col3 = j;
                        zero3 = 1;
                    }
                    break;
                }
            }
            for (int k = i - 1; k >= 0; k--)
            {
                if (current_state.state[k][j] == -1 && current_state.state[i][j] == 0)
                {
                    cnt3++;
                }
                else
                {
                    if (current_state.state[k][j] == 0)
                    {
                        row4 = i;
                        col4 = j;
                        zero5 = 1;
                    }
                    break;
                }
            }

            for (int k = i + 1; k < 15; k++)
            {
                if (current_state.state[k][j] == 1 && current_state.state[i][j] == 0)
                {
                    cnt2++;
                }
                else
                {
                    if (current_state.state[k][j] == 0)
                    {
                        row3 = i;
                        col3 = j;
                        zero4 = 1;
                    }
                    break;
                }
            }
            for (int k = i + 1; k < 15; k++)
            {
                if (current_state.state[k][j] == -1 && current_state.state[i][j] == 0)
                {
                    cnt3++;
                }
                else
                {
                    if (current_state.state[k][j] == 0)
                    {
                        row4 = i;
                        col4 = j;
                        zero6 = 1;
                    }
                    break;
                }
            }
            for (int k = i + 1; k <= 15; k++)
            {
                if (current_state.state[k][j] != current_state.state[i][j] && current_state.state[i][j] != 0)
                {
                    if (current_state.state[k][j] == 0)
                    {
                        row2 = k;
                        col2 = j;
                        zero2 = 1;
                    }
                    break;
                }
                else
                    cnt1++;
            }
            //如果发现有冲四的现象，那么就必须拦截
            if (cnt1 == 4 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    priority = 10;
                else
                    priority = 5;
                r = row1;
                c = col1;
            }
            //虽然没有冲四，但是也不能够置之不理的情境
            else if (cnt1 == 4 && (zero1 || zero2))
            {
                if (zero1 == 1)
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row1;
                    c = col1;
                }
                else
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row2;
                    c = col2;
                }
            }
            //出现双三的情况
            else if (cnt1 == 3 && (zero1 && zero2))
            {
                r = row1;
                c = col1;
                //如果是自己的双三情况出现，应该首先攻占，优先度高一点
                if (current_state.state[i][j] == 1)
                    priority = 4;
                else
                    priority = 3;
            }
            else if (cnt2 == 4)
            {
                r = row3;
                c = col3;
                priority = 10;
            }
            else if (cnt2 == 3 && (zero3 && zero4))
            {
                r = row3;
                c = col3;
                priority = 4;
            }
            else if (cnt3 == 4)
            {
                r = row4;
                c = col4;
                priority = 5;
            }
            else if (cnt3 == 3 && (zero5 && zero6))
            {
                r = row4;
                c = col4;
                priority = 4;
            }

            //对角线方向检测
            cnt1 = 1;
            cnt2 = 0;
            cnt3 = 0;
            row1 = 0;
            col1 = 0;
            row2 = 0;
            col2 = 0;
            row3 = 0;
            col3 = 0;
            row4 = 0;
            col4 = 0;
            zero1 = 0;
            zero2 = 0;
            zero3 = 0;
            zero4 = 0;
            zero5 = 0;
            zero6 = 0;
            int m = i + 1;
            for (int k = j - 1; k >= 0; k--)
            {
                if (m >= 15)
                    break;
                if (current_state.state[m][k] != current_state.state[i][j] && current_state.state[i][j] != 0)
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row1 = m;
                        col1 = k;
                        zero1 = 1;
                    }
                    break;
                }
                else
                    cnt1++;
                m++;
            }
            m = i + 1;
            for (int k = j - 1; k >= 0; k--)
            {
                if (m >= 15)
                    break;
                if (current_state.state[m][k] == 1 && current_state.state[i][j] == 0)
                {
                    cnt2++;
                }
                else
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row3 = i;
                        col3 = j;
                        zero3 = 1;
                    }
                    break;
                }
                m++;
            }
            m = i + 1;
            for (int k = j - 1; k >= 0; k--)
            {
                if (m >= 15)
                    break;
                if (current_state.state[m][k] == -1 && current_state.state[i][j] == 0)
                {
                    cnt3++;
                }
                else
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row4 = i;
                        col4 = j;
                        zero5 = 1;
                    }
                    break;
                }
                m++;
            }

            m = i - 1;
            for (int k = j + 1; k < 15; k++)
            {
                if (m < 0)
                    break;
                if (current_state.state[m][k] == 1 && current_state.state[i][j] == 0)
                {
                    cnt2++;
                }
                else
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row3 = i;
                        col3 = j;
                        zero4 = 1;
                    }
                    break;
                }
                m--;
            }
            m = i - 1;
            for (int k = j + 1; k < 15; k++)
            {
                if (m < 0)
                    break;
                if (current_state.state[m][k] == -1 && current_state.state[i][j] == 0)
                {
                    cnt3++;
                }
                else
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row4 = i;
                        col4 = j;
                        zero6 = 1;
                    }
                    break;
                }
                m--;
            }

            m = i - 1;
            for (int k = j + 1; k < 15; k++)
            {
                if (m < 0)
                    break;
                if (current_state.state[m][k] != current_state.state[i][j] && current_state.state[i][j] != 0)
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row2 = m;
                        col2 = k;
                        zero2 = 1;
                    }
                    break;
                }
                else
                    cnt1++;
                m--;
            }

            //如果发现有冲四的现象，那么就必须拦截
            if (cnt1 == 4 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    priority = 10;
                else
                    priority = 5;
                r = row1;
                c = col1;
            }
            //虽然没有冲四，但是也不能够置之不理的情境
            else if (cnt1 == 4 && (zero1 || zero2))
            {
                if (zero1 == 1)
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row1;
                    c = col1;
                }
                else
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row2;
                    c = col2;
                }
            }
            //出现双三的情况
            else if (cnt1 == 3 && (zero1 && zero2))
            {
                r = row1;
                c = col1;
                //如果是自己的双三情况出现，应该首先攻占，优先度高一点
                if (current_state.state[i][j] == 1)
                    priority = 4;
                else
                    priority = 3;
            }
            else if (cnt2 == 4)
            {
                r = row3;
                c = col3;
                priority = 10;
            }
            else if (cnt2 == 3 && (zero3 && zero4))
            {
                r = row3;
                c = col3;
                priority = 4;
            }
            else if (cnt3 == 4)
            {
                r = row4;
                c = col4;
                priority = 5;
            }
            else if (cnt3 == 3 && (zero5 && zero6))
            {
                r = row4;
                c = col4;
                priority = 4;
            }

            cnt1 = 1;
            cnt2 = 0;
            cnt3 = 0;
            row1 = 0;
            col1 = 0;
            row2 = 0;
            col2 = 0;
            row3 = 0;
            col3 = 0;
            row4 = 0;
            col4 = 0;
            zero1 = 0;
            zero2 = 0;
            zero3 = 0;
            zero4 = 0;
            zero5 = 0;
            zero6 = 0;
            m = i - 1;
            for (int k = j - 1; k >= 0; k--)
            {
                if (m < 0)
                    break;
                if (current_state.state[m][k] != current_state.state[i][j])
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row1 = m;
                        col1 = k;
                        zero1 = 1;
                    }
                    break;
                }
                else
                    cnt1++;
                m--;
            }

            m = i - 1;
            for (int k = j - 1; k >= 0; k--)
            {
                if (m < 0)
                    break;
                if (current_state.state[m][k] == 1 && current_state.state[i][j] == 0)
                {
                    cnt2++;
                }
                else
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row3 = i;
                        col3 = j;
                        zero3 = 1;
                    }
                    break;
                }
                m--;
            }
            m = i - 1;
            for (int k = j - 1; k >= 0; k--)
            {
                if (m < 0)
                    break;
                if (current_state.state[m][k] == -1 && current_state.state[i][j] == 0)
                {
                    cnt3++;
                }
                else
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row4 = i;
                        col4 = j;
                        zero5 = 1;
                    }
                    break;
                }
                m--;
            }

            m = i + 1;
            for (int k = j + 1; k < 15; k++)
            {
                if (m >= 15)
                    break;
                if (current_state.state[m][k] == 1 && current_state.state[i][j] == 0)
                {
                    cnt2++;
                }
                else
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row3 = i;
                        col3 = j;
                        zero4 = 1;
                    }
                    break;
                }
                m++;
            }
            m = i + 1;
            for (int k = j + 1; k < 15; k++)
            {
                if (m >= 15)
                    break;
                if (current_state.state[m][k] == -1 && current_state.state[i][j] == 0)
                {
                    cnt3++;
                }
                else
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row4 = i;
                        col4 = j;
                        zero6 = 1;
                    }
                    break;
                }
                m++;
            }

            m = i + 1;
            for (int k = j + 1; k < 15; k++)
            {
                if (m >= 15)
                    break;
                if (current_state.state[m][k] != current_state.state[i][j])
                {
                    if (current_state.state[m][k] == 0)
                    {
                        row2 = m;
                        col2 = k;
                        zero2 = 1;
                    }
                    break;
                }
                else
                    cnt1++;
                m++;
            }

            //如果发现有冲四的现象，那么就必须拦截
            if (cnt1 == 4 && (zero1 && zero2))
            {
                if (current_state.state[i][j] == 1)
                    priority = 10;
                else
                    priority = 5;
                r = row1;
                c = col1;
            }
            //虽然没有冲四，但是也不能够置之不理的情境
            else if (cnt1 == 4 && (zero1 || zero2))
            {
                if (zero1 == 1)
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row1;
                    c = col1;
                }
                else
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row2;
                    c = col2;
                }
            }
            //出现双三的情况
            else if (cnt1 == 3 && (zero1 && zero2))
            {
                r = row1;
                c = col1;
                //如果是自己的双三情况出现，应该首先攻占，优先度高一点
                if (current_state.state[i][j] == 1)
                    priority = 4;
                else
                    priority = 3;
            }
            else if (cnt2 == 4)
            {
                r = row3;
                c = col3;
                priority = 10;
            }
            else if (cnt2 == 3 && (zero3 && zero4))
            {
                r = row3;
                c = col3;
                priority = 4;
            }
            else if (cnt3 == 4)
            {
                r = row4;
                c = col4;
                priority = 5;
            }
            else if (cnt3 == 3 && (zero5 && zero6))
            {
                r = row4;
                c = col4;
                priority = 4;
            }
        }
    }
    if (priority != 0)
        return priority;
    return 0;
}

void miniMaxSearchForFive(const MatchState& currentState, int& r, int& c)
{
    QVector<Action> actions;
    QVector<double> values;
    int t = 0;
    //首先应该先检验一下是否出现了危险的境地，也就是失误之后必输的情景
    t = guarantee(currentState, r, c);
    if (t != 0)
        return;
    getAvailableAction(currentState, actions);
    int depth = 0;
    //在这里需要我们限制极大极小的深度搜索
    //为了能够减少搜索的时间复杂度，在这里采用alpha beta剪枝策略
    for (int i = 0; i < (int)actions.size(); ++i)
    {
        values.push_back(Min_Value(*actionResult(currentState, actions[i], 1), depth, -10000, 10000));
    }
    int max_idx = 0;
    double max_val = values[max_idx];
    for (int i = 1; i < (int)values.size(); ++i)
    {
        if (values[i] > max_val)
        {
            max_idx = i;
            max_val = values[i];
        }
    }
    r = actions[max_idx].row;
    c = actions[max_idx].col;
}

void MainWindow::computer(){
    int r,c;
    MatchState currentState;
    for (int i = 0; i < 15 * 15; ++i)
    {
        currentState.state[i / 15][i % 15] = chessBoard[i/15][i%15];
    }
    miniMaxSearchForFive(currentState,r,c);
    this->chessBoard[r][c] = -1;
}



