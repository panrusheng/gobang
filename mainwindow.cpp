#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>
#include <iomanip>
using namespace std;

#define LIMIT_TIME_MIN 5
#define LIMIT_TIME_SEC 0

int max_depth = 3;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    // add menu
    //QTimer *timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer1(QString)));
    //connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer2(QString)));
    //timer->start(1000);

    StatusSize = 200;
    //QTextEdit *center = new QTextEdit(this);
    //center->setReadOnly(true);
    //center->setMinimumSize(SIZE, SIZE);
    //setCentralWidget(center);
    setWindowTitle("GoBang");
    setFixedSize(SIZE + StatusSize, SIZE-Y*0.5);
    this->xPressed = 0;
    this->yPressed = 0;
    this->chessCounts = 0;

    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            chessboard[i][j] = 0;
    game.getStatus(chessboard);
    setupMenuBar();

    ui->radioType->setChecked(true);
    ui->radioType_2->setChecked(false);

    timer = new QTimer();

    timeValueComputer = new QTime(0, LIMIT_TIME_MIN, LIMIT_TIME_SEC);
    timeValuePlayer = new QTime(0, LIMIT_TIME_MIN, LIMIT_TIME_SEC);

    ui->lcdComputer->display(this->timeValueComputer->toString());
    ui->lcdPlayer->display(this->timeValuePlayer->toString());

    this->timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(setDisplay()));
}

void MainWindow::setDisplay()
{
    //cout << chessCounts << endl;
    if (this->chessCounts%2 == 0 && !is_win())
        this->timeValuePlayer->setHMS(0, this->timeValuePlayer->addSecs(-1).minute(), this->timeValuePlayer->addSecs(-1).second());
    else if (this->chessCounts%2 == 1 && !is_win())
        this->timeValueComputer->setHMS(0, this->timeValueComputer->addSecs(-1).minute(), this->timeValueComputer->addSecs(-1).second());

    ui->lcdComputer->display(this->timeValueComputer->toString());
    ui->lcdPlayer->display(this->timeValuePlayer->toString());
}


MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::setupMenuBar(void)
{
    QMenuBar *MenuBar = new QMenuBar(this);

    QMenu *GameMenu = new QMenu(tr("Game"), MenuBar);
    QMenu *ModeMenu = new QMenu(tr("Mode"), MenuBar);
    QMenu *AboutMenu = new QMenu(tr("About"), MenuBar);

    QToolBar *GameToolBar = addToolBar(tr("New Game"));
    /// set game menu
    // add action
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/icon/new.png"));
    QAction *newGame = new QAction(newIcon, tr("New Game"), GameMenu);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/icon/open.png"));
    QAction *openGame = new QAction(openIcon, tr("Open Game"), GameMenu);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/icon/save.png"));
    QAction *saveGame = new QAction(saveIcon, tr("Save Game"), GameMenu);
    QAction *quitGame = new QAction(tr("Quit Game"), GameMenu);
    GameMenu->addAction(newGame);
    GameMenu->addAction(openGame);
    GameMenu->addAction(saveGame);
    GameMenu->addAction(quitGame);

    // add menu
    MenuBar->addMenu(GameMenu);

    // connect
    connect(newGame, SIGNAL(triggered(bool)), this, SLOT(newGame()));
    connect(openGame, SIGNAL(triggered(bool)), this, SLOT(openGame()));
    connect(saveGame, SIGNAL(triggered(bool)), this, SLOT(saveGame()));
    connect(quitGame, SIGNAL(triggered(bool)), this, SLOT(quitGame()));

    // set game toolbar
    GameToolBar->addAction(newGame);
    GameToolBar->addAction(openGame);
    GameToolBar->addAction(saveGame);

    /// set mode menu
    // add action
    QAction *easyMode = new QAction("Easy", ModeMenu);
    QAction *middleMode = new QAction("Middle", ModeMenu);
    QAction *difficultMode = new QAction("Difficult", ModeMenu);
    ModeMenu->addAction(easyMode);
    ModeMenu->addAction(middleMode);
    ModeMenu->addAction(difficultMode);

    // add menu
    MenuBar->addMenu(ModeMenu);

    // connect
    connect(easyMode, SIGNAL(triggered(bool)), this, SLOT(setEasyMode()));
    connect(middleMode, SIGNAL(triggered(bool)), this, SLOT(setMiddleMode()));
    connect(difficultMode, SIGNAL(triggered(bool)), this, SLOT(setDifficultMode()));

    /// set about menu
    // add action
    QAction *aboutInfo = new QAction("About", AboutMenu);
    AboutMenu->addAction(aboutInfo);
    // add menu
    MenuBar->addMenu(AboutMenu);
    // connect
    connect(aboutInfo, SIGNAL(triggered(bool)), this, SLOT(showAbout()));

    // set menubar
    setMenuBar(MenuBar);
}

