
#include "display.h"
unsigned int score=0;                   //游戏得分
unsigned char gameState=0;              //游戏是否开始
void delay(unsigned int i)              //delay 1mS
{
    unsigned char x;
    for(i;i>0;i--) for(x=250;x>0;x--);   
}

//产生新的块
void getNewDot()                        
{
    unsigned char rand;
    __asm                       //随机数产生算法，利用交互不确定性
    {
        mov dx,290h
        in al,dx
        mov ah,al
        in al,dx
        mov rand,ah
    }
    tet.tType = rand % 5 + 1;
    tet.dir = 0;
    tet.dotNum = 4;
    switch (tet.tType)
    {
        case 1:
        {
            tet.x[0] = 2;
            tet.x[1] = 3;
            tet.x[2] = 4;
            tet.x[3] = 2;
            tet.y[0] = 0;
            tet.y[1] = 0;
            tet.y[2] = 0;
            tet.y[3] = 1;
            break;
        }
        case 2:
        {
            tet.x[0] = 2;
            tet.x[1] = 3;
            tet.x[2] = 4;
            tet.x[3] = 3;
            tet.y[0] = 0;
            tet.y[1] = 0;
            tet.y[2] = 0;
            tet.y[3] = 1;
            break;
        }
        case 3:
        {
            tet.x[0] = 3;
            tet.x[1] = 3;
            tet.x[2] = 4;
            tet.x[3] = 4;
            tet.y[0] = 2;
            tet.y[1] = 1;
            tet.y[2] = 1;
            tet.y[3] = 0;
            break;
        }
        case 4:
        {
            tet.x[0] = 2;
            tet.x[1] = 3;
            tet.x[2] = 4;
            tet.x[3] = 3;
            tet.y[0] = 0;
            tet.y[1] = 0;
            tet.y[2] = 0;
            tet.y[3] = 0;
            tet.dotNum=3;
            break;
        }
        case 5:
        {
            tet.x[0] = 3;
            tet.x[1] = 4;
            tet.x[2] = 3;
            tet.x[3] = 4;
            tet.y[0] = 0;
            tet.y[1] = 0;
            tet.y[2] = 1;
            tet.y[3] = 1;
            break;
        }
        default:
            break;
    }
}

void startGame()                          //游戏开始
{
    gameState=1;
    for (unsigned char i = 0; i < 16; i++)
    {
        for (unsigned char j = 0; j < 8; j++) dotOcc[i][j]=0;     //碰撞表全部清零       
    }
    clrScreen();                                                  //屏幕清空
    for (unsigned char j = 0; j < 6; j++)
    {
        setRow(6-j);
        setCol(16);
        for (unsigned char i = 0; i < 16; i++) writeData12864(tetris[j][i], 2);
    }
    for (unsigned char j = 0; j < 6; j++)
    {
        setRow(6-j);
        setCol(32);
        for (unsigned char i = 0; i < 16; i++) writeData12864(ready[j][i], 2);
    }
    delay(200);
    clrScreen();                                                  //屏幕清空
    __asm
    {
        push ax
        push dx
        mov dx, 296H
        mov al, 70H
        out dx, al
        mov ax,10000
        mov dx,292H                 ;由CNT1工作
        out dx,al                   ;装入初值低八位
        mov al,ah
        out dx,al                   ;装入初值高八位
        pop dx
        pop ax
    }
    getNewDot();
    writeTet();
}

void endGame()                              //游戏结束
{
    gameState=0;                            //游戏状态置零
    delay(100);                             //延时，玩家心理缓冲时间
    clrScreen();                            //游戏结束，清屏
    for (unsigned char j = 0; j < 8; j++)   //准备打印得分信息
    {
        setRow(7-j);
        setCol(0);
        for (unsigned char i = 0; i < 16; i++) writeData12864(gameOverText[j][i], 2);

    }
    for (unsigned char j = 0; j < 6; j++)
    {
        setRow(6-j);
        setCol(16);
        for (unsigned char i = 0; i < 16; i++) writeData12864(scoreText[j][i], 2);

    }
    if (score>9999) score=9999;
    unsigned char dispNum[4]={0,0,0,0};
    unsigned char e=4;
    while(score)
    {
        e--;
        dispNum[e]=score % 10;
        score = score / 10;
    }
    //结合提高题，此处可以消隐，但没有必要。以显示得分上限
    for (unsigned i = 0; i < 4; i++)
    {
        setRow(5-i);
        setCol(32);
        for (unsigned j = 0; j < 16; j++) writeData12864(number[dispNum[i]][j], 2);
    }
}

