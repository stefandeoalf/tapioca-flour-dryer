/*
 * AHT10 : berhasil
 * LCD : berhasil
 * Relay : sudah bisa bunyi, belum di tes tegangan
 * Humidity 1 : sudah work, perlu recalibrate
 * Humidity 2 : sudah work, perlu di calibrate
 * Servo : sudah work
 */


#include <Adafruit_AHT10.h>
#include <Servo.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

Adafruit_AHT10 aht;

LiquidCrystal_I2C lcd(0x27,16,2);

float rtemp;
float rhumid;

#define heater 2        // relay lampu pemanas di pin 2
#define fan 3           // transistor fan control pin 3

const int wet1 = 200;    // perlu dikalibrasi
const int dry1 = 1023;
const int wet2 = 200;    // perlu dikalibrasi
const int dry2 = 1023;

int m11=50;
int m12=50;
int m13=50;
int m14=50;
int m15=50;
int m16=50;
int m21=50;
int m22=50;
int m23=50;
int m24=50;
int m25=50;
int m26=50;

Servo servo;

void setup()
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  aht.begin();
/*
  if (! aht.begin()) {
    Serial.println("Could not find AHT10? Check wiring");
    while (1) delay(10);
  }
  */
  //Serial.println("AHT10 found");

  lcd.setCursor(0,0);
  lcd.print("    TTG RAS");
  lcd.setCursor(0,1);
  lcd.print("    FTMD ITB");
  
  pinMode(fan, OUTPUT);           // set fan as output
  analogWrite(fan,255);         // initial fan on
  pinMode(heater, OUTPUT);        // set heater as output
  digitalWrite(heater,LOW);       // initial heater off
  servo.attach(8);                // servo control pin 8
  servo.write(90);                 // initial servo at 0 deg
  delay(2000);
}

void loop()
{
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  rtemp = temp.temperature;
  rhumid = humidity.relative_humidity;
  int moists1=analogRead(A0);  // read moisture sensor 1 value at pin A0
  int moist11 = map(moists1, wet1, dry1, 100, 0);    // map the sensor 1 value to 0 to 100 %
  int moists2=analogRead(A1);  // read moisture sensor 2 value at pin A1
  int moist22 = map(moists2, wet2, dry2, 100, 0);

  m11 = m12;
  m12 = m13;
  m13 = m14;
  m14 = m15;
  m15 = m16;
  m16 = moist11;
  int moist1 = (m11+m12+m13+m14+m15+m16)/6;
  
  m21 = m22;
  m22 = m23;
  m23 = m24;
  m24 = m25;
  m25 = m26;
  m26 = moist22;
  int moist2 = (m21+m22+m23+m24+m25+m26)/6;

  
  
  // Monitor
  Serial.print("Humidity (%): ");
  Serial.println(rhumid);
  Serial.print("Temperature (C): ");
  Serial.println(rtemp);
  Serial.print("Flour Moisture 1 (%): ");
  Serial.println(moist1);
  Serial.print("Flour Moisture 2 (%): ");
  Serial.println(moist2);

  lcd.clear();
  if(rtemp<32.0){                 // if the room temperature <32 centigrade
    digitalWrite(heater,HIGH);    // turn on the heater lamp
  }else if (rtemp>35.0){
      digitalWrite(heater,LOW);   // turn off the heater lamp
  }
  if(rhumid<50){        // FAN
    analogWrite(fan,0);
  }else if (rhumid<60){
    analogWrite(fan,63);
  }else if (rhumid<70){
    analogWrite(fan,127);
  }else if (rhumid<80){
    analogWrite(fan,191);
  }else{
    analogWrite(fan,255);
  }
  if(moist1<=1 && moist1>=-1 && moist2<=1 && moist2>=-1){     // if the moisture of tepung <= 3% 
    analogWrite(fan,0);        // turn off the fan
    digitalWrite(heater,LOW);     // turn off the heater
    servo.write(0);              // indicator on
    lcd.setCursor(0,0);
    lcd.print("! SUDAH KERING !");
    lcd.setCursor(0,1);
    lcd.print("Tepung:");
    lcd.setCursor(7,1);
    lcd.print(moist1);
    lcd.setCursor(10,1);
    lcd.print("%");
    lcd.setCursor(12,1);
    lcd.print(moist2);
    lcd.setCursor(15,1);
    lcd.print("%");
  }else{                          // if the tepung's moisture suddenly goes up >5%
  servo.write(90);               // indicator off
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Tepung:");
  lcd.setCursor(7,0);
  lcd.print(moist1);
  lcd.setCursor(10,0);
  lcd.print("%");
  lcd.setCursor(12,0);
  lcd.print(moist2);
  lcd.setCursor(15,0);
  lcd.print("%");
  
  lcd.setCursor(0,1);
  lcd.print("T");
  lcd.setCursor(1,1);
  lcd.print(rtemp);
  lcd.setCursor(6,1);
  lcd.print("C");
  lcd.setCursor(9,1);
  lcd.print("RH");
  lcd.setCursor(11,1);
  lcd.print(rhumid);
  lcd.setCursor(15,1);
  lcd.print("%");
  }
  delay(5000);                   // check every 5 seconds
}