void MainWindow::on_radioType_clicked()
{
    this->newGame();
}

void MainWindow::on_radioType_2_clicked()
{
    this->newGame();
}



void MainWindow::setupStatus()
{
    QDockWidget *dock = new QDockWidget(tr("Computer"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    QListWidget *infoComputer;
    infoComputer = new QListWidget(dock);
    infoComputer->addItems(QStringList()
                       << "Time: "
                       << str);
    ui->timeComputer->setText(str);
    dock->setWidget(infoComputer);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    // player
    dock = new QDockWidget(tr("Player"), this);
    QListWidget *infoPlayer;

    infoPlayer = new QListWidget(dock);
    infoPlayer->addItems(QStringList()
                         << "Time: "
                         << str);
    dock->setWidget(infoPlayer);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    ui->timePlayer->setText(str);
}

void MainWindow::timeUpdate()
{
    //ui->timeComputer->setText();
    //ui->timePlayer->setText();
}

void MainWindow::newGame(void)
{
    game.clear(); // clear the records
    game.getStatus(chessboard);

    this->timer = new QTimer();
    this->timeValueComputer = new QTime(0, LIMIT_TIME_MIN, LIMIT_TIME_SEC);
    this->timeValuePlayer = new QTime(0, LIMIT_TIME_MIN, LIMIT_TIME_SEC);
    this->chessCounts = 0;

    this->update();
}

void MainWindow::openGame(void)
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Open Game"),
                                                ".",
                                                tr("Text Files(*.txt)"));
    if (!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QMessageBox::warning(this,
                                 tr("Read Game File"),
                                 tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream in(&file);


        QString qstr;
        string str;
        int tmp_i;
        //qstr = in.readLine();
        qstr = in.readAll();
        str = qstr.toStdString();
        istringstream iss(str);
        cout << str << endl;
        for (int j = 0; j < 15; j++)
        {
            for (int i = 0; i < 15; i++)
            {
                iss >> tmp_i;
                cout << tmp_i << " ";
                chessboard[i][j] = tmp_i;
            }
            cout << endl;
        }
        this->update();
        //ss << in.readAll();
        cout << str << endl;
        file.close();

    }
    else
    {
        QMessageBox::warning(this,
                             tr("Path"),
                             tr("You have not select any file."));
    }

}

void MainWindow::saveGame(void)
{
    string filename;
    stringstream ss;
    struct tm * timeinfo;
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    ss << timeinfo->tm_year + 1900 ;

    ss << setw(2) << setfill('0') << timeinfo->tm_mon + 1;
    ss << setw(2) << setfill('0') << timeinfo->tm_mday;
    ss << setw(2) << setfill('0') << timeinfo->tm_hour;
    ss << setw(2) << setfill('0') << timeinfo->tm_min;
    ss << setw(2) << setfill('0') << timeinfo->tm_sec;
    filename = "gobang_" + ss.str() + ".txt";
    cout << filename << endl;
    cout <<  asctime(timeinfo) << endl;
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Open Game File"),
                                                ".",
                                                tr("Text Files(*.txt)"));
    if (!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,
                                 tr("Save Game"),
                                 tr("Cannot open file\n%1").arg(path));
            return;
        }
        QTextStream out(&file);
        for (int j = 0; j < 15; j++)
        {
            for (int i = 0; i < 15; i++)
                out << chessboard[i][j] << " ";
            out << endl;
        }
        file.close();
    }
    else
    {
        QMessageBox::warning(this,
                             tr("Path"),
                             tr("You did not select any file."));
    }
}

void MainWindow::quitGame(void)
{
    this->close();
}

void MainWindow::setEasyMode()
{
    this->newGame();
    max_depth = 1;
}

void MainWindow::setMiddleMode()
{
    this->newGame();
    max_depth = 2;
}

void MainWindow::setDifficultMode()
{
    this->newGame();
    max_depth = 3;
}

void MainWindow::showAbout()
{
    QMessageBox::information(NULL, "About", "CopyRight from 2017 OOP Project.", QMessageBox::Yes, QMessageBox::Yes);
}

void MainWindow::updateTimer1(const QString qstr)
{
    ui->timeComputer->clear();
    ui->timeComputer->setText(qstr);
    ui->lcdComputer->display(qstr);
}

void MainWindow::updateTimer2(const QString qstr)
{
    ui->timePlayer->clear();
    ui->timePlayer->setText(qstr);
    ui->lcdPlayer->display(qstr);
}

