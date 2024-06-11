#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Arduino_ST7789_Fast.h>

//Input Butonları
#define JoyX A0
#define JoyY A1
#define ButtonA 2
#define ButtonB 3
#define ButtonC 4
#define ButtonD 5
#define ButtonE 6
#define ButtonF 7
#define JoyButton 8 

#define LCD_DC 9 //Data Connection Pini
#define LCD_RST 10 //Reset Pini
#define SCR_WIDTH 240 //LCD Ekran genişliği
#define SCR_HEIGHT 240 //LCD Ekran genişliği

//Çizim yapmak için lcd ekran objesi
Arduino_ST7789 lcd = Arduino_ST7789(LCD_DC, LCD_RST); 

//Poziyon tutmak için vector structı
struct Vec2 {
public:
  int16_t x;
  int16_t y;
};

struct Kare {
public:
  Vec2 position;
  uint8_t rotation = 0;
  uint16_t color = RED;
  Vec2 points[4] = {
    {0, 1}, 
    {1, 1}, 
    {0, 0}, 
    {1, 0}
  }; 
};

struct Cubuk{
public:
  Vec2 position;
  uint8_t rotation = 0;
  uint16_t color = LGREY;
  Vec2 points[4] = {
    {0,  1}, 
    {0,  0}, 
    {0, -1}, 
    {0, -2}
  }; 
};

struct Zblok{
public:
  Vec2 position;
  uint8_t rotation = 0;
  uint16_t color = RED;
  Vec2 points[4] = {
    {-1,  0}, 
    { 0,  0}, 
    { 0, -1}, 
    { 1, -1}
  }; 
};

struct Lblok{
public:  
  Vec2 position;
  uint8_t rotation = 0;
  uint16_t color = WHITE;
  Vec2 points[4] = {
    {0,  1}, 
    {0,  0}, 
    {0, -1}, 
    {1, -1}
  }; 
};

struct Tblok{
public:  
  Vec2 position;
  uint8_t rotation = 0;
  uint16_t color = CYAN;
  Vec2 points[4] = {
    {-1, 0}, 
    {0,  0}, 
    {1,  0}, 
    {0, -1}
  }; 
};
unsigned long prevTime;
unsigned long frameTime = 0;

//Buton değerleri
int valueBA = 1; //Button A değeri
int valueBB = 1; //Button B değeri
int valueBC = 1; //Button C değeri
int valueBD = 1; //Button D değeri
int valueBE = 1; //Button E değeri
int valueBF = 1; //Button F değeri
int valueJB = 1; //Joybutton değeri

//Joystick eksen değerleri [0, 1000], 500 orta değer
int valueJoyX = 500;
int valueJoyY = 500;

int gridWidth = 8; //Satır sayısı
int gridHeight = 12; //Sütün sayısı
int squareSize = 20; //Kare büyüklüğü
int offsetX = 40; //Grid in ekrandaki kayması

bool grid[12][8] = {false};

Kare kare = {};
Cubuk cubuk = {};
Zblok zblok = {};
Lblok lblok = {};
Tblok tblok= {};

int bloktipi = -1;
int oyunDurumu = 0; //0 = Baslangıc, 1 = Oyun döngüsü, 2 = Game over

void KareOlustur(Vec2 pos) {
  if(pos.x == gridWidth - 1)
    pos.x--;

  kare.position = pos;
}

void CubukOlustur(Vec2 pos) {
  cubuk.position = pos;
  cubuk.rotation = 0;
  cubuk.points[0] = {0,  1}; 
  cubuk.points[1] = {0,  0}; 
  cubuk.points[2] = {0, -1}; 
  cubuk.points[3] = {0, -2};
}

void ZblokOlustur(Vec2 pos) {
  if(pos.x == 0)
    pos.x++;
  else if(pos.x == gridWidth - 1)
    pos.x--;

  zblok.position = pos;
  zblok.rotation = 0;
  zblok.points[0] = {-1,  0}; 
  zblok.points[1] = { 0,  0}; 
  zblok.points[2] = { 0, -1}; 
  zblok.points[3] = { 1, -1};
}

void LblokOlustur(Vec2 pos) {
  if(pos.x == gridWidth - 1)
    pos.x--;

  lblok.position = pos;
  lblok.rotation = 0;
  lblok.points[0] = {0,  1}; 
  lblok.points[1] = {0,  0}; 
  lblok.points[2] = {0, -1}; 
  lblok.points[3] = {1, -1};
}
void TblokOlustur(Vec2 pos) {
  if(pos.x == 0)
    pos.x++;
  else if(pos.x == gridWidth - 1)
    pos.x--;

  tblok.position = pos;
  tblok.rotation = 0;
  tblok.points[0] = {-1, 0}; 
  tblok.points[1] = {0,  0}; 
  tblok.points[2] = {1,  0};
  tblok.points[3] = {0, -1};
}

