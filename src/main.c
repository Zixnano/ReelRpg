#include <gba.h>
#include <stdint.h>
#include <stdbool.h>

#define RGB15C(r,g,b) ((u16)((r)|((g)<<5)|((b)<<10)))
#define W 240
#define H 160
#define VRAM ((volatile u16*)0x06000000)
#define INK RGB15C(4,8,14)
#define CREAM RGB15C(30,29,23)
#define GREEN RGB15C(9,19,13)
#define MINT RGB15C(13,25,18)
#define WATER RGB15C(7,18,25)
#define BLUE RGB15C(9,22,29)
#define GOLD RGB15C(31,24,10)
#define ORANGE RGB15C(31,13,5)
#define WHITE RGB15C(31,31,29)
#define GREY RGB15C(16,17,16)

static u16 keys, oldKeys;
static int scene=0, animal=0, px=112, py=92, coins=12, catches=0, tension=40, progress=0, waitTicks=0;
static bool hooked=false;
static const char *animals[]={"OTTER","CAT","FOX","CAPYBARA"};
static const u8 font[37][5]={
{7,5,5,5,7},{2,6,2,2,7},{7,1,7,4,7},{7,1,7,1,7},{5,5,7,1,1},{7,4,7,1,7},{7,4,7,5,7},{7,1,1,1,1},{7,5,7,5,7},{7,5,7,1,7},{2,5,7,5,5},{6,5,6,5,6},{3,4,4,4,3},{6,5,5,5,6},{7,4,6,4,7},{7,4,6,4,4},{3,4,5,5,3},{5,5,7,5,5},{7,2,2,2,7},{1,1,1,5,2},{5,5,6,5,5},{4,4,4,4,7},{5,7,7,5,5},{5,7,7,7,5},{2,5,5,5,2},{6,5,6,4,4},{2,5,5,7,3},{6,5,6,5,5},{3,4,2,1,6},{7,2,2,2,2},{5,5,5,5,7},{5,5,5,5,2},{5,5,7,7,5},{5,5,2,5,5},{5,5,2,2,2},{7,1,2,4,7},{0,0,0,0,0}};
static const char alphabet[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
static bool hit(u16 k){return (keys&k)&&!(oldKeys&k);}
static void rect(int x,int y,int w,int h,u16 c){
 if(x<0){w+=x;x=0;} if(y<0){h+=y;y=0;} if(x+w>W)w=W-x; if(y+h>H)h=H-y;
 if(w<=0||h<=0)return;
 for(int j=0;j<h;j++)for(int i=0;i<w;i++)VRAM[(y+j)*W+x+i]=c;
}
static void clearScreen(u16 c){for(int i=0;i<W*H;i++)VRAM[i]=c;}
static void txt(int x,int y,const char*s,u16 c,int scale){
 while(*s){char ch=*s++;if(ch>='a'&&ch<='z')ch-=32;int n=0;while(alphabet[n]&&alphabet[n]!=ch)n++;if(n>36)n=36;
 for(int yy=0;yy<5;yy++)for(int xx=0;xx<3;xx++)if(font[n][yy]&(1<<(2-xx)))rect(x+xx*scale,y+yy*scale,scale,scale,c);x+=4*scale;}
}
static void panel(int x,int y,int w,int h){rect(x,y,w,h,INK);rect(x+2,y+2,w-4,h-4,CREAM);}
static void fish(int x,int y,u16 c){rect(x+3,y,9,3,c);rect(x,y+3,14,6,c);rect(x+3,y+9,8,2,c);rect(x+13,y+3,5,6,GOLD);rect(x+4,y+4,2,2,INK);}
static void hero(int x,int y){u16 fur=animal==0?RGB15C(20,13,7):animal==1?RGB15C(22,22,20):animal==2?ORANGE:RGB15C(19,14,9);rect(x+3,y,6,3,fur);rect(x+1,y+3,10,8,fur);rect(x+3,y+7,6,4,CREAM);rect(x+3,y+4,2,2,INK);rect(x+8,y+4,2,2,INK);rect(x+2,y+11,3,3,INK);rect(x+8,y+11,3,3,INK);if(animal==1||animal==2){rect(x+1,y-2,3,4,fur);rect(x+8,y-2,3,4,fur);}}
static void header(const char*s){rect(0,0,W,18,INK);txt(7,6,s,CREAM,1);txt(184,6,"REEL RPG",GOLD,1);}
static void title(void){clearScreen(GREEN);rect(0,98,W,62,WATER);for(int i=0;i<7;i++)rect(i*38,110+(i%3)*8,22,2,BLUE);panel(24,14,192,49);txt(49,25,"REEL RPG",INK,3);txt(49,44,"A TINY FISHING RPG",INK,1);hero(113,75);fish(43,91,MINT);fish(177,119,ORANGE);txt(49,143,"PRESS START",WHITE,1);}
static void selectHero(void){clearScreen(GREEN);header("CHOOSE ANGLER");txt(17,29,"PICK YOUR ANIMAL",INK,1);for(int i=0;i<4;i++){int y=44+i*25;panel(20,y,200,21);if(i==animal)rect(23,y+3,194,15,GOLD);txt(34,y+8,animals[i],INK,1);hero(181,y+4);}txt(14,150,"UP DOWN  A SELECT",WHITE,1);}
static void town(void){clearScreen(GREEN);rect(0,18,W,88,BLUE);rect(0,106,W,54,GREEN);for(int i=0;i<5;i++){rect(i*49+7,43,32,35,CREAM);rect(i*49+10,47,26,27,GREY);rect(i*49+5,39,36,5,GOLD);}rect(0,101,W,8,CREAM);hero(px,py);fish(190,70,MINT);header("BRINEBELL PIER");panel(6,22,87,16);txt(12,28,"COINS",INK,1);txt(58,28,coins<10?"0":"12",INK,1);panel(148,22,86,16);txt(154,28,"FISH",INK,1);txt(200,28,catches<10?"0":"10",INK,1);panel(8,124,224,28);txt(16,131,"A CASTS YOUR LINE",INK,1);txt(16,141,"START OPENS BAG",INK,1);}
static void fishing(void){clearScreen(WATER);header("FISHING");for(int y=29;y<91;y+=13)for(int x=y%25;x<W;x+=45)rect(x,y,19,1,BLUE);rect(0,96,W,64,GREEN);rect(0,91,W,4,CREAM);hero(23,112);fish(157,72,hooked?ORANGE:MINT);panel(36,26,168,32);txt(47,34,hooked?"BITE PRESS A":"WAIT FOR BITE",INK,1);txt(47,46,"B CANCEL",INK,1);txt(12,143,"TENSION",WHITE,1);rect(74,141,150,9,INK);rect(76,143,146,5,GREY);rect(76,143,tension*146/100,5,tension>75?ORANGE:MINT);}
static void reel(void){clearScreen(WATER);header("REEL IT IN");rect(0,20,W,67,BLUE);fish(154+(progress%38),49,MINT);panel(10,28,220,49);txt(20,36,"KEEP TENSION SAFE",INK,1);rect(20,49,200,12,INK);rect(22,51,196,8,GREY);rect(92,51,52,8,MINT);rect(22+tension*192/100,48,4,14,GOLD);txt(20,66,"A REEL  B RELEASE",INK,1);rect(0,88,W,72,GREEN);hero(28,112);fish(163,111,ORANGE);}
static void bag(void){clearScreen(GREEN);header("TRAVEL BAG");panel(18,32,204,94);txt(31,45,"REEL RPG PROTOTYPE",INK,1);txt(31,61,"FISH CAUGHT",INK,1);txt(133,61,catches<10?"0":"10",INK,1);txt(31,75,"COINS",INK,1);txt(77,75,coins<10?"0":"12",INK,1);txt(31,91,"A SAVE  B BACK",INK,1);txt(18,145,"MORE ADVENTURE PLANNED",WHITE,1);}
int main(void){
 REG_DISPCNT=MODE_3|BG2_ENABLE;
 while(1){oldKeys=keys;scanKeys();keys=keysDown();
  if(scene==0){title();if(hit(KEY_START)||hit(KEY_A))scene=1;}
  else if(scene==1){selectHero();if(hit(KEY_UP)||hit(KEY_LEFT))animal=(animal+3)%4;if(hit(KEY_DOWN)||hit(KEY_RIGHT))animal=(animal+1)%4;if(hit(KEY_A))scene=2;}
  else if(scene==2){town();if(hit(KEY_UP)&&py>44)py-=3;if(hit(KEY_DOWN)&&py<132)py+=3;if(hit(KEY_LEFT)&&px>4)px-=3;if(hit(KEY_RIGHT)&&px<220)px+=3;if(hit(KEY_A)){scene=3;hooked=false;waitTicks=0;}if(hit(KEY_START)||hit(KEY_SELECT))scene=5;}
  else if(scene==3){fishing();waitTicks++;if(waitTicks>100)hooked=true;if(hooked&&hit(KEY_A)){scene=4;progress=0;tension=45;}if(hit(KEY_B)){scene=2;hooked=false;}}
  else if(scene==4){reel();progress++;if(hit(KEY_A))tension+=8;if(hit(KEY_B))tension-=10;if(tension<0)tension=0;if(tension>100)tension=100;if(progress>90){if(tension>15&&tension<85){catches++;coins+=3;}scene=2;}if(hit(KEY_START))scene=2;}
  else{bag();if(hit(KEY_A)){volatile u8*s=(volatile u8*)0x0E000000;s[0]='R';s[1]='E';s[2]='E';s[3]='L';s[4]=(u8)animal;s[5]=(u8)coins;s[6]=(u8)catches;s[7]=0xA5;scene=2;}if(hit(KEY_B)||hit(KEY_START))scene=2;}
  VBlankIntrWait();
 }
 return 0;
}
