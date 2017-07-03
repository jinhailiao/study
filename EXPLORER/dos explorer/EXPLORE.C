/***************************************************************************/
/* Copyright (C) 2004, Haisoft有限公司                                     */
/* All rights reserved.                                                    */

/* 文件名称: Explore.c                                                     */
/* 摘    要：仿WINDOWS中的扫雷的游戏 ，加入了玩家管理功能                  */

/* 当前版本: 1.0                                                           */
/* 作    者: 廖金海                                                        */
/* 完成日期: 2004.6.1.                                                     */
/***************************************************************************/

/************************* include files ***********************************/
#include <stdio.h>
#include <dos.h>
#include <graphics.h>
#include <stdlib.h>
#include <alloc.h>
#include <string.h>

/************************** macro define ***********************************/
#define MHIGH 18        /* 鼠标的高度           */  
#define MLEN  11        /* 鼠标的宽度           */
#define GRIDLEN 15      /* 方格的宽度（正方形） */

/************************* data structure **********************************/
struct grid             /* 方格的数据结构 */
  {
    int mine;           /* -1：有雷； 0-8：周围八格雷的情况 */
    int status;         /* 1：初始化；2：已排雷；3：标雷；4：怀疑；0：其它 */
    int check;          /* 1：标示正确；0：标示错误或无标示 */
  };

struct user             /* 玩家的数据结构 */
  {
   char name[7];        /* 姓名 */
   int  time[3];        /* 不同级别的最少时间 */
  };

struct HERO             /* 扫雷英雄的数据结构 */       
  {
    char name[7];       /* 扫雷英雄的姓名 */ 
    int time;           /* 扫雷英雄所用的最少时间 */
  };

/************************* global variable *********************************/
int gamestart, gametime;   /* gamestart:游戏开始； gametime:游戏用时；*/
int minenum;                                          /* 剩余雷的数目 */
int mouse[MHIGH][MLEN] = {{2,0,0,0,0,0,0,0,0,0,0},    /* 鼠标矩阵     */
              {2,2,0,0,0,0,0,0,0,0,0},
              {2,1,2,0,0,0,0,0,0,0,0},
              {2,1,1,2,0,0,0,0,0,0,0},
              {2,1,1,1,2,0,0,0,0,0,0},
              {2,1,1,1,1,2,0,0,0,0,0},
              {2,1,1,1,1,1,2,0,0,0,0},
              {2,1,1,1,1,1,1,2,0,0,0},
              {2,1,1,1,1,1,1,1,2,0,0},
              {2,1,1,1,1,1,1,1,1,2,0},
              {2,1,1,1,1,1,2,2,2,2,2},
              {2,1,1,1,1,1,2,0,0,0,0},
              {2,1,2,2,1,1,1,2,0,0,0},
              {2,2,0,2,1,1,1,2,0,0,0},
              {2,0,0,0,2,1,1,1,2,0,0},
              {0,0,0,0,2,1,1,1,2,0,0},
              {0,0,0,0,0,2,1,1,2,0,0},
              {0,0,0,0,0,2,2,2,0,0,0}};

struct grid board[20][20];        /* 雷区: 20*20; */
struct user player, players[10];  /* player为当前玩家，players为储存的玩家*/
struct HERO hero[3];              /* hero为扫雷英雄排行榜 */
struct time oldtime;              /* 记时时用来保存前一次读取的系统时间 */

/************************ function prototype *******************************/
void GrapInit(void);
int  MouseInit(int xmi, int xma, int ymi, int yma);
void ViewInit(int level);
int  LevelData(int level, int *x, int *y, int *length);
void QuitButton(int flag);
void AboutButton(int flag);
void AboutDialogBox(int *x, int *y, int *mvalue);
void HeroButton(int flag);
void HeroDialogBox(int *x, int *y, int *mvalue);
void ResetButton(int flag);
void DrawRedFlag(int x, int y);
void DrawMine(int x, int y);
void DrawFalse(int x, int y);
void DataInit(int level);
void PlayerInit(void);
void ViewPlayer(int level);
void SelectPlayer(int level, int *x, int *y, int *mvalue);
void EnterName(int level, int *x, int *y, int *mvalue);
void ViewScore(int no, int *x, int *y, int *mvalue);
int  PlayGameL(int startx, int starty, int x, int y, int gridnum);
void PlayGameR(int startx, int starty, int x, int y);
void CountMine(int flag);
void CountTime(int x, int y);
void OpenGrid(int startx, int starty, int i, int j, int gridnum);
int  Win(int gridnum);
void Record(int level, int *x, int *y, int *mvalue);
void GameOver(int startx, int starty, int gridnum);
int  GetMouse(int *mx, int *my, int *mvalue);
int  Get_KorM(int i, int *x, int *y, int *mvalue, int *key);
void Cursor(int mx, int my);
void DrawBoardOut(int x, int y, int sizex, int sizey);
void DrawBoardIn(int x, int y, int sizex, int sizey);
void WriteDisk(void);
void Quit(void);

