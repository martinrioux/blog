/* Par Martin Rioux
 Version 1 de l'episode 1 de nautilus

Fonctions du code

Ouvrir le cadre a l'aide d'un boutton
Ouvrir la porte a l'aide d'un boutton
Terminer le jeu par 6 manette de gaz et un boutton
  -Envoyer signal de fin a ordinateur pas serie
  -Faire allumer des led pour simuler un bateau en
   redemarrage
*/

// ICI VARIABLES DE RESOLUTION EGNIMES
//position gas : 0 = max reverse,.., 4 = stop,.., 8 = max front
//
int gas11_solution = 0;
int gas12_solution = 1;
int gas21_solution = 2;
int gas22_solution = 3;
int gas31_solution = 4;
int gas32_solution = 5;

int computer_solved = false;

int open_monitor = 38;

int gasConfirmButton = 26;
int gasConfirmButtonLED = 46;
bool gas_solved = false;
int gas11 = A0;
int gas12 = A1;
int gas21 = A2;
int gas22 = A3;
int gas31 = A4;
int gas32 = A5;

long blink_or_error_timer;
bool waiting_error = false;
int start_button_blink_time = 1000;
int start_button_error_time = 10000;

int frame_btn = 24;
int frame_lock = 22;
int frameOpen = 0;

int seven_segs_cs1 = 27;
int seven_segs_cs2 = 28;

int seven_segs_a = 29;
int seven_segs_b = 30;
int seven_segs_c = 31;
int seven_segs_d = 32;
int seven_segs_e = 33;
int seven_segs_f = 34;
int seven_segs_g = 35;
int seven_segs_dp = 36;

int seven_segs_cs_scan = 0;
int seven_segs_code_index = 0;

long seven_segs_code_timer;
int seven_segs_time_on = 10;
int seven_segs_time_off = 250;
int seven_segs_code[9][2] = {{-1,-1}, {1,7}, {-1,-1}, {8,3}, {-1,-1}, {7,5}, {-1,-1}, {9,6}, {-1,-1}};
int segs_list[] = {seven_segs_a, seven_segs_b, seven_segs_c, seven_segs_d, 
                   seven_segs_e, seven_segs_f, seven_segs_g, seven_segs_dp
                  };
//                 a  b  c  d  e  f  g  dp
int display_off[] = {0, 0, 0, 0, 0, 0, 0, 0};
int display_0[] = {1, 1, 1, 1, 1, 1, 0, 0};
int display_1[] = {0, 1, 1, 0, 0, 0, 0, 0};
int display_2[] = {1, 1, 0, 1, 1, 0, 1, 0};
int display_3[] = {1, 1, 1, 1, 0, 0, 1, 0};
int display_4[] = {0, 1, 1, 0, 0, 1, 1, 0};
int display_5[] = {1, 0, 1, 1, 0, 1, 1, 0};
int display_6[] = {1, 0, 1, 1, 1, 1, 1, 0};
int display_7[] = {1, 1, 1, 0, 0, 0, 0, 0};
int display_8[] = {1, 1, 1, 1, 1, 1, 1, 0};
int display_9[] = {1, 1, 1, 1, 0, 1, 1, 0};


int started_leds = 52;
int started_scan_leds[] = {47, 48, 49, 50, 51};


int last_status_time;

String serial_data = ""; 
boolean serial_complete = false;

void setup() {
  Serial.begin(9600);
  last_status_time = millis();
  seven_segs_code_timer = millis();
  blink_or_error_timer = millis();
  pinMode(gasConfirmButton, INPUT);
  pinMode(gasConfirmButtonLED, OUTPUT);

  pinMode(frame_btn, INPUT);
  pinMode(frame_lock, OUTPUT);
  
  pinMode(open_monitor, OUTPUT);
  digitalWrite(open_monitor, HIGH);
  pinMode(seven_segs_cs1, OUTPUT);
  pinMode(seven_segs_cs2, OUTPUT);
  for (int i=0; i<7;i++){
    pinMode(segs_list[i], OUTPUT);  
  }
  for (int i=0; i<sizeof(started_scan_leds);i++){
    pinMode(started_scan_leds[i], OUTPUT);  
  }
  
  pinMode(started_leds, OUTPUT);
  digitalWrite(started_leds, HIGH);
}