void keyScan()
{
    unsigned char key,i,j;
    __asm
    {
        push ax
        push dx
        mov dx,280h                     //检测按键号码
        in al,dx
        and al,3fh                      //屏蔽高两位
        mov key,al
        pop dx
        pop ax
    }
    if (gameState) clrTet();
    switch (key)
    {
        case 32:                        //游戏结束
        {
            if (gameState) endGame();                  //显示游戏结束画面
            break;
        }
        case 16:                        //游戏开始
        {
            if (!gameState) startGame();
            break;
        }
        case 8:                         //向左移动
        {
            if (tet.x[0] > 0 && tet.x[1] > 0 && tet.x[2] > 0 && tet.x[3] > 0)
            {
                unsigned char notCrash=1;
                for ( i = 0; i < tet.dotNum; i++) 
                {
                    if (dotOcc[tet.y[i]][tet.x[i]-1])
                    {
                        notCrash = 0;                //检测到将产生碰撞
                        break;
                    }
                }
                if (notCrash)
                {
                    tet.x[0]--;
                    tet.x[1]--;
                    tet.x[2]--;
                    tet.x[3]--;
                }
            }
            break;
        }
        case 2:                         //向右移动
        {
            
            if (tet.x[0] < 7 && tet.x[1] < 7 && tet.x[2] < 7 && tet.x[3] < 7)
            {
                unsigned char notCrash=1;
                for ( i = 0; i < tet.dotNum; i++) 
                {
                    if (dotOcc[tet.y[i]][tet.x[i]+1])
                    {
                        notCrash = 0;                //检测到将产生碰撞
                        break;
                    }
                }
                if (notCrash)
                {
                    tet.x[0]++;
                    tet.x[1]++;
                    tet.x[2]++;
                    tet.x[3]++;
                }
                
                
            }
            break;
        }
        case 1:                         //变化形状
        {
            switch (tet.tType)
            {
                case 1:
                {
                    switch (tet.dir)
                    {
                        case 0:
                        {
                            if (tet.y[1]!=0)
                            {
                                tet.x[1]=tet.x[0];
                                tet.y[1]=tet.y[0]-1;
                                tet.x[2]=tet.x[3]+1;
                                tet.y[2]=tet.y[3];
                                tet.dir++;
                            }
                            break;   
                            }
                        case 1:
                        {
                            tet.x[0]=tet.x[2]+1;
                            tet.y[0]=tet.y[2];
                            tet.x[1]=tet.x[0];
                            tet.y[1]=tet.y[0]-1;
                            tet.dir++;
                            break;   
                        }
                        case 2:
                        {
                            tet.x[2]=tet.x[1];
                            tet.y[2]=tet.y[1]-1;
                            tet.x[3]=tet.x[2]-1;
                            tet.y[3]=tet.y[2];
                            tet.dir++;
                            break;   
                        }
                        case 3:
                        {
                            tet.x[2]=tet.x[1];      //对X2进行定位
                            tet.y[2]=tet.y[1];
                            tet.x[1]=tet.x[2]-1;
                            tet.y[1]=tet.y[2];
                            tet.x[0]=tet.x[1]-1;
                            tet.y[0]=tet.y[1];
                            tet.x[3]=tet.x[0];
                            tet.y[3]=tet.y[0]+1;
                            tet.dir=0;
                            break;   
                        }
                        default:
                            break;  
                    }
                    break;
                }
                case 2:
                {
                    {
                        switch (tet.dir)
                        {
                            case 0:
                            {
                                if (tet.y[0]!=0)
                                {
                                    tet.x[0]=tet.x[1];
                                    tet.y[0]=tet.y[1]-1;
                                    tet.dir++;
                                }
                                break;   
                            }
                            case 1:
                            {
                                tet.x[3]=tet.x[1]-1;
                                tet.y[3]=tet.y[1];
                                tet.dir++;
                                break;   
                            }
                            case 2:
                            {
                                tet.x[2]=tet.x[1];
                                tet.y[2]=tet.y[1]+1;
                                tet.dir++;
                                break;   
                            }
                            case 3:
                            {
                                tet.x[0]=tet.x[1]-1;
                                tet.y[0]=tet.y[1];
                                tet.x[2]=tet.x[1]+1;
                                tet.y[2]=tet.y[1];
                                tet.x[3]=tet.x[1];
                                tet.y[3]=tet.y[1]+1;   
                                tet.dir=0;
                                break;   
                            }
                            default:
                                break;   
                        }
                        break;
                    }
                }
                case 3:
                {
                    switch(tet.dir)                //形状三有两种变换
                    {
                        case 0:
                        {
                            tet.x[3]=tet.x[2];
                            tet.y[3]=tet.y[2]+1;
                            tet.x[0]=tet.x[3]+1;
                            tet.y[0]=tet.y[3];
                            tet.dir=1;
                            break;
                        }
                        case 1:
                        {
                            tet.x[3]=tet.x[2];
                            tet.y[3]=tet.y[2]-1;
                            tet.x[0]=tet.x[1];
                            tet.y[0]=tet.y[1]+1;
                            tet.dir=0;
                            break;
                        }
                    }
                    break;
                }
                case 4:
                {
                    switch(tet.dir)                //形状四有两种变换
                    {
                        case 0:
                        {
                            if (tet.y[1]!=0)
                            {
                                tet.x[2]=tet.x[1];
                                tet.y[2]=tet.y[1]+1;
                                tet.x[0]=tet.x[1];
                                tet.y[0]=tet.y[1]-1;
                                tet.dir=1;
                            }
                            break;
                        }
                        case 1:
                        {
                            tet.x[2]=tet.x[1]+1;
                            tet.y[2]=tet.y[1];
                            tet.x[0]=tet.x[1]-1;
                            tet.y[0]=tet.y[1];
                            tet.dir=0;
                            break;
                        }
                    }
                    break;
                }
                break;
            }
            break;
        }
        case 4:             //移动，快速下降
        {   
            unsigned char goCol,col=0;
            for (goCol = 0; goCol < 16; goCol++)
            {
                
                for (i = 0; i < tet.dotNum; i++)
                {
                    if (dotOcc[tet.y[i]+goCol][tet.x[i]])
                    {
                        if (tet.y[i]+goCol>=16 && tet.tType==1 && tet.dir==0) for (i = 0; i < tet.dotNum; i++) tet.y[i]=tet.y[i]+goCol-3;
                        else if (tet.y[i]+goCol>=17) for (i = 0; i < tet.dotNum; i++) tet.y[i]=tet.y[i]+goCol-3;
                        else for (i = 0; i < tet.dotNum; i++) tet.y[i]=tet.y[i]+goCol-2;
                        col=1;
                        break;
                    }
                    
                }
                if (col) break;
            }
            break;
        }
        default:
            break;
    }
    if (gameState) writeTet();

    __asm
    {
        mov dx,2A0H
        mov al,20H                      ;固定优先级，一般全嵌套方式，EOI命令
        out dx,al
        pop dx
        pop ax
        sti
    }
}

