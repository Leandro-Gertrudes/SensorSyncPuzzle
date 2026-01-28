#include <LedControl.h>
#include <LiquidCrystal.h>

#define TRIG_PIN 13
#define ECHO_PIN A1
#define LDR_PIN A0  

// DIN=11, CLK=13, CS=10, 1 dispositivo
LedControl dis = LedControl(2, 3, 4, 1);
LedControl luz = LedControl(5, 6, 7, 1);

//RS, E, D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

long		distancia;
int 		leituraLuz;
int     goalDis;
int     goalLuz;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  dis.shutdown(0, false);   
  dis.setIntensity(0, 8);   
  dis.clearDisplay(0);      

  luz.shutdown(0, false);   
  luz.setIntensity(0, 8);   
  luz.clearDisplay(0);      

  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {
  sensor_presenca();
  sensor_luz();
  checker();
  delay(100);
}

//--LUZ--//
void sensor_luz() {
  leituraLuz = analogRead(LDR_PIN);  // 0 a 1023
  ledsLuz();

  Serial.print("Luminosidade: ");
  Serial.println(leituraLuz);

  if(leituraLuz >= 220 && leituraLuz < 300)
    goalLuz = 1;
  else
    goalLuz = 0;
}

//--DISTANCIA--//
void sensor_presenca() {
  long duracao;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duracao  = pulseIn(ECHO_PIN, HIGH);
  distancia = duracao * 0.01715;
  ledsDis();

  //Serial.print("Distância: ");
  //Serial.println(distancia);
  //Serial.println(" cm");

  if(distancia <= 18 && distancia > 15)
    goalDis = 1;
  else
    goalDis = 0;
}

//--LEDS--//
byte estadoDis[8] = {0,0,0,0,0,0,0,0};
byte estadoLuz[8] = {0,0,0,0,0,0,0,0};

void ledsDis()
{
  byte ledState[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  if(distancia <= 27)
    ledState[0] = B11111111;
  if(distancia <= 24)
    ledState[1] = B11111111;
  if(distancia <= 21)
    ledState[2] = B11111111;
  if(distancia <= 18)
    ledState[3] = B11111111;
  if(distancia <= 15)
    ledState[4] = B11111111;
  if(distancia <= 12)
    ledState[5] = B11111111;
  if(distancia <= 9)
    ledState[6] = B11111111;
  if(distancia <= 5)
    ledState[7] = B11111111;

  // Atualiza só as colunas que mudaram
  for (int i = 0; i < 8; i++) {
    if (estadoDis[i] != ledState[i]) {
      dis.setColumn(0, i, ledState[i]);
      estadoDis[i] = ledState[i];  // Atualiza o estado salvo
    }
  }
}

void ledsLuz()
{
  byte ledState[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  if(leituraLuz >= 470)
    ledState[7] = B11111111;
  if(leituraLuz >= 460)
    ledState[6] = B11111111;
  if(leituraLuz >= 430)
    ledState[5] = B11111111;
  if(leituraLuz >= 400)
    ledState[4] = B11111111;
  if(leituraLuz >= 350)
    ledState[3] = B11111111;
  if(leituraLuz >= 300)
    ledState[2] = B11111111;
  if(leituraLuz >= 220)
    ledState[1] = B11111111;
  if(leituraLuz >= 180)
    ledState[0] = B11111111;

  // Atualiza só as colunas que mudaram
  for (int i = 0; i < 8; i++) {
    if (estadoLuz[i] != ledState[i]) {
      luz.setColumn(0, i, ledState[i]);
      estadoLuz[i] = ledState[i];  // Atualiza o estado salvo
    }
  }
}


void checker()
{
	if(goal())
     {
       int tempo = 0;
       while(tempo < 2000)
       {
       	delay(100);
         sensor_presenca();
		    sensor_luz();
         tempo += 100;
         if(!goal())
         {
           return ;
         }
       }
      if(tempo == 2000)
      {
        digitalWrite(TRIG_PIN, LOW);
        Serial.println("###################");
        Serial.println("#### GOOD JOB! ####");
        Serial.println("###################");
        Serial.println("Password: ATmega328Pp");
        delay(100);
        displayOk();
        showPassword("ATmega328P");
        while (1);
      }
     }
}
       
int goal()
{
	if(goalDis && goalLuz)
    {
      return 1;
    }
    return 0;
}

void  displayOk()
{
  luz.clearDisplay(0);
  dis.clearDisplay(0);

  luz.setColumn(0, 7, B00000000);
  luz.setColumn(0, 6, B01111100);
  luz.setColumn(0, 5, B01000100);
  luz.setColumn(0, 4, B01000100);
  luz.setColumn(0, 3, B01000100);
  luz.setColumn(0, 2, B01000100);
  luz.setColumn(0, 1, B01111100);
  luz.setColumn(0, 0, B00000000);

  dis.setColumn(0, 7, B00000000);
  dis.setColumn(0, 6, B00100100);
  dis.setColumn(0, 5, B00101000);
  dis.setColumn(0, 4, B00110000);
  dis.setColumn(0, 3, B00101000);
  dis.setColumn(0, 2, B00100100);
  dis.setColumn(0, 1, B00100010);
  dis.setColumn(0, 0, B00000000);
}


void showPassword(String senha) {
  int len = senha.length();
  int col = max(0, (16 - len) / 2); // centraliza na tela

   lcd.clear();
  lcd.setCursor(col, 0);
   lcd.print("Your code:");
  lcd.setCursor(col, 1); // linha 2 (índice 1)

  for (int i = 0; i < len; i++) {
    // antes de revelar a letra certa, mostra caracteres aleatórios
    for (int j = 0; j < 5; j++) {
      char c = char(random(33, 126)); // caracteres ASCII visíveis
      lcd.setCursor(col + i, 1);
      lcd.print(c);
      delay(40);
    }

    // mostra a letra correta
    lcd.setCursor(col + i, 1);
    lcd.print(senha[i]);
    delay(100);
  }

  // opcional: pisca a senha completa no final
  for (int k = 0; k < 3; k++) {
    delay(200);
    lcd.setCursor(col, 1);
    for (int i = 0; i < len; i++) lcd.print(' ');
    delay(150);
    lcd.setCursor(col, 1);
    lcd.print(senha);
  
  }

  // mostra senha final estável
  lcd.setCursor(col, 1);
  lcd.print(senha);
  delay(200);
  }