/************************** main function **********************************/
void main()
{
 int operate;             /* 左击操作: 1, 为正确; 0: 为错误 */
 int x = 320, y = 240;    /* x,y当前鼠标位置 */
 int oldx, oldy;          /* oldx,oldy保存鼠标移动前的位置 */
 int button;              /* button获取按键情况  */
 int mvalue = 0;          /* mvalue保存鼠标前一次按键的值 */
 int level = 0;           /* 0:初级；1:中级；2:高级；*/
 int over = 0, reset;     /* over==1表示游戏结束,reset==1表示重置 */
 int gridnum;             /* gridnum为当前雷区中格子的数目 */
 int startx, starty;      /* startx, starty为雷区的左上角坐标 */
 int length;              /* length为雷区的宽度 */
 int mousesize;           /* mousesize为鼠标的字节数 */
 void *mousemap;          /* mousemap用来保存画鼠标前的区域 */
 
 GrapInit();                                       /* 图形初始化 */
 if(MouseInit(1, 629, 1, 479) == 0)                /* 鼠标初始化 */
  {
   printf("Mouse or Mouse Driver Absent,Please Install!\n");
   delay(5000);
   exit(0);
  }
 
 mousesize = imagesize(0, 0, MLEN-1, MHIGH-1);     /*申请鼠标大小的存储区域*/
 mousemap = malloc(mousesize);
 
 PlayerInit();             /* 玩家初始化 */                        
 
 while (1)
  {
    reset = 0;            /* reset初始为0，为1表示重置 */
    gamestart = 0;        /* gamestart为0表示游戏未开始或暂停，为1为开始 */
    
    gridnum = LevelData(level, &startx, &starty, &length);
                                             /* 获取跟游戏级别有关的数据 */            
    
    ViewInit(level);                         /* 界面初始化     */
    ViewPlayer(level);                       /* 玩家初始化     */
    DataInit(level);                         /* 游戏数据初始化 */
    
    getimage(x, y, x+MLEN-1, y+MHIGH-1, mousemap);   /* 画鼠标 */
    Cursor(x, y);

    while (1)
     {
       operate = 1;    /* operate为1表示排雷正确，为0表示错误，初始为1 */

       oldx = x;       /* 保存移动前的鼠示坐标 */
       oldy = y;

       button = GetMouse(&x, &y, &mvalue);      /* 获取鼠标按键值及坐标 */
       switch (button)
    {
      case -1: putimage(oldx, oldy, mousemap, COPY_PUT);  /* 左击 */
           if ((x>startx) && (x<startx+length)        /* 在雷区 */
            && (y>starty) && (y<starty+length))
             {
               gamestart = 1;
               operate = PlayGameL(startx, starty, x, y, gridnum);
             }

           else     /* 不在雷区 */
             {
               if ((x>407) && (x<427) && (y>35) && (y<55)) /* 重置 */
             {
               reset = 1;
                           ResetButton(0);
               Cursor(x, y);
                           delay(100);
                           ResetButton(1);
             }

               else if ((x>376) && (x<391) && (y>72) && (y<87))
                 {                               /* 选级别 */
                   DrawBoardIn(376, 72, 15, 15);
                   setcolor(0);
                   outtextxy(380, 75, "v");
                   Cursor(x, y);
                   delay(100);
                   DrawBoardOut(376, 72, 15, 15);
                   outtextxy(380, 75, "v");

                   if (level == 0)
                  {
                    level = 1;
                  }
                   else if (level == 1)
                       {
                     level = 2;
                       }
                    else
                       {
                     level = 0;
                       }

                   reset = 1;
                 }

               else if ((x>428) && (x<448) && (y>35) && (y<55))
                 {                              /* 扫雷英雄榜 */
                   HeroDialogBox(&x, &y, &mvalue);
                 }

               else if ((x>449) && (x<469) && (y>35) && (y<55))
                 {                              /* 关于对话框 */
                   AboutDialogBox(&x, &y, &mvalue);
                 }

               else if ((x>255) && (x<272) && (y>78) && (y<95))
                 {                              /* 选择玩家   */
                   SelectPlayer(level, &x, &y, &mvalue);
                 }

               else if ((x>197) && (x<252) && (y>78) && (y<93))
                 {                             /* 玩家姓名输入 */
                   EnterName(level, &x, &y, &mvalue);
                 }

               else if ((x>470) && (x<490) && (y>35) && (y<55))
                 {                             /* 退出游戏 */
                   over = 1;
                   QuitButton(0);
                   Cursor(x, y);
                   delay(100);
                   QuitButton(1);
                   delay(200);
                 }
             }
           if (operate == 1)           /* 左击正确 */
             {
              if (Win(gridnum) == 1)   /* 判断一下是否赢了 */
        	{
              gamestart = 0;       /* 为0，游戏计时停止 */

              setcolor(RED);
              outtextxy(270, 105, "OK! YOU WIN.");

              Record(level, &x, &y, &mvalue);/* 判断是否破纪录 */

              getimage(x, y, x+MLEN-1, y+MHIGH-1, mousemap);
              Cursor(x, y);

              while (1)
               {
                 oldx = x;
                             oldy = y;

                 button = GetMouse(&x, &y, &mvalue);

                 putimage(oldx, oldy, mousemap, COPY_PUT);
                 getimage(x, y, x+MLEN-1, y+MHIGH-1, mousemap);
                 Cursor(x, y);

                 if (button == -1)                /* 左击 */
                   {                              /* 游戏重置 */
                if ((x>407) && (x<427) && (y>35) && (y<55))
                  {                           
                    reset = 1;
                    ResetButton(0);
                    Cursor(x, y);
                    delay(100);
                    ResetButton(1);
                  }

                else if ((x>376) && (x<391)   /* 选取级别 */
                         && (y>72) && (y<87))
                      {                                
                    DrawBoardIn(376, 72, 15, 15);
                    setcolor(0);
                    outtextxy(380, 75, "v");
                    Cursor(x, y);
                    delay(100);
                    DrawBoardOut(376, 72, 15, 15);
                    outtextxy(380, 75, "v");

                    if (level == 0)
                       {
                         level = 1;
                       }
                    else if (level == 1)
                		{
                          level = 2;
                		}
                         else
                		{
                          level = 0;
                		}

                    reset = 1;
                      }

                else if ((x>428) && (x<448)   /* 扫雷英雄 */
                         && (y>35) && (y<55))
                      {                       
                    putimage(x, y, mousemap, COPY_PUT);
                    HeroDialogBox(&x, &y, &mvalue);
                      }

                else if ((x>449) && (x<469)  /* 关于对话框 */
                         && (y>35) && (y<55))
                      {                      
                    putimage(x, y, mousemap, COPY_PUT);
                    AboutDialogBox(&x, &y, &mvalue);
                      }

                else if ((x>255) && (x<272)  /* 选择玩家 */
                         && (y>78) && (y<95))
                      {
                    putimage(x, y, mousemap, COPY_PUT);
                    SelectPlayer(level, &x, &y, &mvalue);
                      }

                else if ((x>197) && (x<252)   /* 输入姓名 */
                         && (y>78) && (y<93))
                      {
                    putimage(x, y, mousemap, COPY_PUT);
                    EnterName(level, &x, &y, &mvalue);
                      }

                else if ((x>470) && (x<490)   /* 游戏退出 */
                         && (y>35) && (y<55))
                      {
                    over = 1;
                    QuitButton(0);
                    Cursor(x, y);
                    delay(100);
                    QuitButton(1);
                    delay(300);
                      }

                   }
                 if ((over == 1) || (reset == 1))
                   {
                 putimage(oldx, oldy, mousemap, COPY_PUT);
                 break;
                   }
               }
        	}
             }
           else         /* 游戏失败的处理 */
              {
            gamestart = 0;   /* 记时停止 */

            GameOver(startx, starty, gridnum);/* 打开剩余格子 */

            setcolor(RED);
            outtextxy(240, 105, "SHIT! YOU ARE STUPID.");

            getimage(x, y, x+MLEN-1, y+MHIGH-1, mousemap);
            Cursor(x, y);

            while (1)
             {
               oldx = x; 
                           oldy = y;

               button = GetMouse(&x, &y, &mvalue);

               putimage(oldx, oldy, mousemap, COPY_PUT);
               getimage(x, y, x+MLEN-1, y+MHIGH-1, mousemap);
               Cursor(x, y);

               if (button == -1)              /* 左击 */
                {
                  if ((x>407) && (x<427) && (y>35) && (y<55))
                 {                           
                   reset = 1;
                   ResetButton(0);
                   Cursor(x, y);
                   delay(100);
                   ResetButton(1);
            	 }

                  else if ((x>376) && (x<391)   
                           && (y>72) && (y<87))
                      {
                    DrawBoardIn(376, 72, 15, 15);
                    setcolor(0);
                    outtextxy(380, 75, "v");
                    Cursor(x, y);
                    delay(100);
                    DrawBoardOut(376, 72, 15, 15);
                    outtextxy(380, 75, "v");

                    if (level == 0)
                        {
                          level = 1;
                        }
                    else if (level == 1)
                		{
                          level = 2;
                		}
                         else
                		{
                          level = 0;
                		}

                    reset = 1;
                           }

                  else if ((x>428) && (x<448)
                           && (y>35) && (y<55))
                      {
                    putimage(x, y, mousemap, COPY_PUT);
                    HeroDialogBox(&x, &y, &mvalue);
                      }

                  else if ((x>449) && (x<469)
                           && (y>35) && (y<55))
                      {
                    putimage(x, y, mousemap, COPY_PUT);
                    AboutDialogBox(&x, &y, &mvalue);
                      }

                  else if ((x>255) && (x<272)
                           && (y>78) && (y<95))
                      {
                    putimage(x, y, mousemap, COPY_PUT);
                    SelectPlayer(level, &x, &y, &mvalue);
                      }

                  else if ((x>197) && (x<252)
                           && (y>78) && (y<93))
                      {
                    putimage(x, y, mousemap, COPY_PUT);
                    EnterName(level, &x, &y, &mvalue);
                      }

                  else if ((x>470) && (x<490)
                           && (y>35) && (y<55))
                      {
                    over = 1;
                    QuitButton(0);
                    Cursor(x, y);
                    delay(100);
                    QuitButton(1);
                    delay(300);
                      }
                }

               if ((over == 1) || (reset == 1))
                  {
                putimage(oldx, oldy, mousemap, COPY_PUT);
                break;
                  }
             }
              }

           getimage(x, y, x+MLEN-1, y+MHIGH-1, mousemap);
           Cursor(x, y);
           break;

      case  1: putimage(oldx, oldy, mousemap, COPY_PUT);  /*  右击  */
           if ((x>startx) && (x<startx+length)        /* 在雷区 */
                  && (y>starty) && (y<starty+length))
             {
               gamestart = 1;                     /* 游戏开始记时 */
               PlayGameR(startx, starty, x, y);   /* 处理方格 */
             }

           if (Win(gridnum) == 1)                 /* 判断是否赢 */
             {
               gamestart = 0;                     /* 暂停记时 */

               setcolor(RED);
               outtextxy(270, 105, "OK! YOU WIN.");

               Record(level, &x, &y, &mvalue);    /* 是否破纪录 */

               getimage(x, y, x+MLEN-1, y+MHIGH-1, mousemap);
               Cursor(x, y);

               while (1)
        	{
              oldx = x;
              oldy = y;

              button = GetMouse(&x, &y, &mvalue);

              putimage(oldx, oldy, mousemap, COPY_PUT);
              getimage(x, y, x+MLEN-1, y+MHIGH-1, mousemap);
              Cursor(x, y);

              if (button == -1)  /* 左击 */
                {
                 if ((x>470) && (x<490) && (y>35) && (y<55))
                  {
                over = 1;
                QuitButton(0);
                Cursor(x, y);
                delay(100);
                QuitButton(1);
                delay(300);
                  } 

                 else if ((x>376) && (x<391) && (y>72) && (y<87))
                    {
                      DrawBoardIn(376, 72, 15, 15);
                      setcolor(0);
                      outtextxy(380, 75, "v");
                      Cursor(x, y);
                      delay(100);
                      DrawBoardOut(376, 72, 15, 15);
                      outtextxy(380, 75, "v");

                      if (level == 0)
                	 {
                       level = 1;
                	 }
                      else if (level == 1)
                          {
                        level = 2;
                          }
                       else
                          {
                        level = 0;
                          }

                      reset = 1;
                    }

                 else if ((x>428) && (x<448) && (y>35) && (y<55))
                    {
                      putimage(x, y, mousemap, COPY_PUT);
                      HeroDialogBox(&x, &y, &mvalue);
                    }

                 else if ((x>449) && (x<469) && (y>35) && (y<55))
                    {
                      putimage(x, y, mousemap, COPY_PUT);
                      AboutDialogBox(&x, &y, &mvalue);
                    }

                 else if ((x>255) && (x<272) && (y>78) && (y<95))
                    {
                      putimage(x, y, mousemap, COPY_PUT);
                      SelectPlayer(level, &x, &y, &mvalue);
                    }

                 else if ((x>197) && (x<252) && (y>78) && (y<93))
                    {
                      putimage(x, y, mousemap, COPY_PUT);
                      EnterName(level, &x, &y, &mvalue);
                    }

                 else if ((x>407) && (x<427) && (y>35) && (y<55))
                    {
                      reset = 1;
                      ResetButton(0);
                      Cursor(x, y);
                      delay(100);
                      ResetButton(1);
                    }
               }

              if ((over == 1) || (reset == 1))
                 {
                   putimage(oldx, oldy, mousemap, COPY_PUT);
                   break;
                 }
        	}
             }

           getimage(x, y, x+MLEN-1, y+MHIGH-1, mousemap);
           Cursor(x, y);
           break;

      case  0: putimage(oldx, oldy, mousemap, COPY_PUT);
           getimage(x, y, x+MLEN-1, y+MHIGH-1, mousemap);
           Cursor(x, y);
           break;
      default: break;
    }
       if ((over == 1) || (reset == 1))
     {
       break;
     }
     }
   if (over == 1)
     {
       break;
     }
  }
 free(mousemap);
 WriteDisk();
 Quit();
}