bool CarpmaKontrolu(Vec2 pos, Vec2 points[4], Vec2 yon) {
  for(int i = 0; i < 4; i++) {
    if (grid[pos.y + points[i].y + yon.y][pos.x + points[i].x + yon.x] == true)
      return true;
  }

  return false;
}

bool GridDisinaCikmaKontrolu(Vec2 pos, Vec2 points[4], Vec2 yon) {
  for(int i = 0; i < 4; i++) {
    Vec2 kordinat = {pos.x + points[i].x + yon.x, pos.y + points[i].y + yon.y};

      if(kordinat.x < 0 || kordinat.x >= gridWidth || 
         kordinat.y < 0 || kordinat.y >= gridHeight)
        return true;
  }

  return false;
}

void KareCiz() {
  lcd.fillRect((kare.position.x + kare.points[0].x) * squareSize + offsetX,
  SCR_HEIGHT - (kare.position.y + kare.points[0].y + 1) * squareSize, squareSize,squareSize, kare.color);

  lcd.fillRect((kare.position.x + kare.points[1].x) * squareSize + offsetX,
  SCR_HEIGHT - (kare.position.y + kare.points[1].y + 1) * squareSize, squareSize,squareSize, kare.color);
  
  lcd.fillRect((kare.position.x + kare.points[2].x) * squareSize + offsetX,
  SCR_HEIGHT - (kare.position.y + kare.points[2].y + 1) * squareSize, squareSize,squareSize, kare.color);
  
  lcd.fillRect((kare.position.x + kare.points[3].x) * squareSize + offsetX,
  SCR_HEIGHT - (kare.position.y + kare.points[3].y + 1) * squareSize, squareSize,squareSize, kare.color);
}

void CubukCiz() {
  lcd.fillRect((cubuk.position.x + cubuk.points[0].x) * squareSize + offsetX,
  SCR_HEIGHT - (cubuk.position.y + cubuk.points[0].y + 1) * squareSize, squareSize,squareSize, cubuk.color);
 
  lcd.fillRect((cubuk.position.x + cubuk.points[1].x) * squareSize + offsetX,
  SCR_HEIGHT - (cubuk.position.y + cubuk.points[1].y + 1) * squareSize, squareSize,squareSize, cubuk.color);
 
  lcd.fillRect((cubuk.position.x + cubuk.points[2].x) * squareSize + offsetX,
  SCR_HEIGHT - (cubuk.position.y + cubuk.points[2].y + 1) * squareSize, squareSize,squareSize, cubuk.color);
 
  lcd.fillRect((cubuk.position.x + cubuk.points[3].x) * squareSize + offsetX,
  SCR_HEIGHT - (cubuk.position.y + cubuk.points[3].y + 1) * squareSize, squareSize,squareSize, cubuk.color);
}

void ZblokCiz() {
  lcd.fillRect((zblok.position.x + zblok.points[0].x) * squareSize + offsetX,
  SCR_HEIGHT - (zblok.position.y + zblok.points[0].y + 1) * squareSize, squareSize,squareSize, zblok.color);
  
  lcd.fillRect((zblok.position.x + zblok.points[1].x) * squareSize + offsetX,
  SCR_HEIGHT - (zblok.position.y + zblok.points[1].y + 1) * squareSize, squareSize,squareSize, zblok.color);
 
  lcd.fillRect((zblok.position.x + zblok.points[2].x) * squareSize + offsetX,
  SCR_HEIGHT - (zblok.position.y + zblok.points[2].y + 1) * squareSize, squareSize,squareSize, zblok.color);
 
  lcd.fillRect((zblok.position.x + zblok.points[3].x) * squareSize + offsetX,
  SCR_HEIGHT - (zblok.position.y + zblok.points[3].y + 1) * squareSize, squareSize,squareSize, zblok.color);
}

