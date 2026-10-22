#include <gba.h>
#include <stdint.h>
#include <stdbool.h>

#define W 240
#define H 160
#define RGB15C(r,g,b) ((u16)((r)|((g)<<5)|((b)<<10)))
#define C(r,g,b) RGB15C(r,g,b)
#define PIX ((volatile u16*)0x06000000)
#define INK C(7,10,13)
#define SHADOW C(10,13,16)
#define CREAM C(30,29,23)
#define PAPER C(26,25,20)
#define WHITE C(31,31,28)
#define PALE C(27,29,23)
#define GRASS C(13,23,12)
#define GRASS2 C(17,27,14)
#define GRASS3 C(20,29,16)
#define LEAF C(10,22,12)
#define LEAF2 C(15,27,13)
#define LEAF3 C(22,29,16)
#define TRUNK C(17,11,5)
#define PATH C(25,22,18)
#define PATH2 C(29,27,22)
#define TILE C(23,20,27)
#define TILE2 C(27,24,30)
#define TILE3 C(19,17,24)
#define WATER C(7,17,24)
#define WATER2 C(9,21,27)
#define WATER3 C(13,25,29)
#define ROOF C(26,8,13)
#define ROOF2 C(31,13,17)
#define WOOD C(20,11,6)
#define GOLD C(31,23,9)
#define ORANGE C(31,12,5)
#define PINK C(31,17,20)
#define MINT C(15,27,17)
#define BLUE C(9,18,29)
#define LILAC C(23,19,29)

