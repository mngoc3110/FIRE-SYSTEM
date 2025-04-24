// --- Khai báo chân cảm biến ---
#define MQ2_PHONG1 2    // D2 - Cảm biến khói phòng 1
#define MQ2_PHONG2 3    // D3 - Cảm biến khói phòng 2
#define LUA_PHONG1 4    // D4 - Cảm biến lửa phòng 1
#define LUA_PHONG2 5    // D5 - Cảm biến lửa phòng 2

String SDT = "0902937006"; // Số điện thoại nhận cảnh báo
String feedback;

// Cờ trạng thái
bool sentKhoi1 = false, sentKhoi2 = false;
bool sentLua1 = false, sentLua2 = false;

void setup() {
  pinMode(MQ2_PHONG1, INPUT);
  pinMode(MQ2_PHONG2, INPUT);
  pinMode(LUA_PHONG1, INPUT);
  pinMode(LUA_PHONG2, INPUT);

  Serial.begin(9600);
  delay(15000);  // Đợi module SIM khởi động

  SIMsetup();
  delay(10000);
}

void loop() {
  bool khoi1 = digitalRead(MQ2_PHONG1) == HIGH;
  bool khoi2 = digitalRead(MQ2_PHONG2) == HIGH;
  bool lua1  = digitalRead(LUA_PHONG1) == HIGH;
  bool lua2  = digitalRead(LUA_PHONG2) == HIGH;

  // Xử lý khói phòng 1
  if (khoi1 && !sentKhoi1) {
    sendSMS("CẢNH BÁO: Có KHÓI tại PHÒNG 1!");
    sentKhoi1 = true;
  }
  if (!khoi1) sentKhoi1 = false;

  // Xử lý khói phòng 2
  if (khoi2 && !sentKhoi2) {
    sendSMS("CẢNH BÁO: Có KHÓI tại PHÒNG 2!");
    sentKhoi2 = true;
  }
  if (!khoi2) sentKhoi2 = false;

  // Xử lý lửa phòng 1
  if (lua1 && !sentLua1) {
    sendSMS("CẢNH BÁO KHẨN: PHÁT HIỆN LỬA tại PHÒNG 1!");
    callPhone();
    sentLua1 = true;
  }
  if (!lua1) sentLua1 = false;

  // Xử lý lửa phòng 2
  if (lua2 && !sentLua2) {
    sendSMS("CẢNH BÁO KHẨN: PHÁT HIỆN LỬA tại PHÒNG 2!");
    callPhone();
    sentLua2 = true;
  }
  if (!lua2) sentLua2 = false;

  delay(1000);
}

// --- Hàm gửi tin nhắn SMS ---
void sendSMS(String msg) {
  Serial.print("AT+CMGS=\"" + SDT + "\"\r");
  delay(500);
  Serial.print(msg);
  delay(500);
  Serial.write(26);  // Ctrl+Z
  delay(1000);
  SIMfeedback();
}

// --- Hàm gọi điện thoại ---
void callPhone() {
  Serial.println("ATD" + SDT + ";");
  delay(20000);  // Gọi trong 20 giây
  Serial.println("ATH"); // Tắt cuộc gọi
  SIMfeedback();
}

// --- Khởi tạo SIM768x ---
void SIMsetup() {
  Serial.println("AT"); delay(1000); SIMfeedback();
  Serial.println("ATE0"); delay(1000); SIMfeedback();
  Serial.println("AT+CSCS=\"GSM\""); delay(1000); SIMfeedback();
  Serial.println("AT+CMGF=1"); delay(1000); SIMfeedback();
  Serial.println("AT+CNMI=2,2,0,0,0"); delay(1000); SIMfeedback();
  Serial.println("AT+CLIP=1"); delay(1000); SIMfeedback();
  Serial.println("AT&W"); delay(1000); SIMfeedback();
  Serial.println("AT+CSQ"); delay(1000); SIMfeedback();
}

// --- Đọc phản hồi từ module SIM ---
void SIMfeedback() {
  feedback = "";
  delay(200);
  while (Serial.available()) {
    char c = Serial.read();
    feedback += c;
  }
  if (feedback.length() > 0) {
    Serial.println(">>");
    Serial.println(feedback);
  }
}