/************************** user function **********************************/
void GrapInit(void)
{
  int gdriver = DETECT, gmode;
  registerbgidriver(EGAVGA_driver);
  initgraph(&gdriver, &gmode, "");
}

int MouseInit(int xmi, int xma, int ymi, int yma)
{
  int retcode;
  union REGS regs;
  regs.x.ax = 0;
  int86(51, &regs, &regs);
  retcode=regs.x.ax;
  if (retcode == 0)
     {
       return 0;
     }
  regs.x.ax = 7;
  regs.x.cx = xmi;
  regs.x.dx = xma;
  int86(51, &regs, &regs);
  regs.x.ax = 8;
  regs.x.cx = ymi;
  regs.x.dx = yma;
  int86(51, &regs, &regs);
  return retcode;
}

void ViewInit(int level)
{
  int i, j;
  int gridnum;             /* 行或列的格子数 */
  int x, y;                /* 雷区左上角的坐标 */
  int length;              /* 雷区的长度     */

  gridnum = LevelData(level, &x, &y, &length); /* 获取有关游戏级别的数据 */

  rectangle(0, 0, 639, 479);            /* 屏幕设置 */
  setfillstyle(SOLID_FILL, BLUE);
  bar(1, 1, 638, 478);

  DrawBoardOut(145, 30, 350, 430);      /* 画游戏面板 */
  DrawBoardIn(155, 35, 20, 20);
  DrawMine(157, 37);
  setcolor(0);
  outtextxy(182, 40, "EXPLORER");
  ResetButton(1);
  HeroButton(1);
  AboutButton(1);
  QuitButton(1);
  DrawBoardOut(148, 58, 344, 398);
  DrawBoardIn(158, 70, 322, 30);
  setcolor(0);
  outtextxy(163, 82, "PLER");
  outtextxy(275, 75, "LEVEL");
  outtextxy(275, 88, "SCORE");
  setfillstyle(SOLID_FILL, BLACK);
  bar(397, 78, 430, 93);
  bar(440, 78, 472, 93);
  DrawBoardOut(255, 78, 17, 17);
  setcolor(0);
  outtextxy(260, 82, "v");
  DrawBoardOut(376, 72, 15, 15);
  setcolor(0);
  outtextxy(380, 75, "v");

  DrawBoardIn(x-3, y-3, length+4, length+4);  /* 画雷区 */
  setcolor(BLACK);                            /* 以下为画雷区的行列线 */
  for (i = 1; i < gridnum; i++)
     {
       line(x-1+i*(GRIDLEN+1), y-1, x-1+i*(GRIDLEN+1), y+length-2);
       line(x-1, y-1+i*(GRIDLEN+1), x+length-2, y-1+i*(GRIDLEN+1));
     }

  for (i = 0; i < gridnum; i++)               /* 画雷区的各个格子 */
     {
       for (j = 0; j < gridnum; j++)
    {
      DrawBoardOut(x+i*(GRIDLEN+1), y+j*(GRIDLEN+1), GRIDLEN, GRIDLEN);
    }
     }
}

/*获取与级别有关的数据:*x,*y为雷区左上角坐标;*length为雷区长度,返回行格子数*/
int LevelData(int level, int *x, int *y, int *length)
{
  if (level == 0)
     {
       *x = 240;
       *y = 202;
       *length = 160;
       return 10;
     }

  if (level == 1)
     {
       *x = 192;
       *y = 154;
       *length = 256;
       return 16;
     }

  *x = 160;
  *y = 122;
  *length = 320;
  return 20;
}

void QuitButton(int flag) /* 退出按钮：flag==1为凸起状态；flag==0为凹进状态*/
{
  if (flag > 0)
     {
       DrawBoardOut(470, 35, 20, 20);
     }
  else
     {
       DrawBoardIn(470, 35, 20, 20);
     }
  setcolor(0);
  outtextxy(476, 41, "X");
}

void AboutButton(int flag) /* 关于按钮:同上 */
{
  if (flag > 0)
     {
       DrawBoardOut(449, 35, 20, 20);
     }
  else
     {
       DrawBoardIn(449, 35, 20, 20);
     }
  setcolor(0);
  outtextxy(451, 39, ".");
  outtextxy(455, 39, ".");
  outtextxy(459, 39, ".");
}