void loop() {
  //printStatus();
  //printGasStatus();
  display_7_segs_code();
  segs_scan();
  check_frame();
  check_gas_handles();
  readSerial();
  useSerialData();
  gas_blink_and_error_delay();
  blink_on_leds();
  scan_on_leds();
}

void readSerial(){
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      serial_complete = true;
      //Serial.println(serial_data);
    } else{
      serial_data += inChar;
    Serial.println(inChar);
    }
    
  }
}
  
void useSerialData() {
  if (serial_complete) {
    Serial.print(serial_data);
    if (serial_data == "display_end_code"){
      computer_solved = true;
    }
    //if (serial_data[0] == 'G'){
     // gas11_solution = (int) serial_data[2] - 48;
     // gas12_solution = (int) serial_data[4] - 48;
     // gas21_solution = (int) serial_data[6] - 48;
     // gas22_solution = (int) serial_data[8] - 48;
     // gas31_solution = (int) serial_data[10] - 48;
     // gas32_solution = (int) serial_data[12] - 48;
     // Serial.println("GOT CODE");
    //}
    serial_data = "";
    serial_complete = false;
  }
}

void display_7_segs_code(){
  if (computer_solved) {
    bool next = false;
    if (seven_segs_code_index%2 == 0 && (((millis()/10) - (seven_segs_code_timer)) > (seven_segs_time_off))){
      next = true;
    } 
    if (seven_segs_code_index%2 == 1 && (((millis()/10) - (seven_segs_code_timer)) > (seven_segs_time_on))){
      next = true;
    }
    if (next){
      seven_segs_code_index++;
      seven_segs_code_timer = millis()/10;
      if (seven_segs_code_index >= 9){
        seven_segs_code_index = 0;
      }
    }
  } else {
    seven_segs_code_index = 0;
  }
}


void segs_scan(){
  seven_segs_cs_scan++;
  if (seven_segs_cs_scan >= 2){
    seven_segs_cs_scan = 0;
  }
  
  digitalWrite(seven_segs_cs1, LOW);
  digitalWrite(seven_segs_cs2, LOW);
  int list[7];
  if ((seven_segs_code[seven_segs_code_index][seven_segs_cs_scan]) == -1) memcpy( list, display_off, 7*sizeof(int) );
  if ((seven_segs_code[seven_segs_code_index][seven_segs_cs_scan]) == 0) memcpy( list, display_0, 7*sizeof(int) );
  if ((seven_segs_code[seven_segs_code_index][seven_segs_cs_scan]) == 1) memcpy( list, display_1, 7*sizeof(int) );
  if ((seven_segs_code[seven_segs_code_index][seven_segs_cs_scan]) == 2) memcpy( list, display_2, 7*sizeof(int) );
  if ((seven_segs_code[seven_segs_code_index][seven_segs_cs_scan]) == 3) memcpy( list, display_3, 7*sizeof(int) );
  if ((seven_segs_code[seven_segs_code_index][seven_segs_cs_scan]) == 4) memcpy( list, display_4, 7*sizeof(int) );
  if ((seven_segs_code[seven_segs_code_index][seven_segs_cs_scan]) == 5) memcpy( list, display_5, 7*sizeof(int) );
  if ((seven_segs_code[seven_segs_code_index][seven_segs_cs_scan]) == 6) memcpy( list, display_6, 7*sizeof(int) );
  if ((seven_segs_code[seven_segs_code_index][seven_segs_cs_scan]) == 7) memcpy( list, display_7, 7*sizeof(int) );
  if ((seven_segs_code[seven_segs_code_index][seven_segs_cs_scan]) == 8) memcpy( list, display_8, 7*sizeof(int) );
  if ((seven_segs_code[seven_segs_code_index][seven_segs_cs_scan]) == 9) memcpy( list, display_9, 7*sizeof(int) );
  
  for (int i=0;i<7;i++){
    digitalWrite(segs_list[i], !list[i]);
  }
  if (seven_segs_cs_scan == 0){
    digitalWrite(seven_segs_cs1, HIGH);
    digitalWrite(seven_segs_cs2, LOW);
  } else {
    digitalWrite(seven_segs_cs1, LOW);
    digitalWrite(seven_segs_cs2, HIGH);  
  }

}

void printGasStatus(){
  Serial.print((String)convert_gas_position(analogRead(gas11)));  
  Serial.print((String)convert_gas_position(analogRead(gas12)));  
  Serial.print((String)convert_gas_position(analogRead(gas21)));  
  Serial.print((String)convert_gas_position(analogRead(gas22)));  
  Serial.print((String)convert_gas_position(analogRead(gas31)));  
  Serial.println((String)convert_gas_position(analogRead(gas32)));  
}

