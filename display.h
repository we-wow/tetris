#include "data.h"
//��8255д�������
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

//��8253д�������
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

//��8255A��д����
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

//��8255B��д����
//B�ڶ�Ӧ�����ݱ�־λ
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


//��8255C��д����
//C�ڶ�Ӧ12864�Ŀ���λ
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

//��12864д�������
/*��Ϊ������C�ڽ�����λ�Ὣ8055��ԭ����������գ�
���ÿ��C�ڲ��ò���д��ķ�ʽ*/
void writeCmd12864(unsigned char cmd)
{
	writeData8255C(0x10);  //0b00010000д���������
	writeData8255B(cmd);
	writeData8255C(0x00);  //�͵�ƽ��������
}

//�����У�д��X�Ĵ���
void setRow(unsigned char x)
{
    x = 0x07 & x;
    x = 0xb8 | x;
    writeData8255C(0x10);
    writeData8255B(x);
    writeData8255C(0x00);
}

//�����У�д��Y�Ĵ���
void setCol(unsigned char y)
{
    y = y & 0x3f; //ȡ����λ
    y = y | 0x40;
    writeData8255C(0x10);
    writeData8255B(y);
    writeData8255C(0x00);
}


//��һ��������д�������
//�ڶ�����������Ļѡ��0ȫ����1����Ļ��2����Ļ
void writeData12864(unsigned char d, unsigned char screen)
{
    unsigned char cmdBegin;
    unsigned char cmdEnd;
    screen = screen & 0x03;
    cmdBegin = screen | 0x14;
    cmdEnd = screen | 0x04;
	writeData8255C(cmdBegin);  //��������д��
	writeData8255B(d);
    writeData8255C(cmdEnd);  //��������
}

//��Ļȫ�����
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

//д��һ��64*64��ͼƬ,��һ��������ͼƬ���飬�ڶ�������Ļ
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

//��Ļ��ʼ��
void screenInitate()
{
    writeCmd12864(0x3f);  //������Ļ
    clrScreen();  //��ʼ��
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