void AboutDialogBox(int *x, int *y, int *mvalue)  /* 关于对话框 */
{                        /* *x,*y为鼠标坐标,*mvalue为鼠标按键值 */
  int tempx = *x, tempy = *y;     /* 保留刚进入时的鼠标坐标     */
  int i, temp;                    /* temp用来保留gamestart的值  */
  int oldx, oldy;                 /* oldx, oldy保留鼠标移动前的坐标 */
  int button;                     /* 取按键值:左击或右击 */
  void *mousemap, *heromap;       
 
  temp = imagesize(0, 0, MLEN-1, MHIGH-1);
  mousemap = malloc(temp);
  temp = imagesize(200, 150, 440, 330);
  heromap = malloc(temp);

  temp = gamestart;
  gamestart = 0;

  AboutButton(0);                            /* 以下为按键的动态变化过程 */
  getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
  Cursor(*x, *y);
  delay(100);
  putimage(*x, *y, mousemap, COPY_PUT);
  AboutButton(1);

  getimage(200, 150, 440, 330, heromap);    /* 画对话框面板 */
  DrawBoardOut(200, 150, 240, 180);
  DrawBoardOut(203, 180, 234, 147);
  DrawBoardIn(218, 190, 203, 102);
  DrawBoardOut(300, 300, 50, 20);

  DrawBoardOut(410, 156, 20, 20);          /* 画退出按钮 */
  setcolor(0);
  outtextxy(416, 162, "X");

  outtextxy(210, 162, "ABOUT");            /* 写信息 */
  outtextxy(310, 306, " OK ");
  outtextxy(270, 210, "Explore");
  outtextxy(270, 230, "Verson: 1.0");
  outtextxy(225, 260, "Programming: Liao Jinhai");
  DrawBoardIn(232, 205, 22, 22);
  setfillstyle(SOLID_FILL, RED);
  bar(234, 207, 251, 224);
  DrawMine(235, 208);

  getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
  Cursor(*x, *y);

  while (1)
     {
       oldx = *x;
       oldy = *y;

       button = GetMouse(x, y, mvalue);

       putimage(oldx, oldy, mousemap, COPY_PUT);
       getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
       Cursor(*x, *y);

       if (button == -1)                                       /* 左击 */
      {
        if ((*x>300) && (*x<350) && (*y>300) && (*y<320))  /* OK按钮 */
           {
         DrawBoardIn(300, 300, 50, 20);
         setcolor(0);
         outtextxy(310, 306, "OK");
         Cursor(*x, *y);
         delay(100);
         DrawBoardOut(300, 300, 50, 20);
         setcolor(0);
         outtextxy(310, 306, "OK");
         Cursor(*x, *y);
         break;
           }

        else if ((*x>410) && (*x<430) && (*y>156) && (*y<176))/*退出按钮*/
            {
              DrawBoardIn(410, 156, 20, 20);
              setcolor(0);
              outtextxy(416, 162, "X");
              Cursor(*x, *y);
              delay(100);
              DrawBoardOut(410, 156, 20, 20);
              setcolor(0);
              outtextxy(416, 162, "X");
              Cursor(*x, *y);
              break;
            }
      }
     }

  putimage(200, 150, heromap, COPY_PUT);

  free(mousemap);
  free(heromap); 

  gamestart = temp;  /* 值还原 */
  *x = tempx;
  *y = tempy;
  delay(300);
}

void HeroButton(int flag)    /* 英雄按钮:flag=1为凸起状态;flag=0为凹进状态 */
{
  if (flag > 0)
     {
       DrawBoardOut(428, 35, 20, 20);
     }
  else
     {
       DrawBoardIn(428, 35, 20, 20);
     }

  setcolor(0);
  outtextxy(435, 40, "I");
  outtextxy(435, 44, ".");
}

void HeroDialogBox(int *x, int *y, int *mvalue)       /* 扫雷英雄对话框 */
{                                /* *x,*y为鼠标坐标,*mvalue为鼠标按键值 */     
  int tempx = *x, tempy = *y;
  int i, temp;
  int oldx, oldy;
  int button;
  char score[7] = "secsec";
  char lev[3][7] = {"easy", "normal", "hard"};
  void *mousemap, *heromap;

  temp = imagesize(0, 0, MLEN-1, MHIGH-1);
  mousemap = malloc(temp);
  temp = imagesize(200, 150, 440, 330);
  heromap = malloc(temp);

  temp = gamestart;
  gamestart = 0;

  HeroButton(0);   /* 按钮动态变化过程 */
  getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
  Cursor(*x, *y);
  delay(100);
  putimage(*x, *y, mousemap, COPY_PUT);
  HeroButton(1);

  getimage(200, 150, 440, 330, heromap);    /* 画面板 */
  DrawBoardOut(200, 150, 240, 180);
  DrawBoardOut(203, 180, 234, 147);
  DrawBoardIn(218, 190, 203, 102);
  DrawBoardOut(300, 300, 50, 20);

  DrawBoardOut(410, 156, 20, 20);          /* 画退出按钮 */
  setcolor(0);
  outtextxy(416, 162, "X");

  setcolor(0);                             /* 写信息 */
  outtextxy(210, 162, "HERO");
  outtextxy(310, 306, " OK ");
  outtextxy(228, 200, "LEVEL");
  outtextxy(300, 200, "NAME");
  outtextxy(364, 200, "SCORE");

  setcolor(RED);
  for (i = 0; i < 3; i++)
     {
       outtextxy(228, i*20+225, lev[i]);
       outtextxy(295, i*20+225, hero[i].name);
       score[0] = (hero[i].time / 100) + 48;
       score[1] = ((hero[i].time % 100) / 10) + 48;
       score[2] = (hero[i].time % 10) + 48;
       outtextxy(364, i*20+225, score);
     }

  getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
  Cursor(*x, *y);

  while (1)
     {
       oldx = *x;
       oldy = *y;

       button = GetMouse(x, y, mvalue);

       putimage(oldx, oldy, mousemap, COPY_PUT);
       getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
       Cursor(*x, *y);

       if (button == -1)
      {
        if ((*x>300) && (*x<350) && (*y>300) && (*y<320))  /* OK按钮 */
           {
         DrawBoardIn(300, 300, 50, 20);
         setcolor(0);
         outtextxy(310, 306, "OK");
         Cursor(*x, *y);
         delay(100);
         DrawBoardOut(300, 300, 50, 20);
         setcolor(0);
         outtextxy(310, 306, "OK");
         Cursor(*x, *y);
         break;
           }

        else if ((*x>410) && (*x<430) && (*y>156) && (*y<176))/*退出按钮*/
            {
              DrawBoardIn(410, 156, 20, 20);
              setcolor(0);
              outtextxy(416, 162, "X");
              Cursor(*x, *y);
              delay(100);
              DrawBoardOut(410, 156, 20, 20);
              setcolor(0);
              outtextxy(416, 162, "X");
              Cursor(*x, *y);
              break;
            }
      }
     }

  putimage(200, 150, heromap, COPY_PUT);

  free(mousemap);
  free(heromap);

  gamestart = temp; 
  *x = tempx;
  *y = tempy;

  delay(300);
}

void ResetButton(int flag)
{
  if (flag > 0)
     {
       DrawBoardOut(407, 35, 20, 20);
     }
  else
     {
       DrawBoardIn(407, 35, 20, 20);
     }
  setcolor(0);
  outtextxy(412, 41, "-");
  outtextxy(415, 41, ">");
}

void DrawRedFlag(int x, int y)  /* 画红旗:x, y为其坐标 */
{
  setfillstyle(SOLID_FILL, RED);
  bar(x+4, y+4, x+9, y+7);
  setcolor(RED);
  line(x+9, y+7, x+9, y+11);
}

void DrawMine(int x, int y)    /* 画雷:x, y为其坐标 */
{
  setcolor(0);
  circle(x+7, y+7, 3);
  setcolor(0);
  line(x+7, y+2, x+7, y+12);
  line(x+2, y+7, x+12, y+7);
}

void DrawFalse(int x, int y)   /* 画 X :x, y为其坐标 */
{
  setcolor(0);
  line(x+3, y+3, x+11, y+11);
  line(x+11, y+3, x+3, y+11);
}

