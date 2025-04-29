#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Khai báo cảm biến
#define CAM_BIEN_KHOI 2
#define CAM_BIEN_CHAY 3
#define MAY_BOM 8
#define QUAT 9
#define COI 13 // Còi chân D13

Servo servo1; // Servo gắn D6


LiquidCrystal_I2C lcd(0x27, 16, 2); // Địa chỉ I2C thường là 0x27

String SDT = "0902937006"; // Số điện thoại cảnh báo
String feedback;
bool daCanhBao = false;
bool servoDaQuay = false;

void setup() {
  Serial.begin(9600);

  pinMode(CAM_BIEN_KHOI, INPUT);
  pinMode(CAM_BIEN_CHAY, INPUT);
  pinMode(MAY_BOM, OUTPUT);
  pinMode(QUAT, OUTPUT);
  pinMode(COI, OUTPUT);

  servo1.attach(6);

  servo1.write(25);


  digitalWrite(MAY_BOM, LOW);
  digitalWrite(QUAT, LOW);
  noTone(COI);

  // Khởi động LCD
  lcd.init();
  lcd.backlight();
  hienThiLCD("System Ready", "");

  Serial.println("Waiting for 4G Signal on 20s...");
  hienThiLCD("Please wait...", "Dang ket noi..."); // <-- thêm dòng này

  delay(15000);

  SIMsetup();
  delay(10000);

  hienThiLCD("System Ready", ""); // <-- sau khi setup xong thì quay lại System Ready
}

void loop() {
  bool khoi = digitalRead(CAM_BIEN_KHOI) == LOW;
  bool chay = digitalRead(CAM_BIEN_CHAY) == LOW;

  String thongBao = "";

  if (khoi) {
    thongBao = "Phat hien KHOI!";
  } else if (chay) {
    thongBao = "Phat hien LUA!";
  } else {
    thongBao = "System Ready";
  }

  Serial.println(thongBao);
  hienThiLCD(thongBao, "");

  if (!daCanhBao) {
    if (khoi) {
      batThietBi();
      quayServo();
      daCanhBao = true;
      guiSMS("Canh bao: Co KHOI!");
      delay(2000);
      goiDien();
    } 
    else if (chay) {
      batThietBi();
      quayServo();
      daCanhBao = true;
      guiSMS("Canh bao: Co LUA!");
      delay(2000);
      goiDien();
    }
  } else {
    if (!khoi && !chay) {
      daCanhBao = false;
      servoDaQuay = false;
      tatThietBi();
      servo1.write(25);
 
      delay(2000);
    }
  }

  SIMfeedback();
  delay(500);
}

// ==== Các hàm chức năng =====

void quayServo() {
  if (!servoDaQuay) {
    servo1.write(90);
 
    servoDaQuay = true;
    Serial.println("Da quay servo1 va servo2.");
  }
}

void batThietBi() {
  digitalWrite(MAY_BOM, HIGH);
  digitalWrite(QUAT, HIGH);
  tone(COI, 1000); // Phát còi
}

void tatThietBi() {
  digitalWrite(MAY_BOM, LOW);
  digitalWrite(QUAT, LOW);
  noTone(COI);
}

void hienThiLCD(String dong1, String dong2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(dong1);
  lcd.setCursor(0, 1);
  lcd.print(dong2);
}

void SIMfeedback() {
  for (int b = 0; b < 1000; b++) {
    feedback = "";
    delay(1);
    if (Serial.available() > 0) {
      delay(500);
      feedback = "";
      while (Serial.available()) {
        char inChar = (char)Serial.read();
        feedback += inChar;
      }
      delay(100);
      b = 1000;
      Serial.println(">>");
      Serial.println(feedback);
    }
  }
}

void SIMsetup() {
  Serial.print("AT\r\n");
  delay(3000);
  SIMfeedback();

  Serial.print("ATE0\r\n");
  delay(3000);
  SIMfeedback();

  Serial.print("AT+CSCS=\"GSM\"\r\n");
  delay(3000);
  SIMfeedback();

  Serial.print("AT+CMGF=1\r\n");
  delay(3000);
  SIMfeedback();

  Serial.print("AT+CNMI=2,2,0,0,0\r\n");
  delay(3000);
  SIMfeedback();

  Serial.print("AT+CMGD=1\r\n");
  delay(3000);
  SIMfeedback();

  Serial.print("AT+CLIP=1\r\n");
  delay(3000);
  SIMfeedback();

  Serial.print("AT&W\r\n");
  delay(3000);
  SIMfeedback();

  Serial.print("AT+CSQ\r\n");
  delay(3000);
  SIMfeedback();
}

void goiDien() {
  Serial.print("ATD");
  Serial.print(SDT);
  Serial.println(";");
  delay(2000);
  SIMfeedback();
  delay(10000);
  Serial.println("AT+CHUP");
  delay(2000);
}

void guiSMS(String noidung) {
  Serial.print("AT+CMGS=\"" + SDT + "\"\r");
  delay(100);
  Serial.print(noidung);
  delay(100);
  Serial.println((char)26);
  delay(1000);
}