void LblokCiz() {
  lcd.fillRect((lblok.position.x + lblok.points[0].x) * squareSize + offsetX,
  SCR_HEIGHT - (lblok.position.y + lblok.points[0].y + 1) * squareSize, squareSize,squareSize, lblok.color);

  lcd.fillRect((lblok.position.x + lblok.points[1].x) * squareSize + offsetX,
  SCR_HEIGHT - (lblok.position.y + lblok.points[1].y + 1) * squareSize, squareSize,squareSize, lblok.color);
  
  lcd.fillRect((lblok.position.x + lblok.points[2].x) * squareSize + offsetX,
  SCR_HEIGHT - (lblok.position.y + lblok.points[2].y + 1) * squareSize, squareSize,squareSize, lblok.color);
  
  lcd.fillRect((lblok.position.x + lblok.points[3].x) * squareSize + offsetX,
  SCR_HEIGHT - (lblok.position.y + lblok.points[3].y + 1) * squareSize, squareSize,squareSize, lblok.color);
}
void TblokCiz() {
  lcd.fillRect((tblok.position.x + tblok.points[0].x) * squareSize + offsetX,
  SCR_HEIGHT - (tblok.position.y + tblok.points[0].y + 1) * squareSize, squareSize,squareSize, tblok.color);

  lcd.fillRect((tblok.position.x + tblok.points[1].x) * squareSize + offsetX,
  SCR_HEIGHT - (tblok.position.y + tblok.points[1].y + 1) * squareSize, squareSize,squareSize, tblok.color);
  
  lcd.fillRect((tblok.position.x + tblok.points[2].x) * squareSize + offsetX,
  SCR_HEIGHT - (tblok.position.y + tblok.points[2].y + 1) * squareSize, squareSize,squareSize, tblok.color);
  
  lcd.fillRect((tblok.position.x + tblok.points[3].x) * squareSize + offsetX,
  SCR_HEIGHT - (tblok.position.y + tblok.points[3].y + 1) * squareSize, squareSize,squareSize, tblok.color);
}

void GridControl() {
  for(int y = 0; y < gridHeight;) {
    bool cizgiOlusumu = true;
    for(int x = 0; x < gridWidth; x++) {
      if(grid[y][x] == false) {
        cizgiOlusumu = false;
        break;
      }
    }

    if(cizgiOlusumu) {
      for(int x = 0; x < gridWidth; x++) {
        grid[y][x] == false;
      }

      for(int k = y; k < gridHeight; k++) {
        for(int x = 0; x < gridWidth; x++) {
          if(k < gridHeight -1) {
            grid[k][x] = grid[k + 1][x];
          }
          else {
            grid[k][x] = false;
          } 
        }
      }

      continue;
    }

    y++;
  }
}

//Joystick ve buton inputlarını kaydeder
void InputControl() {
  valueBA = digitalRead(ButtonA);
  valueBB = digitalRead(ButtonB);
  valueBC = digitalRead(ButtonC);
  valueBD = digitalRead(ButtonD);
  valueBE = digitalRead(ButtonE);
  valueBF = digitalRead(ButtonF);
  valueJB = digitalRead(JoyButton);

  valueJoyX = analogRead(JoyX);
  valueJoyY = analogRead(JoyY);
}