void PlayerInit(void)
{
  int i, j;
  int m = 0, n = 0;
  FILE *fp;

  strcpy(player.name, "noname");            /* 当前玩家缺省为noname */
  player.time[0] = player.time[1] = player.time[2] = 999;

  if ((fp = fopen("c:\\windows\\profile.epl", "rb")) != NULL)/*读已存的玩家*/
     {
       m = fread(hero, sizeof(hero[0]), 3, fp);
       n = fread(players, sizeof(player), 10, fp);

       for (i = 0; i < 3; i++)       /* 以下为验证读入数据的正确性和完整性 */
      {
        hero[i].name[6] = 0;
        if ((hero[i].time <= 0) || (hero[i].time > 999))
           {
         hero[i].time = 999;
           }
      }
       for (i = 0; i < 10; i++)
      {
        players[i].name[6] = 0;
        for (j = 0; j < 3; j++)
           {
         if ((players[i].time[j] <= 0) || (players[i].time[j] > 999))
            {
              players[i].time[j] = 999;
            }
           }
      }
       fclose(fp);
     }

  if ((m != 3) || (n != 10))   /* 读入错误, 就重写 */
     {
       for (i = 0; i < 3; i++)
     {
       strcpy(hero[i].name,"noname");
       hero[i].time = 999;
     }
    for (i = 0; i < 10; i++)
     {
       players[i] = player;
     }
     }
}

void ViewPlayer(int level)           /* 显示玩家 */
{
  char lev[3][7] = {" easy", "normal", " hard"};
  char tm[7] = "999sec";

  tm[0] = (player.time[level] / 100) + 48;          /* 成绩转化为字符串 */
  tm[1] = ((player.time[level] % 100) /10) +48;
  tm[2] = (player.time[level] % 10) +48;

  setfillstyle(SOLID_FILL, WHITE);
  bar(197, 78, 252, 93);
  bar(318, 73, 373, 83);
  bar(318, 86, 373, 96);
  setcolor(0);
  outtextxy(201, 82, player.name);
  outtextxy(322, 74, lev[level]);
  outtextxy(322, 88, tm);
}

void EnterName(int level, int *x, int *y, int *mvalue)
{
  int quit = 0;
  int key;        /* 用 key 判断是键盘按下还是鼠标按下 */
  int i = 0;
  int tempx = *x, tempy = *y;
  int button;
  int oldx, oldy;
  char nam[7] = {0, 0, 0, 0, 0, 0, 0};
  void *mousemap;

  key = imagesize(0, 0, MLEN-1, MHIGH-1);   /* key暂时用一下 */
  mousemap = malloc(key);

  setfillstyle(SOLID_FILL, WHITE);
  bar(197, 78, 252, 93);

  getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
  Cursor(*x, *y);

  while (1)
    {
      oldx = *x;
      oldy = *y;
      button = Get_KorM(i, x, y, mvalue, &key);
      switch (button)
     {
       case  2: if (((key>='0')&&(key<='9')) || ((key>='a')&&(key<='z')) || ((key>='A')&&(key<='Z')))
               {                   /* 取键盘值:a-z,A-Z,0-9 */    
             if (i < 6)        /* 超过6个就不读取 */
                {
                  putimage(oldx, oldy, mousemap, COPY_PUT);
                  nam[i++] = key;
                  setfillstyle(SOLID_FILL, WHITE);
                  bar(197, 78, 252, 93);
                  setcolor(0);
                  outtextxy(201, 82, nam);
                  getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
                  Cursor(*x, *y);
                }
               }

            else if ((key == 8) && (i > 0))    /* 删除,i>0指已读入了字符 */
                {
                  nam[--i] = 0;
                  setfillstyle(SOLID_FILL, WHITE);
                  bar(197, 78, 252, 93);
                  setcolor(0);
                  outtextxy(201, 82, nam);
                }

            else if (key == 13)              /* 回车确认 */
                {
                  quit = 1;
                }

            else if (key == 27)              /* ESC取消 */
                {
                  nam[0] = 0;            /* 清空输入的字符串 */
                  quit = 1;
                }

            break;

       case  0: putimage(oldx, oldy, mousemap, COPY_PUT);  /* 鼠标移动 */
            getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
            Cursor(*x, *y);
            break;

       case -1: putimage(oldx, oldy, mousemap, COPY_PUT); /* 左击确认 */
            getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
            Cursor(*x, *y);
            quit = 1;                                 /* 退出 */
            break;

       case  1: putimage(oldx, oldy, mousemap, COPY_PUT); /* 右击取消 */
            getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
            Cursor(*x, *y);
            nam[0] = 0;                               /* 清空 */
            quit = 1;                                 /* 退出 */
            break;

       default: break;
     }
      if (quit == 1)
     {
       break;
     }
    }

  if (nam[0] == 0)  /* 无输入字符, 还原原来的玩家名 */
     {
       putimage(*x, *y, mousemap, COPY_PUT);
       setfillstyle(SOLID_FILL, WHITE);
       bar(197, 78, 252, 93);
       setcolor(0);
       outtextxy(201, 82, player.name);
       getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
     }

  else                                      /* 储存玩家名,切换到当前玩家 */
     {
       strcpy(player.name, nam);
       for (i = 0; i < 10; i++)
      {
        if (strcmp(players[i].name, nam) == 0) break;
      }

       if (i == 10)                         /* 无同名 */
     { for (i = 9; i > 0; i--)          /* 去掉最后一个 */
          {
        players[i] = players[i-1];
          }
                        /* 新名放入0号位置 */
       player.time[0] = player.time[1] = player.time[2] = 999;
       players[0] = player;
     }

       else                                 /* 同名视为查找 */
      {
        player = players[i];
      }

       ViewPlayer(level);
     }

  putimage(*x, *y, mousemap, COPY_PUT);

  free(mousemap);

  *x = tempx;
  *y = tempy;
  delay(300);
}

/* 获取键盘或鼠标的键值:返回0,鼠标移动;返回-1,左击;返回1:右击; */
/* 返回2,键盘有键按下, 键盘值由*key带回; i指出画光标的位置     */
int Get_KorM(int i, int *mx, int *my, int *mvalue, int *key)
{
  union REGS regs;
  int x, y, button;
  static unsigned int cursor = 0, colorflag = 0;

  do
   {
    if (++cursor == 50000)    /* 画光标 */
    {
        cursor = 0;
        setcolor(colorflag == 0? 0:WHITE);
        line(i*8+199, 90, i*8+204, 90);
        line(i*8+199, 91, i*8+204, 91);
        colorflag = !colorflag;
    }

     if (kbhit())                   /* 键盘有键按下,返回 */
    {
      *key = getch();
      return 2;
    }

     regs.x.ax = 3;
     int86(51, &regs, &regs);
     x = regs.x.cx;
     y = regs.x.dx;
     button = regs.x.bx;

   }while ((x==*mx) && (y==*my) && (button==*mvalue));

  *mx = x;
  *my = y;
  *mvalue = button;

  if (button & 1)         /* 左键按下 */
     {
       return -1;
     }

  button = button >> 1;   /* 右键按下 */
  if (button & 1)    
     {
       return 1;
     }

  return 0;               /* 其它情况 */
}

