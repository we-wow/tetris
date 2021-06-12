#include "data.h"
//向8255写入控制字
void writeCmd8255(unsigned char cmd)
{
    __asm
    { 
        push dx
        push ax
        mov dx, 286H
        mov al, cmd
        out dx, al
        pop ax
        pop dx
    }
}

//向8253写入控制字
void writeCmd8253(unsigned char cmd)
{
    __asm
    { 
        push dx
        push ax
        mov dx, 296H
        mov al, cmd
        out dx, al
        pop ax
        pop dx
    }
}

//向8255A口写数据
void writeData8255A(unsigned char d)
{   
    __asm
    { 
        push dx
        push ax
        mov dx, 280H
        mov al, d
        out dx, al
        pop ax
        pop dx
    }
}

//向8255B口写数据
//B口对应其数据标志位
void writeData8255B(unsigned char d)
{
    __asm
    { 
        push dx
        push ax
        mov dx, 282H
        mov al, d
        out dx, al
        pop ax
        pop dx
    }
}


//向8255C口写数据
//C口对应12864的控制位
void writeData8255C(unsigned char d)
{
    __asm
    { 
        push dx
        push ax
        mov dx, 284H
        mov al, d
        out dx, al
        pop ax
        pop dx
    }
}

//向12864写入控制字
/*因为单独对C口进行置位会将8055的原本控制字清空，
因此每次C口采用并行写入的方式*/
void writeCmd12864(unsigned char cmd)
{
	writeData8255C(0x10);  //0b00010000写入控制命令
	writeData8255B(cmd);
	writeData8255C(0x00);  //低电平锁存数据
}

//设置行，写入X寄存器
void setRow(unsigned char x)
{
    x = 0x07 & x;
    x = 0xb8 | x;
    writeData8255C(0x10);
    writeData8255B(x);
    writeData8255C(0x00);
}

//设置列，写入Y寄存器
void setCol(unsigned char y)
{
    y = y & 0x3f; //取低五位
    y = y | 0x40;
    writeData8255C(0x10);
    writeData8255B(y);
    writeData8255C(0x00);
}


//第一个参数是写入的数据
//第二个参数是屏幕选择，0全屏，1右屏幕，2左屏幕
void writeData12864(unsigned char d, unsigned char screen)
{
    unsigned char cmdBegin;
    unsigned char cmdEnd;
    screen = screen & 0x03;
    cmdBegin = screen | 0x14;
    cmdEnd = screen | 0x04;
	writeData8255C(cmdBegin);  //允许数据写入
	writeData8255B(d);
    writeData8255C(cmdEnd);  //锁存数据
}

//屏幕全部清空
void clrScreen()
{
    unsigned char i, j;
    for(i=0;i<8;i++)
	{
		setRow(i);
		setCol(0);
		for(j=0;j<64;j++)
		{
			writeData12864(0x00,0);
		}
	}
}

//写入一张64*64的图片,第一个参数是图片数组，第二个是屏幕
void writePic(unsigned char array[512], unsigned char s)
{
    unsigned i,j;
    for(i=0;i<8;i++)
	{
		setRow(i);
		setCol(0);
		for(j=0;j<64;j++)
		{
			writeData12864(array[i*64+j], s);
		}
		
	}
}

//屏幕初始化
void screenInitate()
{
    writeCmd12864(0x3f);  //开启屏幕
    clrScreen();  //初始化
}

void clrTet()
{
    unsigned char i=0,j=0;
    for (j=0;j<tet.dotNum;j++)
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
}
    

void writeTet()
{
    unsigned char i=0,j=0;
    for (j=0;j<tet.dotNum;j++)
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
}