void GameUpdate() {
  if(oyunDurumu == 0) { //Baslangıc
    if(valueBC == 1) {
      oyunDurumu = 1;
      bloktipi = -1;
    }
  } 
  else if(oyunDurumu == 1) { //Oyun döngüsü
    if(valueBC == 1){
      bloktipi = -1; 
    }

    if(bloktipi == -1) {
      int rastgelBlokTipi = random(0, 5);
      int rastGeleYatayKonum = random(0, 8);
      bloktipi = rastgelBlokTipi;

      Vec2 pos = {rastGeleYatayKonum, gridHeight};

      if(bloktipi == 0) {
        KareOlustur(pos);
      }
      else if(bloktipi == 1) {
        CubukOlustur(pos);
      }
      else if(bloktipi == 2) {
        ZblokOlustur(pos);
      }
      else if(bloktipi == 3) {
        LblokOlustur(pos);
      }
      else if(bloktipi == 4) {
        TblokOlustur(pos);
      }
    }
    else {
      bool cizgiKontrol = false;

      if(bloktipi == 0) {
        kare.position.y--;

        if(kare.position.y <= 0  || CarpmaKontrolu(kare.position, kare.points, {0, -1})) {
          bloktipi = -1;

          if(GridDisinaCikmaKontrolu(kare.position, kare.points, {0, 0})) {
            oyunDurumu = 2;
          }
          else {
            grid[kare.position.y + kare.points[0].y][kare.position.x + kare.points[0].x] = true;
            grid[kare.position.y + kare.points[1].y][kare.position.x + kare.points[1].x] = true;
            grid[kare.position.y + kare.points[2].y][kare.position.x + kare.points[2].x] = true;
            grid[kare.position.y + kare.points[3].y][kare.position.x + kare.points[3].x] = true;

            cizgiKontrol = true;
          }
        }
        else {
          if(valueBD == 1 && !GridDisinaCikmaKontrolu(kare.position, kare.points, {-1, 0})
           && !CarpmaKontrolu(kare.position, kare.points, {-1, 0})) {
            kare.position.x--;
          }
          if(valueBB == 1 && !GridDisinaCikmaKontrolu(kare.position, kare.points, {1, 0})
           && !CarpmaKontrolu(kare.position, kare.points, {1, 0})) {
            kare.position.x++;
          }
        }
      }
      else if(bloktipi == 1) {
        cubuk.position.y--;

        if(cubuk.position.y <= 2 || CarpmaKontrolu(cubuk.position, cubuk.points, {0, -1})) {
          bloktipi = -1;

          if(GridDisinaCikmaKontrolu(cubuk.position, cubuk.points, {0, 0})) {
            oyunDurumu = 2;
          }
          else {
            grid[cubuk.position.y + cubuk.points[0].y][cubuk.position.x + cubuk.points[0].x] = true;
            grid[cubuk.position.y + cubuk.points[1].y][cubuk.position.x + cubuk.points[1].x] = true;
            grid[cubuk.position.y + cubuk.points[2].y][cubuk.position.x + cubuk.points[2].x] = true;
            grid[cubuk.position.y + cubuk.points[3].y][cubuk.position.x + cubuk.points[3].x] = true;

            cizgiKontrol = true;
          }
        }
        else {
          if(valueBD == 1 && !GridDisinaCikmaKontrolu(cubuk.position, cubuk.points, {-1, 0})
           && !CarpmaKontrolu(cubuk.position, cubuk.points, {-1, 0})) {
            cubuk.position.x--;
          }
          if(valueBB == 1 && !GridDisinaCikmaKontrolu(cubuk.position, cubuk.points, {1, 0})
           && !CarpmaKontrolu(cubuk.position, cubuk.points, {1, 0})) {
            cubuk.position.x++;
          }
        }
      }
      else if(bloktipi == 2) {
        zblok.position.y--;

        if(zblok.position.y <= 1 || CarpmaKontrolu(zblok.position, zblok.points, {0, -1})) {
          bloktipi = -1;
          if(GridDisinaCikmaKontrolu(zblok.position, zblok.points, {0, 0})) {
            oyunDurumu = 2;
          }
          else {
            grid[zblok.position.y + zblok.points[0].y][zblok.position.x + zblok.points[0].x] = true;
            grid[zblok.position.y + zblok.points[1].y][zblok.position.x + zblok.points[1].x] = true;
            grid[zblok.position.y + zblok.points[2].y][zblok.position.x + zblok.points[2].x] = true;
            grid[zblok.position.y + zblok.points[3].y][zblok.position.x + zblok.points[3].x] = true;

            cizgiKontrol = true;
          }
        }
        else {
          if(valueBD == 1 && !GridDisinaCikmaKontrolu(zblok.position, zblok.points, {-1, 0})
           && !CarpmaKontrolu(zblok.position, zblok.points, {-1, 0})) {
            zblok.position.x--;
          }
          if(valueBB == 1 && !GridDisinaCikmaKontrolu(zblok.position, zblok.points, {1, 0})
           && !CarpmaKontrolu(zblok.position, zblok.points, {1, 0})) {
            zblok.position.x++;
          }
        }
      }
      else if(bloktipi == 3) {
        lblok.position.y--;

        if(lblok.position.y <= 1 || CarpmaKontrolu(lblok.position, lblok.points, {0, -1})) {
          bloktipi = -1;

          if(GridDisinaCikmaKontrolu(lblok.position, lblok.points, {0, 0})) {
            oyunDurumu = 2;
          }
          else {
            grid[lblok.position.y + lblok.points[0].y][lblok.position.x + lblok.points[0].x] = true;
            grid[lblok.position.y + lblok.points[1].y][lblok.position.x + lblok.points[1].x] = true;
            grid[lblok.position.y + lblok.points[2].y][lblok.position.x + lblok.points[2].x] = true;
            grid[lblok.position.y + lblok.points[3].y][lblok.position.x + lblok.points[3].x] = true;

            cizgiKontrol = true;
          }
        }
        else {
          if(valueBD == 1 && !GridDisinaCikmaKontrolu(lblok.position, lblok.points, {-1, 0})
           && !CarpmaKontrolu(lblok.position, lblok.points, {-1, 0})) {
            lblok.position.x--;
          }
          if(valueBB == 1 && !GridDisinaCikmaKontrolu(lblok.position, lblok.points, {1, 0})
           && !CarpmaKontrolu(lblok.position, lblok.points, {1, 0})) {
            lblok.position.x++;
          }
        }
      }
      else if(bloktipi == 4) {
        tblok.position.y--;

        if(tblok.position.y <= 1 || CarpmaKontrolu(tblok.position, tblok.points, {0, -1})) {
          bloktipi = -1;

          if(GridDisinaCikmaKontrolu(tblok.position, tblok.points, {0, 0})) {
            oyunDurumu = 2;
          }
          else {
            grid[tblok.position.y + tblok.points[0].y][tblok.position.x + tblok.points[0].x] = true;
            grid[tblok.position.y + tblok.points[1].y][tblok.position.x + tblok.points[1].x] = true;
            grid[tblok.position.y + tblok.points[2].y][tblok.position.x + tblok.points[2].x] = true;
            grid[tblok.position.y + tblok.points[3].y][tblok.position.x + tblok.points[3].x] = true;

            cizgiKontrol = true;
          }
        }
        else {
          if(valueBD == 1 && !GridDisinaCikmaKontrolu(tblok.position, tblok.points, {-1, 0})
           && !CarpmaKontrolu(tblok.position, tblok.points, {-1, 0})) {
            tblok.position.x--;
          }
          if(valueBB == 1 && !GridDisinaCikmaKontrolu(tblok.position, tblok.points, {1, 0})
           && !CarpmaKontrolu(tblok.position, tblok.points, {1, 0})) {
            tblok.position.x++;
          }
        }
      }

      if(cizgiKontrol == true) {
        GridControl();
      }
    }
  }
  else if(oyunDurumu == 2) { //Game Over
    if(valueBC == 1) {
      oyunDurumu = 1;
      bloktipi = -1;

      for(int y = 0; y < gridHeight; y++) {
        for(int x = 0; x < gridWidth; x++) {
          grid[y][x] = false;
        }
      }
    }
  }
}