static u16 keys, oldKeys;
static int scene=0, animal=0, px=118, py=100, coins=24, level=1, xp=0;
static int catches=0, fishCount=0, bait=5, rods=1, island=0, day=1, stamina=100;
static int hooked=0, waitTicks=0, tension=42, progress=0, fishKind=0, fishQuality=0;
static int bagCursor=0, menuCursor=0, pet=0, petBond=12, cooked=0, battleEnemy=0, battleHp=30, heroHp=30, battleTurn=0;
static int caught[8]={0,0,0,0,0,0,0,0};
static int fishTotal[8]={0,0,0,0,0,0,0,0};
static int mapOffset=0, toastTicks=0, toastCode=0, castX=0, castY=0;
static bool hasPet=false, questDone=false, saved=false, showHelp=false;
static const char *animals[]={"OTTER","CAT","FOX","CAPYBARA"};
static const char *fishNames[]={"MINNOW","SUN CARP","POND KOI","MOON EEL","GOLDEN KOI","STARFISH","CLOUD TROUT","OLD BOOTS"};
static const int fishPrices[]={2,4,7,12,22,18,30,1};
static const int fishXP[]={2,3,5,8,14,12,20,1};
static const u16 fishColors[]={C(23,25,19),C(31,22,8),C(31,13,13),C(18,19,30),C(31,25,7),C(31,19,22),C(20,27,31),C(14,14,13)};
static const u8 font[37][5]={
{7,5,5,5,7},{2,6,2,2,7},{7,1,7,4,7},{7,1,7,1,7},{5,5,7,1,1},{7,4,7,1,7},{7,4,7,5,7},{7,1,1,1,1},{7,5,7,5,7},{7,5,7,1,7},{2,5,7,5,5},{6,5,6,5,6},{3,4,4,4,3},{6,5,5,5,6},{7,4,6,4,7},{7,4,6,4,4},{3,4,5,5,3},{5,5,7,5,5},{7,2,2,2,7},{1,1,1,5,2},{5,5,6,5,5},{4,4,4,4,7},{5,7,7,5,5},{5,7,7,7,5},{2,5,5,5,2},{6,5,6,4,4},{2,5,5,7,3},{6,5,6,5,5},{3,4,2,1,6},{7,2,2,2,2},{5,5,5,5,7},{5,5,5,5,2},{5,5,7,7,5},{5,5,2,5,5},{5,5,2,2,2},{7,1,2,4,7},{0,0,0,0,0}};
static const char alphabet[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
static bool hit(u16 k){return (keys&k)&&!(oldKeys&k);}
static int absI(int a){return a<0?-a:a;}
static void rect(int x,int y,int w,int h,u16 c){if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}if(x+w>W)w=W-x;if(y+h>H)h=H-y;if(w<=0||h<=0)return;for(int j=0;j<h;j++)for(int i=0;i<w;i++)PIX[(y+j)*W+x+i]=c;}
static void dot(int x,int y,u16 c){if(x>=0&&x<W&&y>=0&&y<H)PIX[y*W+x]=c;}
static void clear(u16 c){for(int i=0;i<W*H;i++)PIX[i]=c;}
static void txt(int x,int y,const char*s,u16 c,int scale){while(*s){char ch=*s++;if(ch>='a'&&ch<='z')ch-=32;int n=36;for(int j=0;j<36;j++)if(alphabet[j]==ch){n=j;break;}for(int yy=0;yy<5;yy++)for(int xx=0;xx<3;xx++)if(font[n][yy]&(1<<(2-xx)))rect(x+xx*scale,y+yy*scale,scale,scale,c);x+=4*scale;}}
static void frame(int x,int y,int w,int h){rect(x,y,w,h,INK);rect(x+1,y+1,w-2,h-2,CREAM);rect(x+3,y+3,w-6,h-6,PAPER);rect(x+3,y+3,w-6,1,GOLD);}
static void panel(int x,int y,int w,int h){rect(x,y,w,h,INK);rect(x+1,y+1,w-2,h-2,CREAM);rect(x+3,y+3,w-6,h-6,PALE);}
static void line(int x,int y,int x2,int y2,u16 c){int dx=absI(x2-x),sx=x<x2?1:-1,dy=-absI(y2-y),sy=y<y2?1:-1,err=dx+dy;for(;;){dot(x,y,c);if(x==x2&&y==y2)break;int e=2*err;if(e>=dy){err+=dy;x+=sx;}if(e<=dx){err+=dx;y+=sy;}}}
static void ellipse(int cx,int cy,int rx,int ry,u16 c){for(int y=-ry;y<=ry;y++)for(int x=-rx;x<=rx;x++)if(x*x*ry*ry+y*y*rx*rx<=rx*rx*ry*ry)dot(cx+x,cy+y,c);}
static void flower(int x,int y){dot(x,y-2,PINK);dot(x-2,y,PINK);dot(x+2,y,PINK);dot(x,y+2,WHITE);dot(x,y,GOLD);}
static void fishDraw(int x,int y,u16 c,int size){rect(x+size/3,y,size,2,c);rect(x,y+2,size+size/2,4,c);rect(x+size/3,y+6,size,2,c);rect(x+size+size/2,y+2,size/2,4,ORANGE);dot(x+2,y+3,INK);dot(x+3,y+3,WHITE);}
static void tree(int x,int y,int s){rect(x-s/5,y+s/2,s/2,s/2,TRUNK);ellipse(x,y,s,s,C(7,15,8));ellipse(x-s/3,y-s/4,s/2,s/2,LEAF);ellipse(x+s/3,y-s/4,s/2,s/2,LEAF2);ellipse(x,y-s/2,s/2,s/2,LEAF3);dot(x-s/3,y-s/2,WHITE);dot(x+s/4,y-s/3,MINT);dot(x,y-s/5,GREEN);}
static void hero(int x,int y){u16 fur=animal==0?C(22,14,7):animal==1?C(25,24,21):animal==2?ORANGE:C(20,15,9);rect(x+3,y+10,3,5,SHADOW);rect(x+9,y+10,3,5,SHADOW);rect(x+2,y+3,12,9,fur);rect(x+4,y,3,4,fur);rect(x+10,y,3,4,fur);rect(x+4,y+7,8,4,CREAM);dot(x+5,y+6,INK);dot(x+11,y+6,INK);dot(x+8,y+9,INK);rect(x+5,y+12,2,3,fur);rect(x+10,y+12,2,3,fur);if(animal==0){dot(x+2,y+6,WHITE);dot(x+14,y+6,WHITE);}if(animal==2){dot(x+7,y+3,GOLD);}}
static void waterTile(int x,int y){rect(x,y,16,12,WATER);line(x+2,y+3,x+9,y+3,WATER3);line(x+8,y+8,x+14,y+8,WATER2);dot(x+12,y+2,WHITE);}
static void waterScene(void){rect(0,18,W,142,WATER);for(int y=22;y<154;y+=13)for(int x=((y/13)%2)*9;x<W;x+=31)waterTile(x,y);rect(0,18,W,3,WATER3);}
static void header(const char*s){rect(0,0,W,18,INK);rect(0,17,W,1,GOLD);txt(6,6,s,CREAM,1);txt(185,6,"REEL RPG",GOLD,1);}
static void toast(const char*s){if(toastTicks>0){panel(30,126,180,23);txt(38,135,s,INK,1);toastTicks--;}}
static void setToast(int code){toastCode=code;toastTicks=80;}
static void drawToast(void){if(toastTicks>0){const char *s=toastCode==1?"NICE CATCH":toastCode==2?"FISH ESCAPED":toastCode==3?"LEVEL UP":toastCode==4?"NOT ENOUGH COINS":toastCode==5?"PET BOND UP":toastCode==6?"RECIPE MADE":"SAVED";toast(s);}}
static void treeLine(void){tree(18,36,19);tree(54,31,18);tree(91,37,20);tree(22,82,19);tree(58,76,17);tree(94,85,18);tree(16,126,18);tree(57,125,18);}
static void house(int x,int y){rect(x+3,y+11,40,29,CREAM);rect(x,y+7,46,7,ROOF);rect(x+4,y+1,38,8,ROOF2);rect(x+9,y+14,10,10,BLUE);rect(x+27,y+14,10,10,BLUE);rect(x+20,y+26,9,14,WOOD);rect(x+22,y+28,2,2,GOLD);rect(x+1,y+39,45,3,SHADOW);line(x+9,y+14,x+9,y+23,WHITE);line(x+14,y+14,x+14,y+23,WHITE);}
static void mapBase(void){
 clear(GRASS);
 /* top-down village blocks, inspired by handheld-era cozy RPG maps */
 rect(0,0,240,160,GRASS);for(int y=5;y<160;y+=19)for(int x=(y%2)*7;x<240;x+=23){dot(x,y,GRASS3);dot(x+1,y+1,GRASS2);}
 /* canal and stepping stones */
 rect(0,104,63,56,WATER);for(int y=106;y<158;y+=12)for(int x=(y%2)*8;x<61;x+=27)waterTile(x,y);
 rect(0,101,66,3,C(20,25,20));rect(0,157,66,3,C(20,25,20));ellipse(20,132,8,4,C(15,24,28));ellipse(20,132,5,2,C(18,27,30));
 /* paved market plaza */
 rect(91,0,149,105,TILE);for(int y=1;y<104;y+=12)for(int x=93+(y%2)*6;x<239;x+=24){line(x,y,x+12,y,TILE2);line(x+12,y,x+12,y+5,TILE3);}
 /* road toward pier */
 rect(62,70,44,90,PATH);rect(67,70,34,90,PATH2);for(int y=74;y<158;y+=14)rect(79,y,2,5,CREAM);
 /* flower beds and trees */
 tree(15,27,17);tree(46,18,14);tree(78,31,15);tree(22,66,16);tree(52,58,14);tree(18,89,13);
 for(int i=0;i<7;i++){flower(69+i*7,49+(i%2)*6);flower(10+i*7,96+(i%3)*2);}
 /* market buildings */
 house(113,18);house(176,16);
 /* shop awnings */
 rect(108,29,53,12,ROOF2);for(int x=110;x<160;x+=10){rect(x,29,5,12,CREAM);rect(x,38,5,3,ROOF);}
 rect(171,28,56,12,PINK);for(int x=174;x<225;x+=10){rect(x,28,5,12,CREAM);}
 /* fence by boardwalk */
 rect(107,82,132,4,WOOD);for(int x=109;x<239;x+=12)rect(x,78,3,10,CREAM);
 /* dock planks */
 rect(115,88,124,23,WOOD);for(int x=119;x<238;x+=14){rect(x,90,1,19,C(26,16,9));rect(x+1,91,10,1,GOLD);}
 /* buoy and plants */
 ellipse(101,112,7,4,C(20,17,10));ellipse(101,110,4,4,ORANGE);
 for(int i=0;i<4;i++)flower(91+i*7,122+(i%2)*6);
}
static void title(void){clear(C(13,23,13));rect(0,0,240,160,GRASS);tree(24,38,22);tree(207,35,22);tree(32,112,19);tree(211,117,20);waterScene();frame(22,18,196,56);txt(47,29,"REEL RPG",GOLD,3);txt(52,49,"ISLAND FISHING LIFE",INK,1);hero(111,80);fishDraw(65,102,MINT,9);fishDraw(166,116,ORANGE,9);panel(51,133,138,18);txt(75,140,"PRESS START",INK,1);}
static void selectHero(void){clear(GRASS);header("CHOOSE ANGLER");txt(16,27,"PICK YOUR TRAVEL PAL",INK,1);for(int i=0;i<4;i++){int y=39+i*27;panel(19,y,202,23);if(i==animal)rect(23,y+4,194,15,GOLD);hero(31,y+5);txt(55,y+9,animals[i],INK,1);txt(167,y+9,i==0?"SWIMMER":i==1?"LUCKY":i==2?"QUICK":"COZY",INK,1);}txt(13,151,"UP DOWN  A CHOOSE",WHITE,1);}
static void town(void){
 mapBase();
 /* character and readable map labels */
 hero(px,py);
 header(island==0?"BRINEBELL VILLAGE":island==1?"SUNPETAL ISLAND":"MOONGLASS COVE");
 panel(4,21,67,19);txt(9,26,"COINS",INK,1);char num[8];int n=coins;num[0]='0'+(n/100)%10;num[1]='0'+(n/10)%10;num[2]='0'+n%10;num[3]=0;txt(46,26,num,INK,1);
 panel(171,21,65,19);txt(177,26,"LV",INK,1);num[0]='0'+(level/10)%10;num[1]='0'+level%10;num[2]=0;txt(193,26,num,INK,1);txt(211,26,"HP",INK,1);
 /* building interaction hints */
 panel(4,127,232,28);txt(10,134,"A INTERACT  START BAG",INK,1);txt(10,145,"SELECT JOURNAL  B PET",INK,1);
 drawToast();
}
static void fishing(void){
 waterScene();header("FISHING SPOT");rect(0,111,240,49,GRASS);rect(0,108,240,4,PATH2);for(int x=4;x<235;x+=19)rect(x,114,11,2,GRASS3);hero(25,117);fishDraw(157,73,hooked?ORANGE:MINT,9);
 panel(30,27,180,36);txt(43,36,hooked?"A BITE! PRESS A":"WAIT FOR A BITE",INK,1);txt(43,49,"B TO GO BACK",INK,1);
 txt(10,143,"BAIT",WHITE,1);rect(45,141,70,8,INK);rect(47,143,bait*13,4,GOLD);txt(132,143,"FISH",WHITE,1);rect(163,141,65,8,INK);rect(165,143,(catches%6+1)*10,4,MINT);
}
static void reel(void){waterScene();header("REEL IT IN");rect(0,92,240,68,GRASS);hero(24,119);fishDraw(154+(progress%36),59,fishColors[fishKind],10);panel(13,25,214,58);txt(24,33,"KEEP THE FISH ON",INK,1);rect(24,47,192,13,INK);rect(26,49,188,9,C(18,18,18));rect(90,49,62,9,MINT);rect(26+tension*184/100,46,4,15,GOLD);txt(24,68,"A PULL   B EASE",INK,1);txt(22,143,"TENSION",WHITE,1);rect(83,141,143,8,INK);rect(85,143,tension*139/100,4,tension>78?ORANGE:MINT);}
static void bag(void){
 clear(GRASS);header("TRAVEL BAG");panel(10,23,220,117);
 const char *opts[]={"FISH COLLECTION","BAIT AND GEAR","COOK A MEAL","SELL ALL FISH","SAVE GAME","CLOSE BAG"};
 for(int i=0;i<6;i++){int y=31+i*17;if(i==bagCursor)rect(17,y-2,206,14,GOLD);txt(24,y+2,opts[i],INK,1);}
 txt(15,149,"UP DOWN A SELECT B BACK",WHITE,1);drawToast();
}
static void journal(void){clear(C(22,25,20));header("FISH JOURNAL");panel(8,24,224,119);for(int i=0;i<4;i++){int k=(bagCursor+i)%8;int y=33+i*25;fishDraw(18,y,fishColors[k],8);txt(46,y+1,fishNames[k],INK,1);txt(46,y+11,caught[k]?"CAUGHT":"NOT FOUND",caught[k]?MINT:C(14,14,12),1);txt(184,y+6,caught[k]?"YES":"???",INK,1);}txt(12,151,"LEFT RIGHT PAGE  B BACK",WHITE,1);}
static void petScreen(void){clear(GRASS);header("YOUR COMPANION");panel(15,25,210,112);tree(62,69,23);hero(112,66);if(!hasPet)txt(29,104,"FIND A PET ON THE ISLANDS",INK,1);else{txt(33,104,pet==0?"LITTLE OTTER":pet==1?"BREEZY CAT":"TINY FOX",INK,1);txt(34,116,"BOND",INK,1);rect(75,116,110,8,INK);rect(77,118,petBond,4,PINK);}txt(22,143,"A GIVE FISH  B BACK",INK,1);}
static void shopScreen(void){clear(TILE);header("TIDEPOOL MARKET");panel(12,25,216,116);txt(25,37,"BAIT PACK   5 COINS",INK,1);txt(25,56,"WOODEN ROD 18 COINS",INK,1);txt(25,75,"PET SNACK    8 COINS",INK,1);txt(25,94,"COOKED FISH  3 FISH",INK,1);txt(25,117,"A BUY  UP DOWN",INK,1);if(menuCursor==0)rect(19,34,201,13,GOLD);else if(menuCursor==1)rect(19,53,201,13,GOLD);else if(menuCursor==2)rect(19,72,201,13,GOLD);else rect(19,91,201,13,GOLD);txt(14,151,"B EXIT",WHITE,1);}
static void cookScreen(void){clear(C(18,24,15));header("CAMPFIRE COOKING");rect(80,54,80,47,WOOD);ellipse(120,75,27,18,ORANGE);ellipse(120,73,18,12,GOLD);ellipse(120,72,9,7,WHITE);fishDraw(111,56,MINT,8);panel(15,24,210,25);txt(25,33,"COOK 3 FISH INTO A MEAL",INK,1);panel(26,112,188,29);txt(42,122,"A COOK   B BACK",INK,1);}
static void islandScreen(void){clear(C(9,21,25));waterScene();ellipse(120,86,82,46,GRASS2);ellipse(120,82,73,42,GRASS);tree(85,64,16);tree(151,72,19);tree(119,49,16);house(100,79);rect(112,103,16,20,PATH2);hero(117,108);panel(27,22,186,20);txt(38,29,"NEW ISLAND DISCOVERED",INK,1);panel(30,132,180,19);txt(42,139,"A EXPLORE  B RETURN",INK,1);}
static void battleScreen(void){clear(C(19,25,17));header("WILD ENCOUNTER");rect(0,22,240,62,C(20,26,18));ellipse(175,61,24,17,C(9,16,9));ellipse(170,54,16,14,C(13,21,12));dot(164,52,WHITE);rect(0,85,240,75,C(13,21,12));hero(45,103);ellipse(173,111,18,12,ORANGE);ellipse(169,105,12,10,C(31,20,8));panel(10,24,111,31);txt(17,31,"WILD SLIME",INK,1);txt(17,43,"HP",INK,1);rect(40,43,68,5,INK);rect(42,44,battleHp*64/30,3,MINT);panel(122,112,108,35);txt(130,120,"A SPLASH",INK,1);txt(130,132,"B RUN",INK,1);txt(14,145,"YOUR HP",WHITE,1);rect(65,143,95,6,INK);rect(67,145,heroHp*91/30,2,PINK);}
static void menuScreen(void){clear(GRASS);header("ISLAND GUIDE");panel(14,25,212,119);const char*o[]={"VISIT TIDEPOOL MARKET","CAMPFIRE COOKING","TRAVEL TO ISLAND","WILD PET BATTLE","FISHING JOURNAL","RETURN TO VILLAGE"};for(int i=0;i<6;i++){int y=34+i*17;if(menuCursor==i)rect(20,y-2,200,14,GOLD);txt(26,y+2,o[i],INK,1);}txt(13,151,"UP DOWN A SELECT B BACK",WHITE,1);}
static void saveGame(void){volatile u8 *s=(volatile u8*)0x0E000000;s[0]='R';s[1]='E';s[2]='E';s[3]='L';s[4]=1;s[5]=(u8)animal;s[6]=(u8)coins;s[7]=(u8)level;s[8]=(u8)xp;s[9]=(u8)catches;s[10]=(u8)bait;s[11]=(u8)island;s[12]=(u8)hasPet;s[13]=(u8)petBond;for(int i=0;i<8;i++)s[14+i]=(u8)fishTotal[i];s[22]=0xA5;saved=true;setToast(7);}
static void catchFish(void){caught[fishKind]=1;fishTotal[fishKind]++;catches++;coins+=fishPrices[fishKind]/2;xp+=fishXP[fishKind];if(fishKind<3)fishCount++;if(xp>=level*10){xp-=level*10;level++;stamina=100;setToast(3);}else setToast(1);if(hasPet&&petBond<100)petBond+=2;}
static void interact(void){
 if(island>0){scene=8;return;}
 if(px<76&&py>90){scene=3;hooked=0;waitTicks=0;if(bait<=0)setToast(4);return;}
 if(py<52&&px>100&&px<170){scene=6;menuCursor=0;return;}
 if(py<52&&px>=170){scene=6;menuCursor=0;return;}
 if(py>75&&px>108){scene=7;menuCursor=0;return;}
 scene=10;menuCursor=0;
}
int main(void){
 REG_DISPCNT=MODE_3|BG2_ENABLE;
 while(1){
  oldKeys=keys;scanKeys();keys=keysDown();
  if(scene==0){title();if(hit(KEY_START)||hit(KEY_A))scene=1;}
  else if(scene==1){selectHero();if(hit(KEY_UP)||hit(KEY_LEFT))animal=(animal+3)%4;if(hit(KEY_DOWN)||hit(KEY_RIGHT))animal=(animal+1)%4;if(hit(KEY_A)){scene=2;px=119;py=99;}}
  else if(scene==2){
   town();
   if(hit(KEY_UP)&&py>23)py-=4;if(hit(KEY_DOWN)&&py<137)py+=4;if(hit(KEY_LEFT)&&px>5)px-=4;if(hit(KEY_RIGHT)&&px<222)px+=4;
   if(hit(KEY_A))interact();
   if(hit(KEY_START)){scene=5;bagCursor=0;}
   if(hit(KEY_SELECT)){scene=9;bagCursor=0;}
   if(hit(KEY_B)){scene=11;}
  }
  else if(scene==3){
   fishing();waitTicks++;
   if(bait<=0){if(waitTicks==1)setToast(4);if(hit(KEY_B)||waitTicks>45)scene=2;}
   else if(waitTicks>95){hooked=1;fishKind=(waitTicks/13+level+day)%7;if(waitTicks>105&&waitTicks%3==0)fishKind=(fishKind+1)%7;}
   if(hooked&&hit(KEY_A)){bait--;scene=4;progress=0;tension=43;}
   if(hit(KEY_B)){scene=2;hooked=0;}
  }
  else if(scene==4){
   reel();progress++;tension+=((progress%17)<8?1:-1);if(tension<8)tension=8;if(tension>96)tension=96;
   if(hit(KEY_A)){tension+=10;progress+=2;}if(hit(KEY_B)){tension-=12;if(tension<0)tension=0;}
   if(progress>78){if(tension>18&&tension<82){catchFish();scene=2;}else{setToast(2);scene=2;}hooked=0;}
   if(hit(KEY_START)){scene=2;hooked=0;}
  }
  else if(scene==5){
   bag();if(hit(KEY_UP))bagCursor=(bagCursor+5)%6;if(hit(KEY_DOWN))bagCursor=(bagCursor+1)%6;
   if(hit(KEY_B))scene=2;
   if(hit(KEY_A)){if(bagCursor==0){scene=9;bagCursor=0;}else if(bagCursor==1){scene=6;menuCursor=0;}else if(bagCursor==2){scene=7;menuCursor=0;}else if(bagCursor==3){if(catches>0){coins+=catches*2;catches=0;for(int i=0;i<8;i++)fishTotal[i]=0;setToast(1);}else setToast(4);}else if(bagCursor==4){saveGame();scene=2;}else scene=2;}
  }
  else if(scene==6){
   shopScreen();if(hit(KEY_UP))menuCursor=(menuCursor+3)%4;if(hit(KEY_DOWN))menuCursor=(menuCursor+1)%4;if(hit(KEY_B))scene=2;
   if(hit(KEY_A)){if(menuCursor==0){if(coins>=5){coins-=5;bait+=5;setToast(1);}else setToast(4);}if(menuCursor==1){if(coins>=18){coins-=18;rods++;setToast(1);}else setToast(4);}if(menuCursor==2){if(coins>=8){coins-=8;hasPet=true;petBond+=5;setToast(5);}else setToast(4);}if(menuCursor==3){scene=7;}}
  }
  else if(scene==7){
   if(menuCursor==0){cookScreen();if(hit(KEY_A)){if(catches>=3){catches-=3;cooked++;stamina=100;setToast(6);}else setToast(4);}if(hit(KEY_B))scene=2;}
   else{petScreen();if(hit(KEY_A)){if(catches>0){catches--;hasPet=true;if(petBond<100)petBond+=10;setToast(5);}else setToast(4);}if(hit(KEY_B))scene=2;}
  }
  else if(scene==8){islandScreen();if(hit(KEY_A)){island=(island+1)%3;day++;px=120;py=108;scene=2;setToast(1);}if(hit(KEY_B))scene=2;}
  else if(scene==9){
   if(menuCursor==0){journal();if(hit(KEY_LEFT))bagCursor=(bagCursor+4)%8;if(hit(KEY_RIGHT))bagCursor=(bagCursor+4)%8;if(hit(KEY_B)||hit(KEY_START))scene=2;}
   else{menuScreen();if(hit(KEY_UP))menuCursor=(menuCursor+5)%6;if(hit(KEY_DOWN))menuCursor=(menuCursor+1)%6;if(hit(KEY_B))scene=2;if(hit(KEY_A)){if(menuCursor==0){scene=6;menuCursor=0;}else if(menuCursor==1){scene=7;menuCursor=0;}else if(menuCursor==2){scene=8;}else if(menuCursor==3){scene=12;battleHp=30;heroHp=30;}else if(menuCursor==4){scene=9;menuCursor=0;}else scene=2;}}
  }
  else if(scene==10){clear(GRASS);header("VILLAGE NOTICE");panel(13,27,214,103);txt(25,38,"WELCOME TO BRINEBELL",INK,1);txt(25,53,"CAST BY THE WATER",INK,1);txt(25,66,"SELL FISH FOR COINS",INK,1);txt(25,79,"BUY BAIT AT THE SHOP",INK,1);txt(25,92,"FIND A PET COMPANION",INK,1);txt(25,105,"EXPLORE THE ISLANDS",INK,1);txt(16,147,"A OR B TO RETURN",WHITE,1);if(hit(KEY_A)||hit(KEY_B))scene=2;}
  else if(scene==11){petScreen();if(hit(KEY_A)){if(catches>0){catches--;hasPet=true;petBond+=8;setToast(5);}else hasPet=!hasPet;}if(hit(KEY_B)||hit(KEY_START))scene=2;}
  else if(scene==12){
   battleScreen();
   if(hit(KEY_A)){battleHp-=8+(petBond/25);if(battleHp<=0){coins+=8;xp+=6;if(xp>=level*10){xp-=level*10;level++;setToast(3);}else setToast(1);scene=2;}else{heroHp-=4;if(heroHp<=0){heroHp=30;coins=coins>5?coins-5:0;scene=2;setToast(2);}}}
   if(hit(KEY_B)){scene=2;}
  }
  if(hit(KEY_L)&&scene==2){island=(island+1)%3;day++;setToast(1);}
  if(hit(KEY_R)&&scene==2){scene=8;}
  VBlankIntrWait();
 }
 return 0;
}