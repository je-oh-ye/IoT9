#include <BluetoothSerial.h>
#include <SPI.h>
#include <MFRC522.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

//Timer set
unsigned long previousMillis = 0;    // Stores last time temperature was published
const long interval = 5000;

const int RST_PIN = 22;
const int SS_PIN = 21; 
const int led1 = 32;
const int led2 = 33;

//Isi data
String pesan="";
 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

 
void setup() {
  Serial.begin(115200); // Initialize serial communications with the PC
  
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  
  while (!Serial); // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details

  SerialBT.begin("RFID Access"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

}
 
void loop() {

//RFID
// Look for new cards
if ( ! mfrc522.PICC_IsNewCardPresent()) {
  return;
}
 
// Select one of the cards
if ( ! mfrc522.PICC_ReadCardSerial()) {
  return;
}
 
// Dump debug info about the card; PICC_HaltA() is automatically called
//mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

String uid;
String temp;
for(int i=0;i<4;i++){
  if(mfrc522.uid.uidByte[i]<0x10){
    temp = "0" + String(mfrc522.uid.uidByte[i],HEX);
  }
  else temp = String(mfrc522.uid.uidByte[i],HEX);
  
  if(i==3){
    uid =  uid + temp;
  }
  else uid =  uid + temp+ " ";
}
Serial.println("UID "+uid);
String grantedAccess = "0a 19 86 19"; //Akses RFID yang ditunjuk
grantedAccess.toLowerCase();

//BLUETOOTH
  if (SerialBT.available()){
    char pesanMasuk = SerialBT.read();
    if (pesanMasuk != '\n'){
      pesan += String(pesanMasuk);}
    else{
      pesan = "";}
    Serial.write(pesanMasuk);  
  }


//Pengiriman data RFID via Bluetooth
  if (uid == grantedAccess) {
     digitalWrite(led1,HIGH);
     digitalWrite(led2,LOW);
     SerialBT.println("UID "+uid);
     SerialBT.println("Access Granted");
  }
  else{
    digitalWrite(led1,LOW);
    digitalWrite(led2,HIGH);
    SerialBT.println("UID "+uid);
    SerialBT.println("Access Denied");
  }
  Serial.println("\n");
  pesan = "";
  SerialBT.flush();
  mfrc522.PICC_HaltA();
  delay(3000);
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);

}