void printStatus(){
  if (millis() > (last_status_time + 3000)){
    Serial.println("-------------------STATUS START-------------------");  
    Serial.print("Gas11: " + (String)analogRead(gas11) + ", ");  
    Serial.print("Gas12: " + (String)analogRead(gas12) + ", ");  
    Serial.print("Gas21: " + (String)analogRead(gas21) + ", ");  
    Serial.print("Gas22: " + (String)analogRead(gas22) + ", ");  
    Serial.print("Gas31: " + (String)analogRead(gas31) + ", ");  
    Serial.println("Gas32: " + (String)analogRead(gas32) + ", ");  
    Serial.println("Frame button: " + (String)digitalRead(frame_btn));  
    Serial.println("Frame Open: " + (String)digitalRead(frame_lock));   
    Serial.println("End Game Button: " + (String)digitalRead(gasConfirmButton));  
    Serial.println("-------------------STATUS END-------------------");  
    last_status_time = millis();
  }
}

void check_frame(){
  if (digitalRead(frame_btn) == LOW){
    if (frameOpen) return;
    digitalWrite(frame_lock, LOW);
    delay(200);
    digitalWrite(frame_lock, HIGH);
    frameOpen = 1;
  } else {
    frameOpen = 0;
    digitalWrite(frame_lock, HIGH);
  }
}

void gas_blink_and_error_delay(){
    if (gas_solved){
      digitalWrite(gasConfirmButtonLED, HIGH);
      return;
    }
    if (waiting_error){
        if (millis() - blink_or_error_timer > start_button_error_time){
            waiting_error = false;
            blink_or_error_timer = millis();
    }
        digitalWrite(gasConfirmButtonLED, LOW);
    } else {
        if ((millis() - blink_or_error_timer > start_button_blink_time)){
            digitalWrite(gasConfirmButtonLED, !digitalRead(gasConfirmButtonLED));
            blink_or_error_timer = millis();
        }
    }
}

void check_gas_handles(){
  if (gas_solved) return;
  if (digitalRead(gasConfirmButton) == LOW && !waiting_error) {
    int solved = 1;
    if (convert_gas_position(analogRead(gas11)) != gas11_solution) solved = 0;
    if (convert_gas_position(analogRead(gas12)) != gas12_solution) solved = 0;
    if (convert_gas_position(analogRead(gas21)) != gas21_solution) solved = 0;
    if (convert_gas_position(analogRead(gas22)) != gas22_solution) solved = 0;
    if (convert_gas_position(analogRead(gas31)) != gas31_solution) solved = 0;
    if (convert_gas_position(analogRead(gas32)) != gas32_solution) solved = 0;
    if (solved){
      Serial.println("THEY GOT THE GAS");
      digitalWrite(open_monitor, LOW);
      gas_solved = true;
    } else {
      //Serial.println("THEY GOT THE GAS");
      //digitalWrite(open_monitor, LOW);
      waiting_error = true;
      Serial.println("GAS ERROR");
     }
  }
}

int confirm_position(int gas, int solution){
  if (convert_gas_position(analogRead(gas)) == solution){
    return 1;
  }
  return 0;
}

void blink_on_leds(){
  if (computer_solved){
    digitalWrite(started_leds,((millis()/1000)%2 == 0));
  } else {
    digitalWrite(started_leds,HIGH);
  } 
}

void scan_on_leds(){
  if (computer_solved){
    for (int i=0; i<sizeof(started_scan_leds);i++){
      //Serial.println((millis()/1000)%7);
      digitalWrite(started_scan_leds[4-i],!(((millis()/500)%7 == i) || (millis()/500)%7 == i+1));
    }
   } else {
    for (int i=0; i<sizeof(started_scan_leds);i++){
      digitalWrite(started_scan_leds[4-i],HIGH);
    }    
   }
}

int convert_gas_position(int gas){
  if (gas > 955) return 8;
  if (gas > 828) return 7;
  if (gas > 707) return 6;
  if (gas > 577) return 5;
  if (gas > 449) return 4;
  if (gas > 318) return 3;
  if (gas > 195) return 2;
  if (gas > 68) return 1;
  return 0;
}