void MainWindow::paintEvent(QPaintEvent *){
    // get the current status
    //game.getStatus(chessboard);

    QPainter paint(this);
    paint.setRenderHint(QPainter::Antialiasing,true);
    //background
    double factor = 0.73;
    paint.setPen(QPen(QColor::fromRgbF(1, 211.0/255.0, 0,0.7),2,Qt::SolidLine));
    paint.setBrush(QBrush(QColor::fromRgbF(1, 211.0/255.0, 0,0.7),Qt::SolidPattern));
    paint.drawRect(0,Y*factor,SIZE,SIZE-2*Y*factor);
    paint.setPen(QPen(QColor::fromRgbF(0,0,0,1),1,Qt::SolidLine));
    paint.drawLine(0,Y*factor,SIZE,Y*factor);
    paint.drawLine(0,-Y*factor+SIZE,SIZE,-Y*factor+SIZE);
    paint.drawLine(0,Y*factor,0,-Y*factor+SIZE);
    paint.drawLine(SIZE,Y*factor,SIZE,-Y*factor+SIZE);
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
            //itoa(num++,str,10);
            sprintf(str, "%d", num++);
            paint.drawText(X*0.78,Y*1.1+i*CHECK_WIDTH,str);
            paint.drawText(X*0.95+i*CHECK_WIDTH,Y*0.9,char_a);
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
    //black_chess.load("../Gobang/black.png");
    //white_chess.load("../Gobang/white.png");
    black_chess.load(":/images/black");
    white_chess.load(":/images/white");
    black_chess = black_chess.scaled(radix,radix,Qt::KeepAspectRatio);
    white_chess = white_chess.scaled(radix,radix,Qt::KeepAspectRatio);
    if(1){
        int pos_x,pos_y;
        for(short i = 0; i < 15; i++){
            for(short j = 0; j < 15; j++){
                pos_x = X-radix/2+CHECK_WIDTH*i;
                pos_y = Y-radix/2+CHECK_WIDTH*j;
                if(chessboard[i][j]==1){
                    paint.drawImage(pos_x,pos_y,black_chess);
                }
                else if(chessboard[i][j]==-1){
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

    if(x>0&&x<SIZE&&y>0&&y<SIZE&&this->chessboard[int((x-X)*1.0/CHECK_WIDTH+0.5)][int((y-Y)*1.0/CHECK_WIDTH+0.5)]==0)
    {

        this->chessCounts += 1;
        if (ui->radioType->isChecked()&&(!is_win()))
        {

            this->chessboard[int((x-X)*1.0/CHECK_WIDTH+0.5)][int((y-Y)*1.0/CHECK_WIDTH+0.5)] = 1;
            this->chessCounts += 1;
            this->update();
            computer();
        }
        else if(ui->radioType_2->isChecked()&&(!is_win()))
        {
            this->chessboard[int((x-X)*1.0/CHECK_WIDTH+0.5)][int((y-Y)*1.0/CHECK_WIDTH+0.5)] = (this->chessCounts%2 == 0)?1:-1;
            this->chessboard[int((x-X)*1.0/CHECK_WIDTH+0.5)][int((y-Y)*1.0/CHECK_WIDTH+0.5)] = (this->chessCounts%2 == 1)?1:-1;
        }
        this->update();      //update the window
    }
    /*
    QString m_t = QString::number(this->max_time, 10);
    QString s_t = QString::number(this->sum_time, 10);
    QString a_t = QString::number(this->average_time, 10);
    QString m_m = QString::number(this->max_memory, 10);
    QString a_m = QString::number(this->average_memory, 10);
    */
    if(is_win() ==1){
        cout << "is win" << endl;
        QMessageBox::information(NULL, "Game Over", "Black Win!", QMessageBox::Yes, QMessageBox::Yes);
        this->close();
    }
    else if(is_win()==-1){
        cout << "is not win" << endl;
        QMessageBox::information(NULL, "Game Over", "White Win!", QMessageBox::Yes, QMessageBox::Yes);
        this->close();
    }
}

int MainWindow::is_win(){
    int stat = 0;//0: not ended; 1: black win; -1: white win;
    if (timeValuePlayer->minute() == 0 && timeValuePlayer->second() == 0)
        return -1;
    else if (timeValueComputer->minute() == 0 && timeValueComputer->second() == 0)
        return 1;


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
            switch(chessboard[j][i]){
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
                switch(chessboard[i][j]){
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
                switch(chessboard[len-(i-j)][j]){
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
                switch(chessboard[i-j][len-j]){
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
                switch(chessboard[i-j][j]){
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
                switch(chessboard[len-(i-j)][len-j]){
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

int Max_Value(MatchState& current_state, int depth, int alpha, int beta, int max_depth);
int Min_Value(MatchState& current_state, int depth, int alpha, int beta, int max_depth);

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


int Max_Value(MatchState& current_state, int depth, int alpha, int beta, int max_depth)
{
    if (depth == max_depth)
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
        int min_value = Min_Value(*a, depth, alpha, beta, max_depth);
        if (min_value > v)
            v = min_value;
        if (v >= beta)
            return v;
        if (alpha < v)
            alpha = v;
    }
    return v;
}


int Min_Value(MatchState& current_state, int depth, int alpha, int beta, int max_depth)
{
    if (depth == max_depth)
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
        int max_value = Max_Value(*a, depth, alpha, beta, max_depth);
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
                if (priority <= 5)
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row1;
                    c = col1;
                }
            }
            //虽然没有冲四，但是也不能够置之不理的情境
            else if (cnt1 == 4 && (zero1 || zero2))
            {
                if (zero1 == 1)
                {
                    if (priority <= 5)
                    {
                        if (current_state.state[i][j] == 1)
                            priority = 10;
                        else
                            priority = 5;
                        r = row1;
                        c = col1;
                    }
                }
                else
                {
                    if (priority <= 5)
                    {
                        if (current_state.state[i][j] == 1)
                            priority = 10;
                        else
                            priority = 5;
                        r = row2;
                        c = col2;
                    }
                }
            }
            //出现双三的情况
            else if (cnt1 == 3 && (zero1 && zero2))
            {
                if (priority <= 3)
                {
                    r = row1;
                    c = col1;
                    //如果是自己的双三情况出现，应该首先攻占，优先度高一点
                    if (current_state.state[i][j] == 1)
                        priority = 4;
                    else
                        priority = 3;
                }
            }
            else if (cnt2 == 4)
            {
                if (priority <= 10)
                {
                    r = row3;
                    c = col3;
                    priority = 10;
                }
            }
            else if (cnt2 == 3 && (zero3 && zero4))
            {
                if (priority <= 4)
                {
                    r = row3;
                    c = col3;
                    priority = 4;
                }
            }
            else if (cnt3 == 4)
            {
                if (priority <= 5)
                {
                    r = row4;
                    c = col4;
                    priority = 5;
                }
            }
            else if (cnt3 == 3 && (zero5 && zero6))
            {
                if (priority <= 4)
                {
                    r = row4;
                    c = col4;
                    priority = 4;
                }
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
            for (int k = i + 1; k < 15; k++)
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
                if (priority <= 5)
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row1;
                    c = col1;
                }
            }
            //虽然没有冲四，但是也不能够置之不理的情境
            else if (cnt1 == 4 && (zero1 || zero2))
            {
                if (priority <= 5)
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
            }
            //出现双三的情况
            else if (cnt1 == 3 && (zero1 && zero2))
            {
                if (priority <= 3)
                {
                    //如果是自己的双三情况出现，应该首先攻占，优先度高一点
                    if (current_state.state[i][j] == 1)
                        priority = 4;
                    else
                        priority = 3;
                    r = row1;
                    c = col1;
                }
            }
            else if (cnt2 == 4)
            {
                if (priority <= 10)
                {
                    r = row3;
                    c = col3;
                    priority = 10;
                }
            }
            else if (cnt2 == 3 && (zero3 && zero4))
            {
                if (priority <= 4)
                {
                    r = row3;
                    c = col3;
                    priority = 4;
                }
            }
            else if (cnt3 == 4)
            {
                if (priority <= 5)
                {
                    r = row4;
                    c = col4;
                    priority = 5;
                }
            }
            else if (cnt3 == 3 && (zero5 && zero6))
            {
                if (priority <= 4)
                {
                    r = row4;
                    c = col4;
                    priority = 4;
                }
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
                if (priority <= 5)
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row1;
                    c = col1;
                }
            }
            //虽然没有冲四，但是也不能够置之不理的情境
            else if (cnt1 == 4 && (zero1 || zero2))
            {
                if (priority <= 5)
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
            }
            //出现双三的情况
            else if (cnt1 == 3 && (zero1 && zero2))
            {
                if (priority <= 3)
                {
                    //如果是自己的双三情况出现，应该首先攻占，优先度高一点
                    if (current_state.state[i][j] == 1)
                        priority = 4;
                    else
                        priority = 3;
                    r = row1;
                    c = col1;
                }
            }
            else if (cnt2 == 4)
            {
                if (priority <= 10)
                {
                    r = row3;
                    c = col3;
                    priority = 10;
                }
            }
            else if (cnt2 == 3 && (zero3 && zero4))
            {
                if (priority <= 4)
                {
                    r = row3;
                    c = col3;
                    priority = 4;
                }
            }
            else if (cnt3 == 4)
            {
                if (priority <= 5)
                {
                    r = row4;
                    c = col4;
                    priority = 5;
                }
            }
            else if (cnt3 == 3 && (zero5 && zero6))
            {
                if (priority <= 4)
                {
                    r = row4;
                    c = col4;
                    priority = 4;
                }
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
                if (priority <= 5)
                {
                    if (current_state.state[i][j] == 1)
                        priority = 10;
                    else
                        priority = 5;
                    r = row1;
                    c = col1;
                }
            }
            //虽然没有冲四，但是也不能够置之不理的情境
            else if (cnt1 == 4 && (zero1 || zero2))
            {
                if (priority <= 5)
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
            }
            //出现双三的情况
            else if (cnt1 == 3 && (zero1 && zero2))
            {
                if (priority <= 3)
                {
                    //如果是自己的双三情况出现，应该首先攻占，优先度高一点
                    if (current_state.state[i][j] == 1)
                        priority = 4;
                    else
                        priority = 3;
                    r = row1;
                    c = col1;
                }
            }
            else if (cnt2 == 4)
            {
                if (priority <= 10)
                {
                    r = row3;
                    c = col3;
                    priority = 10;
                }
            }
            else if (cnt2 == 3 && (zero3 && zero4))
            {
                if (priority <= 4)
                {
                    r = row3;
                    c = col3;
                    priority = 4;
                }
            }
            else if (cnt3 == 4)
            {
                if (priority <= 5)
                {
                    r = row4;
                    c = col4;
                    priority = 5;
                    cout << r << endl;
                    cout << c << endl;
                }
            }
            else if (cnt3 == 3 && (zero5 && zero6))
            {
                if (priority <= 4)
                {
                    r = row4;
                    c = col4;
                    priority = 4;
                }
            }
        }
    }

    if (priority != 0)
        return priority;
    return 0;
}

int miniMaxSearchForFive(const MatchState& currentState, int& r, int& c, int max_depth)
{
    QVector<Action> actions;
    QVector<double> values;
    int t = 0;
    //首先应该先检验一下是否出现了危险的境地，也就是失误之后必输的情景
    t = guarantee(currentState, r, c);
    if (t != 0)
        return 0;
    getAvailableAction(currentState, actions);
    //在这里需要我们限制极大极小的深度搜索
    int depth = 0;
    //为了能够减少搜索的时间复杂度，在这里采用alpha beta剪枝策略
    for (int i = 0; i < (int)actions.size(); ++i)
    {
        values.push_back(Min_Value(*actionResult(currentState, actions[i], 1), depth, -10000, 10000, max_depth));
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
    return 1;
}

void MainWindow::computer(){
    int r,c;
    //clock_t clockBegin, clockEnd; //分别记录搜索前后的时间
    //DWORD dwValue1,dwValue2;//分别记录搜索前后可用内存
    MatchState currentState;
    //MEMORYSTATUS mem;
    int strategy;//strategy 用来判断miniMaxSearchForFive采取的是搜索策略（1）还是防御策略（0）
    for (int i = 0; i < 15 * 15; ++i)
    {
        currentState.state[i / 15][i % 15] = chessboard[i/15][i%15];
    }
    //GlobalMemoryStatus(&mem);
    //dwValue1 = mem.dwAvailPhys / mb;
    //clockBegin = clock();
    //max_depth代表的是游戏的难度，1为简单，2为中等，3为困难
    strategy = miniMaxSearchForFive(currentState,r,c, max_depth);
    //GlobalMemoryStatus(&mem);
    //dwValue2 = mem.dwAvailPhys / mb;
    //clockEnd = clock();
    /*
    if(strategy){   //只有在搜索策略下才记录相应的内存和时间消耗
        this->count = this->count +1;
        if(this->max_memory<(dwValue1-dwValue2)){
            this->max_memory = dwValue1-dwValue2;
        }
        if(this->max_time<(clockEnd-clockBegin)){
            this->max_time = clockEnd - clockBegin;
        }
        this->sum_memory = this->sum_memory + dwValue1-dwValue2;
        this-> average_memory = this-> sum_memory / this->count;
        this->sum_time = this->sum_time + clockEnd-clockBegin;
        this->average_time = this->sum_time / this->count;
    }
    */
    this->chessboard[r][c] = -1;
}



