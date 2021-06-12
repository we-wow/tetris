
#include "display.h"
unsigned int score=0;                   //��Ϸ�÷�
unsigned char gameState=0;              //��Ϸ�Ƿ�ʼ
void delay(unsigned int i)              //delay 1mS
{
    unsigned char x;
    for(i;i>0;i--) for(x=250;x>0;x--);   
}

//�����µĿ�
void getNewDot()                        
{
    unsigned char rand;
    __asm                       //����������㷨�����ý�����ȷ����
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

void startGame()                          //��Ϸ��ʼ
{
    gameState=1;
    for (unsigned char i = 0; i < 16; i++)
    {
        for (unsigned char j = 0; j < 8; j++) dotOcc[i][j]=0;     //��ײ��ȫ������       
    }
    clrScreen();                                                  //��Ļ���
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
    clrScreen();                                                  //��Ļ���
    __asm
    {
        push ax
        push dx
        mov dx, 296H
        mov al, 70H
        out dx, al
        mov ax,10000
        mov dx,292H                 ;��CNT1����
        out dx,al                   ;װ���ֵ�Ͱ�λ
        mov al,ah
        out dx,al                   ;װ���ֵ�߰�λ
        pop dx
        pop ax
    }
    getNewDot();
    writeTet();
}

void endGame()                              //��Ϸ����
{
    gameState=0;                            //��Ϸ״̬����
    delay(100);                             //��ʱ�����������ʱ��
    clrScreen();                            //��Ϸ����������
    for (unsigned char j = 0; j < 8; j++)   //׼����ӡ�÷���Ϣ
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
    //�������⣬�˴�������������û�б�Ҫ������ʾ�÷�����
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
        mov dx,280h                     //��ⰴ������
        in al,dx
        and al,3fh                      //���θ���λ
        mov key,al
        pop dx
        pop ax
    }
    if (gameState) clrTet();
    switch (key)
    {
        case 32:                        //��Ϸ����
        {
            if (gameState) endGame();                  //��ʾ��Ϸ��������
            break;
        }
        case 16:                        //��Ϸ��ʼ
        {
            if (!gameState) startGame();
            break;
        }
        case 8:                         //�����ƶ�
        {
            if (tet.x[0] > 0 && tet.x[1] > 0 && tet.x[2] > 0 && tet.x[3] > 0)
            {
                unsigned char notCrash=1;
                for ( i = 0; i < tet.dotNum; i++) 
                {
                    if (dotOcc[tet.y[i]][tet.x[i]-1])
                    {
                        notCrash = 0;                //��⵽��������ײ
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
        case 2:                         //�����ƶ�
        {
            
            if (tet.x[0] < 7 && tet.x[1] < 7 && tet.x[2] < 7 && tet.x[3] < 7)
            {
                unsigned char notCrash=1;
                for ( i = 0; i < tet.dotNum; i++) 
                {
                    if (dotOcc[tet.y[i]][tet.x[i]+1])
                    {
                        notCrash = 0;                //��⵽��������ײ
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
        case 1:                         //�仯��״
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
                            tet.x[2]=tet.x[1];      //��X2���ж�λ
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
                    switch(tet.dir)                //��״�������ֱ任
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
                    switch(tet.dir)                //��״�������ֱ任
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
        case 4:             //�ƶ��������½�
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
        mov al,20H                      ;�̶����ȼ���һ��ȫǶ�׷�ʽ��EOI����
        out dx,al
        pop dx
        pop ax
        sti
    }
}

void autoDown()                         //ÿһ���Զ��½�һ��
{
    __asm
    {
        push dx
        push ax
        mov dx,2A0H
        mov al,20H                      ;�̶����ȼ���һ��ȫǶ�׷�ʽ��EOI����
        out dx,al
    }
    if (gameState){
    __asm
    {
        mov ax,10000
        mov dx,292H                     ;��װ��ֵ
        out dx,al                       ;װ���ֵ�Ͱ�λ
        mov al,ah
        out dx,al                       ;װ���ֵ�߰�λ
    }
    unsigned char i=0,j=0,gameOver=0;
    for (j=0;j<tet.dotNum;j++)               //���ԭ����ͼ��
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
    for (i = 0; i < tet.dotNum; i++) tet.y[i]++;    //�����ƶ�һ��
    for (j=0;j<tet.dotNum;j++)                      //�����µ�ͼ��
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
    //������ײ��������
    for ( i = 0; i < tet.dotNum; i++) 
    {
        if (dotOcc[tet.y[i]+1][tet.x[i]])
        {
            crashed = 1;                //��⵽��һ�ν�������ײ
            break;
        }
    }
    if (tet.y[0]==15 || tet.y[1]==15 || tet.y[2]==15 || tet.y[3]==15 || crashed)       
    {
        //��ײ��⣬һ����ײ����¼��ǰλ����Ϣ
        for ( i = 0; i < tet.dotNum; i++) dotOcc[tet.y[i]][tet.x[i]] = 1;
        //���������
        unsigned char allZero=1,allOne,zeroLine,oneLine=0,k,m;
        for (i = 0; i < 16; i++)
        {
            allZero=1;allOne=1;
            for ( j = 0; j < 8; j++)
            {
                if (dotOcc[15-i][j]) allZero=0;     //��ȫ����
                else allOne=0;                      //��ȫһ��
            }
            if (allOne)                             //ȫһ��ִ����������
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
                score++;                            //������1
                i--;                                //ά�ֵ�ǰɨ����
            }
            else if (allZero) break;                //����ȫ�����������
        }   
        //���������һ������
        getNewDot();
        for (j=0;j<tet.dotNum;j++)               //�����µ�ͼ��
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
        }                                       //���н������
        for ( i = 0; i < tet.dotNum; i++) 
        {
            if (dotOcc[tet.y[i]+1][tet.x[i]])
            {
                gameOver = 1;                   //��⵽��һ�ν�������ײ����Ϸ����
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
     __asm                              //�ⲿ�������жϣ�INTR������װ��
    {
        PUSHF
        PUSH AX
        PUSH ES
        PUSH DI
        CLI                             ;�����ⲿ�ж�
        MOV AX,0
        MOV ES,AX
        MOV DI,4*08H                    ;IR0�ж����ͺ�00001000B��ʵ�ְ�����⹦��
        MOV AX, OFFSET keyScan
        MOV WORD PTR ES:[DI],AX
        MOV AX, SEG keyScan
        MOV WORD PTR ES:[DI+2],AX
        MOV DI,4*0BH                    ;IR3�ж����ͺ�00001011B��ʵ���Զ��½�����
        MOV AX, OFFSET autoDown
        MOV WORD PTR ES:[DI],AX
        MOV AX, SEG autoDown
        MOV WORD PTR ES:[DI+2],AX
        POP DI
        POP ES
        POP AX
        POPF
    }
    //8259��ʼ��
    __asm
    {
        push ax
        push dx
        cli
        mov dx,2A0H
        mov al,13H                      ;��ʼ��ICW1,����ҪICW3
        out dx,al
        mov dx,2A2H                     ;��ʼ��ICW2
        mov al,08H
        out dx,al
        mov al,01H                      ;��ʼ��ICW4,���Զ������жϷ�ʽ
        out dx,al
        mov al,0F6H                     ;���γ�IR3�������ж�
        out dx,al
        sti
        pop dx
        pop ax
    }
    writeCmd8255(0x90);                 //8255��ʼ��
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
    __asm                               //8253CNT0��ʼ��
    {
        push ax
        push dx
        mov dx, 296H
        mov al, 36H
        out dx, al
        mov ax,100
        mov dx,290H                     ;��CNT0����
        out dx,al                       ;װ���ֵ�Ͱ�λ
        mov al,ah
        out dx,al                       ;װ���ֵ�߰�λ
        pop dx
        pop ax
    }
    screenInitate();                    //��Ļ��ʼ��
    startGame();                        //��Ϸ�Զ���ʼ
    while (1);                          //�ȴ��ж�
 }