void autoDown()                         //每一秒自动下降一格
{
    __asm
    {
        push dx
        push ax
        mov dx,2A0H
        mov al,20H                      ;固定优先级，一般全嵌套方式，EOI命令
        out dx,al
    }
    if (gameState){
    __asm
    {
        mov ax,10000
        mov dx,292H                     ;重装初值
        out dx,al                       ;装入初值低八位
        mov al,ah
        out dx,al                       ;装入初值高八位
    }
    unsigned char i=0,j=0,gameOver=0;
    for (j=0;j<tet.dotNum;j++)               //灭掉原来的图形
    {
        if (tet.y[j]<8)
        {
            setRow(7-tet.x[j]);
            setCol(tet.y[j]*8);
            for(i=0;i<8;i++)
                {
                    writeData12864(0x00,2);
                }
        }
        else
        {
            setRow(7-tet.x[j]);
            setCol(tet.y[j]*8-64);
            for(i=0;i<8;i++)
                {
                    writeData12864(0x00,1);
                }
        }
    }
    for (i = 0; i < tet.dotNum; i++) tet.y[i]++;    //向下移动一格
    for (j=0;j<tet.dotNum;j++)                      //点亮新的图形
    {
        if (tet.y[j]<8)
        {
            setRow(7-tet.x[j]);
            setCol(tet.y[j]*8);
            for(i=0;i<8;i++)
                {
                    writeData12864(dot[i],2);
                }
        }
        else
        {
            setRow(7-tet.x[j]);
            setCol(tet.y[j]*8-64);
            for(i=0;i<8;i++)
                {
                    writeData12864(dot[i],1);
                }
        }
    }
    unsigned crashed = 0;
    //设置碰撞触发条件
    for ( i = 0; i < tet.dotNum; i++) 
    {
        if (dotOcc[tet.y[i]+1][tet.x[i]])
        {
            crashed = 1;                //检测到下一次将产生碰撞
            break;
        }
    }
    if (tet.y[0]==15 || tet.y[1]==15 || tet.y[2]==15 || tet.y[3]==15 || crashed)       
    {
        //碰撞检测，一旦碰撞，记录当前位置信息
        for ( i = 0; i < tet.dotNum; i++) dotOcc[tet.y[i]][tet.x[i]] = 1;
        //行消除检测
        unsigned char allZero=1,allOne,zeroLine,oneLine=0,k,m;
        for (i = 0; i < 16; i++)
        {
            allZero=1;allOne=1;
            for ( j = 0; j < 8; j++)
            {
                if (dotOcc[15-i][j]) allZero=0;     //非全零行
                else allOne=0;                      //非全一行
            }
            if (allOne)                             //全一行执行消除程序
            {
                for (j = i; j < 16; j++)
                {
                    zeroLine=1;
                    for ( k = 0; k < 8; k++)
                    {
                        dotOcc[15-j][k] = dotOcc[15-j-1][k];
                        if (dotOcc[15-j-1][k])
                        {
                            zeroLine=0;
                            setRow(7-k);
                            if ( j < 8)
                            {
                                setCol((7-j)*8);
                                for (m = 0; m < 8; m++) writeData12864(dot[m],1);
                            }
                            else
                            {
                                setCol(j*8);
                                for (m = 0; m < 8; m++) writeData12864(dot[m],2);
                            }
                        }
                        else
                        {
                            setRow(7-k);
                            if ( j < 8)
                            {
                                setCol((7-j)*8);
                                for (m = 0; m < 8; m++) writeData12864(0x00,1);
                            }
                            else
                            {
                                setCol(j*8);
                                for (m = 0; m < 8; m++) writeData12864(0x00,2);
                            }
                        }
                        
                    }
                    if (zeroLine) break;                    
                }
                score++;                            //分数加1
                i--;                                //维持当前扫描行
            }
            else if (allZero) break;                //遇到全零行则检测完毕
        }   
        //随机产生下一个方块
        getNewDot();
        for (j=0;j<tet.dotNum;j++)               //点亮新的图形
        {
            if (tet.y[j]<8)
            {
                setRow(7-tet.x[j]);
                setCol(tet.y[j]*8);
                for(i=0;i<8;i++)
                    {
                        writeData12864(dot[i],2);
                    }
            }
            else
            {
                setRow(7-tet.x[j]);
                setCol(tet.y[j]*8-64);
                for(i=0;i<8;i++)
                    {
                        writeData12864(dot[i],1);
                    }
            }
        }                                       //进行结束检测
        for ( i = 0; i < tet.dotNum; i++) 
        {
            if (dotOcc[tet.y[i]+1][tet.x[i]])
            {
                gameOver = 1;                   //检测到下一次将产生碰撞，游戏结束
                break;
            }
        }
        
    }
    if (gameOver) endGame();
    }
    __asm
    {
        sti
        pop ax
        pop dx
    }
    
    
    
}