void SelectPlayer(int level, int *x, int *y, int *mvalue) /* 玩家选择 */
{
  int i;
  int num, no;                  /* num为储存的玩家个数,no为第几个玩家 */
  int flag = 0;                 /* 鼠标移到过玩家面板flag=1,未flag=0  */
  int tempx = *x, tempy = *y;
  int oldx, oldy;
  int button;
  int temp, quit = 0;
  void *mousemap, *playermap;

  temp = imagesize(0, 0, MLEN-1, MHIGH-1);
  mousemap = malloc(temp);

  DrawBoardIn(255, 78, 17, 17);         /* 选择玩家按钮的动态变化过程 */
  setcolor(0);
  outtextxy(260, 82, "v");
  getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
  Cursor(*x, *y);
  delay(100);
  putimage(*x, *y, mousemap, COPY_PUT);
  DrawBoardOut(255, 78, 17, 17);
  setcolor(0);
  outtextxy(260, 82, "v");

  for (num = 0; num < 10; num++)
     {
       if (strcmp(players[num].name, "noname") == 0)
      {
        break;
      }
     }
  if (num == 0)                         /* 没有储存玩家,返回 */
     {
       return;
     }

  temp = imagesize(173, 95, 278, num*20+95+10+MHIGH);
  playermap = malloc(temp);

  temp = gamestart;                     /* 保留gamestart原值*/
  gamestart = 0;                        /* 记时停止 */

  getimage(173, 95, 278, num*20+95+10+MHIGH, playermap);/* 画面板 */
  DrawBoardOut(173, 95, 105, num*20+10);

  for (i = 0; i < num; i++)               /* 在面板上出示玩家姓名 */
     {
       setcolor(0);
       outtextxy(184, i*20+106, "<");
       outtextxy(188, i*20+106,"-");
       outtextxy(201, i*20+106, players[i].name);
       outtextxy(255, i*20+106, "-");
       outtextxy(259, i*20+106, ">");
     }

  getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
  Cursor(*x, *y);

  while (1)
    {
      oldx = *x;
      oldy = *y;
      button = GetMouse(x, y, mvalue);
      switch (button)
    {
      case -1: putimage(oldx, oldy, mousemap, COPY_PUT);
           if ((*x>178) && (*x<273) && (*y>100) && (*y<num*20+100))
             {              /* 左击,如在玩家面板上,选玩家,退出*/
               DrawBoardIn(178, no*20+100, 95, 20);   /* 按钮 */
               setcolor(0);
               outtextxy(184, no*20+106, "<");
               outtextxy(188, no*20+106,"-");
               outtextxy(201, no*20+106, players[no].name);
               outtextxy(255, no*20+106, "-");
               outtextxy(259, no*20+106, ">");
               Cursor(*x, *y);
               delay(100);
               DrawBoardOut(178, no*20+100, 95, 20);
               player = players[no];
               ViewPlayer(level);
               quit = 1;                              /* 退出 */
             }

           else if ((*x<173)||(*x>278) || (*y<95)||(*y>num*20+95+10))
              {
                quit = 1;             /*其它区域左击, 退出 */
              }

           getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
           Cursor(*x, *y);
           break;

      case  1: putimage(oldx, oldy, mousemap, COPY_PUT);
           if ((*x>178) && (*x<273) && (*y>100) && (*y<num*20+100))
             {                           /* 右击, 显示玩家成绩 */
               ViewScore(no, x, y, mvalue);
             }

           else if ((*x<173) || (*x>278) || (*y<95) || (*y>num*20+95+10))
             {
               quit = 1;                 /* 其它区域右击, 退出 */
             }

           getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
           Cursor(*x, *y);
           break;

      case  0: putimage(oldx, oldy, mousemap, COPY_PUT);   /* 移动 */
           if ((*x>178) && (*x<273) && (*y>100) && (*y<num*20+100))
             {          /* 在玩家面板上移动, 一个玩家形成一个按钮 */
               no = (*y - 100) / 20;              /* 获取玩家号数 */
               DrawBoardOut(178, no*20+100, 95, 20);
               setcolor(0);
               outtextxy(184, no*20+106, "<");
               outtextxy(188, no*20+106,"-");
               outtextxy(201, no*20+106, players[no].name);
               outtextxy(255, no*20+106, "-");
               outtextxy(259, no*20+106, ">");
               flag = 1;            /* 鼠标在玩家面板上 */
             }

           getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
           Cursor(*x, *y);
           break;
    }

      if (flag == 1)          /* 鼠标在玩家面板上 */
    {                     /* 本程序段的功能：把原来呈按钮状玩家画平 */
      for (i = 0; i < num; i++)          
       {                          
         if (i != no)     /* 跳过鼠标指向的玩家, 不画平 */
           {
         setfillstyle(SOLID_FILL, 7);
         bar(178, i*20+100, 273, (i+1)*20+100);
         setcolor(0);
         outtextxy(184, i*20+106, "<");
         outtextxy(188, i*20+106,"-");
         outtextxy(201, i*20+106, players[i].name);
         outtextxy(255, i*20+106, "-");
         outtextxy(259, i*20+106, ">");
           }
       }
      flag = 0;
      Cursor(*x, *y);
    }

      if (quit == 1)
    {
      break;
    }
    }

  putimage(*x, *y, mousemap, COPY_PUT);
  putimage(173, 95, playermap, COPY_PUT);

  free(mousemap);
  free(playermap);

  gamestart = temp;
  *x = tempx;
  *y = tempy;
  delay(300);
}

void ViewScore(int no, int *x, int *y, int *mvalue) /* 显示no号玩家的成绩 */
{
  int i;
  int oldx, oldy;
  int button;
  int tempx = *x, tempy = *y;
  char tm[7] = "secsec";
  char lev[3][7] = {"easy", "normal", "hard"};
  void *mousemap, *scoremap;

  i = imagesize(0, 0, MLEN-1, MHIGH-1);
  mousemap = malloc(i);
  i = imagesize(0, 0, 150, 70);
  scoremap = malloc(i);

  getimage(278, no*20+100, 428, no*20+170, scoremap);   /* 画面板 */
  DrawBoardOut(278, no*20+100, 150, 70);

  setcolor(0);                                          /* 写成绩 */
  for (i = 0; i < 3; i++)
     {
       outtextxy(295, i*20+no*20+110, lev[i]);
       tm[0] = (players[no].time[i] /100) +48;
       tm[1] = ((players[no].time[i] % 100) / 10) +48;
       tm[2] = (players[no].time[i] % 10) +48;
       outtextxy(360, i*20+no*20+110, tm);
     }

  getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
  Cursor(*x, *y);

  while (1)
     {
       oldx = *x;
       oldy = *y;

       button = GetMouse(x, y, mvalue);

       putimage(oldx, oldy, mousemap, COPY_PUT);
       getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
       Cursor(*x, *y);

       if (button != 0)
      {
        break;
      }
     }

  putimage(*x, *y, mousemap, COPY_PUT);
  putimage(278, no*20+100, scoremap, COPY_PUT);

  free(mousemap);
  free(scoremap);

  *x = tempx;
  *y = tempy;
  delay(300);
}

void DataInit(int level)   /* 初始化为level级别的数据 */
{
  int i, j, m, n;
  int gridnum, num;  /* gridnum为行列格子数，num为某一级别的雷数 */
  char mnum[3] = {0, 0, 0};   /* 储存雷数转化的字符串 */

  if (level == 0)     /* 根据level决定行列格子数和雷数 */
     {
       gridnum = 10;
       num = 15;
     }
  else if (level == 1)
      {
        gridnum = 16;
        num = 40;
      }
       else
      {
        gridnum = 20;
        num = 80;
      }

  gametime = -1;    /* 记时初始为-1，方便运算 */

  minenum = num;    /* 显示雷数 */
  mnum[0] = (minenum / 10) + 48;
  mnum[1] = (minenum % 10) + 48;
  setcolor(RED);
  outtextxy(406, 82, mnum);

  outtextxy(444, 82, "000");   /* 初始时间为0秒 */

  for (i = 0; i < gridnum; i++)        /* 方格的初始各值 */
      {
    for (j = 0; j < gridnum; j++)
        {
           board[i][j].status = 1;
           board[i][j].mine = board[i][j].check = 0;
        }
      }

  randomize();                      /* 随机布雷 */
  for (m = 0; m < num;)
      {
    i = random(gridnum);
    j = random(gridnum);
    if (board[i][j].mine != -1)
       {
         board[i][j].mine = -1;
         m++;
       }
      }

  for (i = 0; i < gridnum; i++)        /* 无雷格记录周围的八个格子的雷数 */
      {
    for (j = 0; j < gridnum; j++)
        {
          if (board[i][j].mine != -1)
         {
           for (m = i-1; m <= i+1; m++)
               {
             if ((m < 0) || (m >= gridnum)) continue;
             for (n = j-1; n <= j+1; n++)
                 {
                   if ((n < 0) || (n >= gridnum)) continue;
                   if (board[m][n].mine == -1) board[i][j].mine++;
                 }
               }
         }
        }
      }
}

