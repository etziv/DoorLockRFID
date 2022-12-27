#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#define BLYNK_TEMPLATE_ID "TMPLaN0MLKRy"
#define BLYNK_DEVICE_NAME "IoTN2"
#define BLYNK_AUTH_TOKEN "EXo_TtbCnRprkirUvOg8gl48CXjAS73e"
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <MFRC522.h> // import file MFRC522 yang sudah kita install
#include <Servo.h> 
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = BLYNK_AUTH_TOKEN; 
char ssid[] = "WIFI ID"; 
char pass[] = "qazplmvvgg"; 

//RFID
#define SDA_PIN D4 // mendefinisikan SDA pin ke D3
#define RST_PIN D3 // mendefinisikan RST pin ke D4
#define motor D0

int buzz = D8;

Servo servo;

MFRC522 mfrc522(SDA_PIN, RST_PIN); // setting mfrc dengan SDA dan RST

void setup() 
{
  Serial.begin(115200);
  SPI.begin();      // Initiate  SPI bus
  Blynk.begin(auth, ssid, pass);
  Wire.begin();
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  mfrc522.PCD_Init(); // Inisiasi MFRC522 card
  pinMode(motor, OUTPUT);
  Serial.println("Approximate your card to the reader..."); // pesan untuk mentap kartu
  Serial.println();
  lcd.print("Tap Your Card");
  pinMode(buzz, OUTPUT);
  servo.attach(motor);
  servo.write(0); 
}
       
void loop()      
{
  Blynk.run();
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  // menampilkan UID di serial Monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;

  // berfungsi untuk mengubah ID dari Kartu menjadi kode Hexa seperti 04 EA F0 2B
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();  // UID dijadikan Huruf besar semua
    
  if(content.substring(1) == "6D 0D 56 17") // masukkan ID kartu / E-KTP disini
  {
    buzzOpen();
    Blynk.virtualWrite(V9, "Access Granted");      // Menampilkan pesan ditolak di LCD blynk
    Blynk.virtualWrite(V10, content);
    lcdTrue();
    delay(3000);
    Blynk.virtualWrite(V9, "Welcome");
    Blynk.virtualWrite(V10, "Blynker");
    // fungsi untuk menggerakkan servo         
  } 
  else
  {
    buzzClose();           
    Blynk.virtualWrite(V9, "Invalid card");      // Menampilkan pesan ditolak di LCD blynk
    Blynk.virtualWrite(V10, "Access Denied");
    lcdFalse();
    delay(3000);
    Blynk.virtualWrite(V9, "Welcome");
    Blynk.virtualWrite(V10, "Blynker");
  }                           
                 
          
}   

// membuat fungsi untuk menggerakkan servo
void buka(){ 
  servo.write(90); 
  delay(5000);
  servo.write(0);
}

void buzzOpen(){
  digitalWrite(buzz, HIGH);
  delay(500);
  digitalWrite(buzz, LOW);
}

void buzzClose(){
  digitalWrite(buzz, HIGH);
  delay(100);
  digitalWrite(buzz, LOW);
  delay(100);
  digitalWrite(buzz, HIGH);
  delay(100);
  digitalWrite(buzz, LOW);
  delay(1000);
}

void lcdTrue(){
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Access Granted");
    lcd.setCursor(1, 1);
    lcd.print("Silahkan Masuk");
    buka();
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tap Your Card"); 
}

void lcdFalse(){
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Access Denied");
    lcd.setCursor(0, 1);
    lcd.print("ID Not Registed");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tap Your Card"); 
}

BLYNK_WRITE(V2)
{
  servo.write(90); 
  Blynk.virtualWrite(V9, "Door Opened");
  Blynk.virtualWrite(V10, "Welcome Master");
  delay(3000);
  servo.write(0);
  Blynk.virtualWrite(V9, "Door Closed");
  Blynk.virtualWrite(V10, "");
  delay(1000);
  Blynk.virtualWrite(V9, "Welcome");
  Blynk.virtualWrite(V10, "Blynker");
  Blynk.virtualWrite(V2, 0);
}