void main(void)
 {
     __asm                              //外部可屏蔽中断（INTR）程序装入
    {
        PUSHF
        PUSH AX
        PUSH ES
        PUSH DI
        CLI                             ;屏蔽外部中断
        MOV AX,0
        MOV ES,AX
        MOV DI,4*08H                    ;IR0中断类型号00001000B，实现按键检测功能
        MOV AX, OFFSET keyScan
        MOV WORD PTR ES:[DI],AX
        MOV AX, SEG keyScan
        MOV WORD PTR ES:[DI+2],AX
        MOV DI,4*0BH                    ;IR3中断类型号00001011B，实现自动下降功能
        MOV AX, OFFSET autoDown
        MOV WORD PTR ES:[DI],AX
        MOV AX, SEG autoDown
        MOV WORD PTR ES:[DI+2],AX
        POP DI
        POP ES
        POP AX
        POPF
    }
    //8259初始化
    __asm
    {
        push ax
        push dx
        cli
        mov dx,2A0H
        mov al,13H                      ;初始化ICW1,不需要ICW3
        out dx,al
        mov dx,2A2H                     ;初始化ICW2
        mov al,08H
        out dx,al
        mov al,01H                      ;初始化ICW4,非自动结束中断方式
        out dx,al
        mov al,0F6H                     ;屏蔽除IR3外其余中断
        out dx,al
        sti
        pop dx
        pop ax
    }
    writeCmd8255(0x90);                 //8255初始化
    for (unsigned i = 0; i < 8; i++)
    {
        setRow(i);
        setCol(0);
        for (unsigned j = 0; j < 64; j++) writeData12864(~emoji_left[64*i+j],2);
    }
    for (unsigned i = 0; i < 8; i++)
    {
        setRow(i);
        setCol(0);
        for (unsigned j = 0; j < 64; j++) writeData12864(~emoji_right[64*i+j],1);
    }
    delay(500);
    __asm                               //8253CNT0初始化
    {
        push ax
        push dx
        mov dx, 296H
        mov al, 36H
        out dx, al
        mov ax,100
        mov dx,290H                     ;由CNT0工作
        out dx,al                       ;装入初值低八位
        mov al,ah
        out dx,al                       ;装入初值高八位
        pop dx
        pop ax
    }
    screenInitate();                    //屏幕初始化
    startGame();                        //游戏自动开始
    while (1);                          //等待中断
 }