/* 左击雷区进行的处理 */
int PlayGameL(int startx, int starty, int x, int y, int gridnum)
{
  char num[2] = {0,0};
  int i, j;

  i = (x - startx) / (GRIDLEN+1); /* 鼠标指向格子的行列下标值 */
  j = (y - starty) / (GRIDLEN+1);

  if (board[i][j].status == 1)              /* 指向初始状态的格子 */
     {
       if (board[i][j].mine != -1)          /* 指向无雷格 */
      {
        if (board[i][j].mine != 0) /*周围八格有若干个雷的情况,打开本身*/
           {
         board[i][j].status = 2;
         board[i][j].check = 1;
         DrawBoardIn(i*(GRIDLEN+1)+startx, j*(GRIDLEN+1)+starty, GRIDLEN, GRIDLEN);
         num[0] = board[i][j].mine+48;
         setcolor(0);
         outtextxy(i*(GRIDLEN+1)+startx+4, j*(GRIDLEN+1)+starty+4, num);
           }

        else                    /* 周围八格无雷，打开本身和周围八个格 */
           {                    /* 递归 */
         OpenGrid(startx, starty, i, j, gridnum);
           } 

        return 1;
      }

       else                        /* 左击错误，标记有雷，返回0 */
      {
        board[i][j].status = 0;
        board[i][j].check = 0;
        DrawBoardIn(i*(GRIDLEN+1)+startx, j*(GRIDLEN+1)+starty, GRIDLEN, GRIDLEN);
        setfillstyle(SOLID_FILL, RED);
        bar(i*(GRIDLEN+1)+startx+2, j*(GRIDLEN+1)+starty+2, i*(GRIDLEN+1)+startx+12, j*(GRIDLEN+1)+starty+12);
        DrawMine(i*(GRIDLEN+1)+startx, j*(GRIDLEN+1)+starty);
        return 0;
      }
     }

   else        /* 其它格 */
      {
    return 1;
      }
}

void PlayGameR(int startx, int starty, int x, int y)   /* 右击方格的处理 */
{
  int i, j;

  i = (x - startx) / (GRIDLEN+1);  /* 鼠标指向方格的坐标 */
  j = (y - starty) / (GRIDLEN+1);

  if (board[i][j].status == 1)     /* 标雷 */
     {
       board[i][j].status = 3;
       if (board[i][j].mine == -1)
      {
        board[i][j].check = 1;
      }
       DrawRedFlag(i*(GRIDLEN+1)+startx, j*(GRIDLEN+1)+starty);
       CountMine(-1);
       return;
     }

  if (board[i][j].status == 3)    /* 标？：表示此格有疑问 */
     {
       board[i][j].status = 4;
       board[i][j].check = 0;
       DrawBoardOut(i*(GRIDLEN+1)+startx, j*(GRIDLEN+1)+starty, GRIDLEN, GRIDLEN);
       setcolor(RED);
       outtextxy(i*(GRIDLEN+1)+startx+4, j*(GRIDLEN+1)+starty+4, "?");
       CountMine(1);
       return;
     }

  if (board[i][j].status == 4)    /* 解除？号 */
     {
       board[i][j].status = 1;
       DrawBoardOut(i*(GRIDLEN+1)+startx, j*(GRIDLEN+1)+starty, GRIDLEN, GRIDLEN);
       return;
     }
}

void CountMine(int flag)/* 运算剩余雷数：flag=-1标了一个雷,flag=1标了一问号 */
{
  int temp;
  char mnum[3] = {0, 0, 0};

  if (flag == 1)
     {
       minenum++;
     }
  else if (flag == -1)
     {
       minenum--;
     }

  if (minenum < 0)   /* 标的雷比实际雷多的情况 */
     {
       temp = 0;
     }
  else
     {
       temp = minenum;
     }

  mnum[0] = (temp / 10) + 48;   /* 显示雷数 */
  mnum[1] = (temp % 10) + 48;
  setfillstyle(SOLID_FILL, BLACK);
  bar(397, 78, 430, 93);
  setcolor(RED);
  outtextxy(406, 82, mnum);
}

void CountTime(int x, int y)    /* 记时 */
{
  char tm[4] = {0, 0, 0, 0};
  struct time curtime;

  gettime(&curtime);

  if (gametime < 0)            /* 如果还没开始，就开始 */
     {
       gametime++;
       oldtime = curtime;
       return;
     }

  if (oldtime.ti_sec != curtime.ti_sec)   /* 按秒记时 */
     {
       oldtime = curtime;
       gametime++;
       tm[0] = (gametime / 100) + 48;
       tm[1] = (gametime % 100) / 10 +48;
       tm[2] = (gametime % 10) +48;
       setfillstyle(SOLID_FILL, BLACK);
       bar(440, 78, 472, 93);
       setcolor(RED);
       outtextxy(444, 82, tm);

       if ((x>440-MLEN) && (x<=472) && (y>78-MHIGH) && (y<=93))
      {                               /* 鼠标在记时表上的情况 */
        Cursor(x, y);
      }
     }
}

/* 如果左击方格的周围八个方格无雷，则全打开，递归。i,j为当前格的坐标*/
void OpenGrid(int startx, int starty, int i, int j, int gridnum)
{
  char num[2] = {0,0};
  int x, y;

  if (board[i][j].mine != 0)  /* 左击的方格值不为0，则返回 */
     {
       return;
     }

  for (x = i-1; x <= i+1; x++) /* 打开周围八格 */
     {
       if ((x < 0) || (x >= gridnum))   /* x下标越界返回 */
           {
             continue;
           }

       for (y = j-1; y <= j+1; y++)
      {
        if ((y < 0) || (y >= gridnum))  /* y下标越界返回 */
                 {
                    continue;
                 }

        if (board[x][y].mine != 0)          /* 周围八格有雷的情况 */
           {
         if (board[x][y].status == 3)   /* 如标雷则错误，雷数加1 */
            {
              minenum++;
            }
         board[x][y].status = 2;
         board[x][y].check = 1;
         DrawBoardIn(x*(GRIDLEN+1)+startx, y*(GRIDLEN+1)+starty, GRIDLEN, GRIDLEN);
         num[0] = board[x][y].mine+48;
         setcolor(0);
         outtextxy(x*(GRIDLEN+1)+startx+4, y*(GRIDLEN+1)+starty+4, num);
           }

        else if (board[x][y].status != 2)/*周围八格无雷的情况，且没打开*/
           {
         if (board[x][y].status == 3)/* 如标雷则错误，雷数加1 */
            {
              minenum++;
            }
         board[x][y].status = 2;
         board[x][y].check = 1;
         DrawBoardIn(x*(GRIDLEN+1)+startx, y*(GRIDLEN+1)+starty, GRIDLEN, GRIDLEN);
         OpenGrid(startx, starty, x, y, gridnum);     /* 递归调用 */
           }
      }
     }
  CountMine(0);
}

int Win(int gridnum)   /* 检测一下是否赢了 */
{
  int i, j;

  for (i = 0; i < gridnum; i++)
     {
       for (j = 0; j < gridnum; j++)
      {
        if (board[i][j].check == 0) return 0;
      }
     }

  return 1;
}