void RenderScreen() {
  //Arka plan rengi
  lcd.fillScreen(BLACK);

  if(oyunDurumu == 0) {
    lcd.setCursor(20, 100);
    lcd.setTextSize(6);
    lcd.println("Tetris");
  }
  else if(oyunDurumu == 1) {
    //Grid çizimi
    for(int x = 0; x < gridWidth + 1; x++) {
      lcd.drawLine(offsetX + x * squareSize, 0, offsetX + x * squareSize, SCR_HEIGHT, LBLUE);
    }

    for(int y = 0; y < gridHeight + 1; y++) {
      lcd.drawLine(offsetX , y * squareSize, SCR_WIDTH - offsetX, y * squareSize, LBLUE);
    }

    for(uint8_t y = 0; y < gridHeight; y++) {
      for(uint8_t x = 0; x < gridWidth; x++) {
        if(grid[y][x] == true) {
          lcd.fillRect(x * squareSize + offsetX, SCR_HEIGHT - (y + 1) * squareSize, squareSize, squareSize, YELLOW);
        }
      } 
    }

    if(bloktipi == 0) {
      KareCiz();
    }
    else if(bloktipi == 1) {
      CubukCiz();
    }
    else if(bloktipi == 2) {
      ZblokCiz();
    }
    else if(bloktipi == 3) {
      LblokCiz();
    }
    else if(bloktipi == 4) {
      TblokCiz();
    }
  }
  else if(oyunDurumu == 2) {
    lcd.setCursor(15, 100);
    lcd.setTextSize(4);
    lcd.println("GAME OVER");
  }
}

void setup() {
  pinMode(ButtonA, INPUT);
  pinMode(ButtonB, INPUT);
  pinMode(ButtonC, INPUT);
  pinMode(ButtonD, INPUT);
  pinMode(ButtonE, INPUT);
  pinMode(ButtonF, INPUT);
  pinMode(JoyButton, INPUT);

  lcd.init(SCR_WIDTH, SCR_HEIGHT);
  lcd.fillScreen(GREEN);

  lcd.setTextColor(WHITE);  
  lcd.setTextSize(6);

  prevTime = 0;

  Serial.begin(9600);
}

void loop() {
  InputControl();  
  GameUpdate();
  RenderScreen();

  unsigned long time = millis();
  frameTime = time - prevTime;
  prevTime = time;

  randomSeed(prevTime);
}