void Record(int level, int *x, int *y, int *mvalue)  /* 记录成绩 */
{
  int i;
  int tempx = *x, tempy = *y;
  int oldx, oldy;
  int button;
  char tm[7] = "secsec";
  void *mousemap, *okmap;

  i = imagesize(0, 0, MLEN-1, MHIGH-1);
  mousemap = malloc(i);
  i = imagesize(200, 150, 440, 330);
  okmap = malloc(i);

  if (player.time[level] > gametime)    /* 记录最少的秒数 */
     {
       player.time[level] = gametime;
       for (i = 0; i < 10; i++)
      {
        if (strcmp(player.name, players[i].name) == 0)
           {
         players[i].time[level] = gametime;
         break;
           }
      }
       ViewPlayer(level);
     }

  if (hero[level].time > gametime)               /* 破纪录 */
     {
       hero[level].time = gametime;              /* 更新纪录 */
       strcpy(hero[level].name, player.name);

       tm[0] = (gametime / 100) + 48;            /* 以下为提示玩家破了纪录 */
       tm[1] = ((gametime % 100) / 10) + 48;
       tm[2] = (gametime % 10) + 48;
       getimage(200, 150, 440, 330, okmap);
       DrawBoardOut(200, 150, 240, 180);
       DrawBoardOut(203, 180, 234, 147);
       DrawBoardIn(218, 190, 203, 102);
       DrawBoardOut(300, 300, 50, 20);

       DrawBoardOut(410, 156, 20, 20);
       setcolor(0);
       outtextxy(416, 162, "X");
       outtextxy(210, 162, "RECORD");
       outtextxy(310, 306, " OK ");
       setcolor(RED);
       outtextxy(270, 210, "Ok! You broke the");
       outtextxy(230, 230, "record.");
       outtextxy(230, 260, "Your score is ");
       outtextxy(340, 260, tm);
       outtextxy(390, 260, "!");

       getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
       Cursor(*x, *y);

       while (1)
     {
       oldx = *x;
       oldy = *y;

       button = GetMouse(x, y, mvalue);

       putimage(oldx, oldy, mousemap, COPY_PUT);
       getimage(*x, *y, *x+MLEN-1, *y+MHIGH-1, mousemap);
       Cursor(*x, *y);

       if (button == -1)
          {
        if ((*x>300) && (*x<350) && (*y>300) && (*y<320))
          {
            DrawBoardIn(300, 300, 50, 20);
            setcolor(0);
            outtextxy(310, 306, "OK");
            Cursor(*x, *y);
            delay(100);
            DrawBoardOut(300, 300, 50, 20);
            setcolor(0);
            outtextxy(310, 306, "OK");
            Cursor(*x, *y);
            break;
          }

        else if ((*x>410) && (*x<430) && (*y>156) && (*y<176))
               {
             DrawBoardIn(410, 156, 20, 20);
             setcolor(0);
             outtextxy(416, 162, "X");
             Cursor(*x, *y);
             delay(100);
             DrawBoardOut(410, 156, 20, 20);
             setcolor(0);
             outtextxy(416, 162, "X");
             Cursor(*x, *y);
             break;
               }
          }
     }

      putimage(200, 150, okmap, COPY_PUT);
      delay(300);
     }

  free(mousemap);
  free(okmap);
  *x = tempx;
  *y = tempy;
}

void GameOver(int startx, int starty, int gridnum)  /* 游戏失败的处理 */
{
  int i, j;

  for (i = 0; i < gridnum; i++)
     {
       for (j = 0; j < gridnum; j++)
       {
         if ((board[i][j].mine == -1) && (board[i][j].status == 1))
            {                              /* 未标示为雷的方格显示雷 */
                  DrawBoardIn(i*(GRIDLEN+1)+startx,
                              j*(GRIDLEN+1)+starty, GRIDLEN, GRIDLEN);

          DrawMine(i*(GRIDLEN+1)+startx, j*(GRIDLEN+1)+starty);
        }

         else if ((board[i][j].mine != -1) && (board[i][j].status == 3))
             {                         /* 标错了的画 X */
               DrawFalse(i*(GRIDLEN+1)+startx, j*(GRIDLEN+1)+starty);
             }
      }
     }
}

int GetMouse(int *mx, int *my, int *mvalue)    /* 获取鼠标按键值 */
{
  union REGS regs;
  int x, y, button;

  do
   {
     if ((gamestart == 1) && (gametime < 999))   /* 记时 */
      {
        CountTime(x, y);
      }

     regs.x.ax = 3;
     int86(51, &regs, &regs);
     x = regs.x.cx;
     y = regs.x.dx;
     button = regs.x.bx;
   }while ((x==*mx) && (y==*my) && (button==*mvalue));

  *mx = x;
  *my = y;
  *mvalue = button;
  if (button & 1)     /* 左键按下 */
     {
       return -1;
     }

  button = button >> 1;
  if (button & 1)    /* 右键按下 */
     {
       return 1;
     }

  return 0;          /* 其它情况 */
}

void Cursor(int mx, int my)  /* 画鼠标 */
{
  int i, j;
  for (i = 0; i < MHIGH; i++)
     {
       for (j = 0; j < MLEN; j++)
      {
        if (mouse[i][j] == 1)
           {
         putpixel(mx+j, my+i, WHITE);
           }
        else if (mouse[i][j] == 2)
            {
               putpixel(mx+j, my+i, BLACK);
            }
      }
     }
}

void DrawBoardOut(int x, int y, int sizex, int sizey)   /* 画凸出的按钮 */
{               /* x, y为左上角坐标;sizex,sizey为长和宽 */
 int sizx = sizex-1;
 int sizy = sizey-1;

 setcolor(15);                          /* 白色 */
 line(x, y, x+sizx-1, y);
 line(x, y+1, x+sizx-2, y+1);
 line(x, y, x, y+sizy-1);
 line(x+1, y, x+1, y+sizy-2);

 setcolor(8);                           /* 深灰色 */
 line(x+1, y+sizy, x+sizx, y+sizy);
 line(x+2, y+sizy-1, x+sizx, y+sizy-1);
 line(x+sizx-1, y+1, x+sizx-1, y+sizy);
 line(x+sizx, y+2, x+sizx, y+sizy);

 setcolor(7);                          /* 灰色 */
 putpixel(x, y+sizy, 3);
 putpixel(x+1, y+sizy-1, 3);
 putpixel(x+sizx, y, 3);
 putpixel(x+sizx-1, y+1, 3);

 setfillstyle(1, 7);                   /* 灰色,并设置填充模式*/
 bar(x+2, y+2, x+sizx-2, y+sizy-2);
}

void DrawBoardIn(int x, int y, int sizex, int sizey)  /* 画凹进的按键 */
{                      /* x, y为左上角坐标;sizex,sizey为长和宽*/
 int sizx = sizex-1;
 int sizy = sizey-1;

 setcolor(8);                          /* 白色 */
 line(x, y, x+sizx-1, y);
 line(x, y+1, x+sizx-2, y+1);
 line(x, y, x, y+sizy-1);
 line(x+1, y, x+1, y+sizy-2);

 setcolor(15);                         /* 深灰色 */
 line(x+1, y+sizy, x+sizx, y+sizy);
 line(x+2, y+sizy-1, x+sizx, y+sizy-1);
 line(x+sizx-1, y+1, x+sizx-1, y+sizy);
 line(x+sizx, y+2, x+sizx, y+sizy);

 setcolor(7);                         /* 灰色 */
 putpixel(x, y+sizy, 3);
 putpixel(x+1, y+sizy-1, 3);
 putpixel(x+sizx, y, 3);
 putpixel(x+sizx-1, y+1, 3);

 setfillstyle(1, 7);                  /*灰色,并设置填充模式*/
 bar(x+2, y+2, x+sizx-2, y+sizy-2);
}

void WriteDisk(void)        /* 游戏结束，数据写盘 */
{
  int i;
  FILE *fp;

  for (i = 0; i < 10; i++)
     {
       if (strcmp(players[i].name, "noname") == 0)
      {
        players[i].time[0] = players[i].time[1]
                   = players[i].time[2] = 999;
      }
     }

  if ((fp = fopen("c:\\windows\\profile.epl", "wb")) != NULL)
     {
       fwrite(hero, sizeof(struct HERO), 3, fp);
       fwrite(players, sizeof(struct user), 10, fp);
       fclose(fp);
     }
}

void Quit(void)
{
  cleardevice();
  closegraph();
}
