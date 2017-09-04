#include <Bridge.h>
#include <string.h>
#include <Process.h>
#include "FSR.h"

const char SEND= '1';
const char RESET= '2';
const char EARS='3';
const char FACE_light='4';
const char FACE_heavy='5';
const char HANDS='6';
const char BELLY_light='7';
const char BELLY_heavy='8';
const char FEET='9';

const int EARS_Sensor = 1;
const int FACE_Sensor = 3;
const int HANDS_Sensor = 4;
const int BELLY_Sensor = 6;
const int FEET_Sensor = 7 ;

const int Num_sensors = 9;  //number of fsr sensors
int Pin_sensors[Num_sensors]={A0,A1,A2,A3,A4,A5,A6,A7,A8}; //pin of fsr sensors A6 pin4 A7 pin6 A8 pin8 
char Key_sensors[Num_sensors][2]={  //every key sensors
  { SEND , RESET },
  { EARS , EARS },
  { EARS , EARS },
  { FACE_light , FACE_heavy },
  { HANDS, HANDS },
  { HANDS, HANDS },
  { BELLY_light , BELLY_heavy },
  { FEET, FEET },
  { FEET, FEET },
  };

char Trans_Key_sensors[Num_sensors][2];

Pressure fsr(Num_sensors,Pin_sensors,(char*)Key_sensors);



const int buffersize= 50;

char message[buffersize] ; // melody 
char r_message[buffersize];

const int ioTtalkPin = 13;
const int questionPin = 12;
const int answerPin = 11;

int index=0;
int r_index=0;
int blink_state= 0 ;

long previousMillis =0; //blink no delay
long interval= 300;

int system_regist_flag=0; 
// iottalk

char pin13[2];
char valueStr[buffersize];
int valueInt;

char incomming[2]={'\0'};
int previous_incomming_SystemMsg_O=999;
int previous_incomming_Ears_O=999;
int previous_incomming_Face_O=999;
int previous_incomming_Hands_O=999;
int previous_incomming_Belly_O=999;
int previous_incomming_Feet_O=999;

char outcome[4];
int outcomming_SystemMsg_I=0;
int outcomming_Ears_I=0;
int outcomming_Face_I=0;
int outcomming_Hands_I=0;
int outcomming_Belly_I=0;
int outcomming_Feet_I=0;

int have_Q = 0 ;
int state = 2 ;

char q_time[buffersize]="R"; //if no initial will error


String getTime() { //ask question id
  String result;
  Process time;
  // date is a command line utility to get the date and the time
  // in different formats depending on the additional parameter
  time.begin("date");
  time.addParameter("+%s");  // parameters: D for the complete date mm/dd/yy
  //             T for the time hh:mm:ss
  time.run();  // run the command
  Process macaddr;
  macaddr.begin("cat");
  macaddr.addParameter("/sys/class/net/eth0/address");
  macaddr.run();

  while (time.available() > 0) {
    char c = time.read();
    if (c != '\n') {
      result += c;
    }
  }
   result += '_';
  // read the output of the command
  while (macaddr.available() > 0) {
    char c = macaddr.read();
    if (c != '\n') {
      result += c;
    }
  }
  Serial.println(result);
  return result;
}
void music (String key,String parameter=""){ 
  Process p ;
  p.begin("/usr/bin/killall");
  p.addParameter("-9");
  p.addParameter("madplay");
  p.run();
  delay(10);
  p.runShellCommandAsynchronously("/usr/bin/madplay /root/music/"+ key +".mp3 "+ parameter);
  delay(50);
}
void play_music(String str,String volume="0"){ // --amplify= loudly --attenuate= smaller
     music(str,"--attenuate="+volume );
  }
void play_music(char key){
  String str(key);
  //Serial.println(key);
  switch (key){
    case EARS :music("EARS","--attenuate=-20");break;
    case FACE_light : music("FACE_light","--attenuate=-20");break;
    case FACE_heavy : music("FACE_heavy","--attenuate=-20");break;
    case HANDS : music("HANDS","--attenuate=-20");break;
    case BELLY_light : music("BELLY_light","--attenuate=-20"); break;
    case BELLY_heavy : music("BELLY_heavy","-20");break;
    case FEET : music("FEET","--attenuate=-20"); break;
    }
  }
void led_alarm(){
   play_music("fail","-30");
   r_index=0;
  }
void blink(int PIN ){
  if ((millis() - previousMillis ) >= interval ) {
    previousMillis = millis();
    digitalWrite(PIN, !digitalRead(PIN));
  }
}
void Translator_I()
{ 
  itoa(EARS_Sensor, valueStr, 10); 
  Bridge.put("Ears_I", valueStr );
  itoa( (outcomming_Ears_I=outcomming_Ears_I^1), outcome, 10);
  Bridge.put("outcomming_Ears_I", outcome);
  itoa(FACE_Sensor, valueStr, 10); 
  Bridge.put("Face_I", valueStr );
  itoa( (outcomming_Face_I=outcomming_Face_I^1), outcome, 10);
  Bridge.put("outcomming_Face_I", outcome);
  itoa(HANDS_Sensor, valueStr, 10); 
  Bridge.put("Hands_I", valueStr );
  itoa( (outcomming_Hands_I=outcomming_Hands_I^1), outcome, 10);
  Bridge.put("outcomming_Hands_I", outcome);
  itoa(BELLY_Sensor, valueStr, 10); 
  Bridge.put("Belly_I", valueStr );
  itoa( (outcomming_Belly_I=outcomming_Belly_I^1), outcome, 10);
  Bridge.put("outcomming_Belly_I", outcome);
  itoa(FEET_Sensor, valueStr, 10); 
  Bridge.put("Feet_I", valueStr );
  itoa( (outcomming_Feet_I=outcomming_Feet_I^1), outcome, 10);
  Bridge.put("outcomming_Feet_I", outcome);
}
void Translator_O()
{ 
  memcpy(Trans_Key_sensors, Key_sensors , 2*sizeof(char));

  Bridge.get("incomming_Ears_O",  incomming, 2); 
  if (atoi(incomming)!=previous_incomming_Ears_O){
  previous_incomming_Ears_O=atoi(incomming);
  Bridge.get("Ears_O",  valueStr, buffersize);
  memcpy(Trans_Key_sensors[EARS_Sensor], Key_sensors[atoi(valueStr)] , 2*sizeof(char));
  memcpy(Trans_Key_sensors[EARS_Sensor+1], Key_sensors[atoi(valueStr)] , 2*sizeof(char));

}
  Bridge.get("incomming_Face_O",  incomming, 2); 
  if (atoi(incomming)!=previous_incomming_Face_O){
  previous_incomming_Face_O=atoi(incomming);
  Bridge.get("Face_O",  valueStr, buffersize);
  memcpy(Trans_Key_sensors[FACE_Sensor], Key_sensors[atoi(valueStr)] , 2*sizeof(char));
}
  Bridge.get("incomming_Hands_O",  incomming, 2);
  if (atoi(incomming)!=previous_incomming_Hands_O){
  previous_incomming_Hands_O=atoi(incomming);
  Bridge.get("Hands_O",  valueStr, buffersize);
  memcpy(Trans_Key_sensors[HANDS_Sensor], Key_sensors[atoi(valueStr)] , 2*sizeof(char));
  memcpy(Trans_Key_sensors[HANDS_Sensor+1], Key_sensors[atoi(valueStr)] , 2*sizeof(char));
}
  Bridge.get("incomming_Belly_O",  incomming, 2); 
  if (atoi(incomming)!=previous_incomming_Belly_O){
  previous_incomming_Belly_O=atoi(incomming);
  Bridge.get("Belly_O",  valueStr, buffersize);
  memcpy(Trans_Key_sensors[BELLY_Sensor], Key_sensors[atoi(valueStr)] , 2*sizeof(char));
}
  Bridge.get("incomming_Feet_O",  incomming, 2);
  if (atoi(incomming)!=previous_incomming_Feet_O){
  previous_incomming_Ears_O=atoi(incomming);
  Bridge.get("Feet_O",  valueStr, buffersize);
  memcpy(Trans_Key_sensors[FEET_Sensor], Key_sensors[atoi(valueStr)] , 2*sizeof(char));
  memcpy(Trans_Key_sensors[FEET_Sensor+1], Key_sensors[atoi(valueStr)] , 2*sizeof(char));
} 
  //Serial.println(Trans_Key_sensors[0][1]);
  //Serial.println(Trans_Key_sensors[EARS_Sensor][1]);
}
void Send_key(String str){
  Bridge.put("SystemMsg_I", str);
  itoa( (outcomming_SystemMsg_I=outcomming_SystemMsg_I^1), outcome, 10);
  Bridge.put("outcomming_SystemMsg_I", outcome);
}
void reset(){
  state = 2 ; 
  have_Q = 0;
  blink_state=0;
  strcpy(q_time,"R");
  digitalWrite(answerPin , LOW);
  digitalWrite(questionPin, LOW); 
  fsr.Setkey((char*)Key_sensors);
  system_regist_flag= 0 ;
}






void setup() {

  // Bridge startup
  Bridge.begin();
  
  Serial.begin(9600);
  pinMode(questionPin , OUTPUT );
  pinMode(answerPin , OUTPUT ) ;
  pinMode(ioTtalkPin,OUTPUT);

  fsr.set_press_level(0,5,100);
  fsr.set_press_level(1,5,80);
  fsr.set_press_level(2,5,80);
  
}

void loop(){

  if (blink_state==1)
    blink(ioTtalkPin);
  else {  
    Bridge.get("Reg_done",  pin13, 2);
    digitalWrite(ioTtalkPin, atoi(pin13));
    if (blink_state==2) 
      blink(questionPin);  
    else if (blink_state==3)
      blink(answerPin);
  }
//Q_OK state 2 to state 1 when enemy receive question_request send
//QC_OK question compelte state 1 to state 2

  Bridge.get("incomming_SystemMsg_O",  incomming, 2); // receive Q_state set OK 
  if (atoi(incomming)!=previous_incomming_SystemMsg_O){
    previous_incomming_SystemMsg_O=atoi(incomming);
    Bridge.get("SystemMsg_O",  valueStr, buffersize);
    Serial.println(valueStr);
    if (strcmp (valueStr, "QA_OK") == 0 )  { 
      have_Q = 1 ; 
      state = 1 ;
      blink_state = 0; 
      digitalWrite(questionPin, HIGH);
      digitalWrite(answerPin , LOW);  
      index=2;
      strcpy(message,"Q_");
      play_music("start","-30 --start=.5"); }
    else if (have_Q == 0 && strncmp(valueStr, "QA_",3 ) ==0 ) { //involve question button at the same time
      
      if(strcmp(valueStr , q_time) <= 0 ) {
        digitalWrite(questionPin , LOW ); 
        digitalWrite(answerPin , LOW);
        Send_key("QA_OK"); 
        blink_state = 3; 
        have_Q =1 ; 
        play_music("waiting","-30");
      }
    }
    else if (have_Q == 1 && strcmp(valueStr, "A_OK") ==0 ) {
      play_music("success","-30 "); 
      for (int i=0;i<4200;i+=300)  { 
      digitalWrite(questionPin, !digitalRead(questionPin)); 
      digitalWrite(answerPin, !digitalRead(answerPin));
      delay(300);}
      reset(); 
      }   //答對了       
    else if ( strncmp(valueStr, "Q_", 2) ==0 ){
      strcpy(r_message,valueStr+2);
      Translator_O();
      fsr.Setkey((char*)Trans_Key_sensors);
      //if (state==2) {
        r_index=0;state=3;blink_state = 0;digitalWrite(answerPin , HIGH);digitalWrite(questionPin , LOW); 
        for (int i=0;i!=strlen(r_message);i++) {
          Serial.print(r_message[i]);
          play_music(r_message[i]); delay(2200);} //}  // receive the question enter answer section
          play_music("answer","-30");delay(1500);
      
    }
    else if (strcmp (valueStr, "RESET") == 0 ) { 
      reset();
      Send_key("RESET_OK");
      }
    else if (strcmp (valueStr, "RESET_OK") == 0 ) { 
      blink_state= 0 ;
      }
    else if (strcmp (valueStr, "ERROR") == 0 ) { 
      play_music("laugh","-30 ");
      }
  }
//state 1 question 2 waiting 3 answer




  char key = fsr.getkey();

  if (key != NO_KEY){
    Serial.println(key);
    if (key == RESET ) { reset();  blink_state=1; Send_key("RESET"); }
    if (state == 1 )  {

      if (index == 7 && key != SEND){
            play_music("limit","-30");
          }
      else if (key >= EARS && key <= FEET ) { play_music(key);message[index++]=key;/*String str(key);Send_key("K_"+str);*/      }
      else if (key == SEND ) {
        if (index==2) play_music("no_melody","-30");
        else {
          message[index]=0;
          state= 4 ;
          Send_key(message);
          strcpy(r_message,message+2);
          play_music("send","-30");
          blink_state = 3; 
          digitalWrite(questionPin, HIGH); 
          digitalWrite(answerPin , LOW); 
        }
      } 
    }
    else if (state == 2 )  { 
      play_music(key);
      if (key == SEND && have_Q == 0) { //Q
        Translator_I();
        strcpy(q_time, ("QA_"+getTime()).c_str());
        Send_key("QA_"+getTime());
        blink_state=2;       
        play_music("game_start","-20");
      }
      /*if (key == SEND && have_Q == 1) { //A
        state = 3 ;
        r_index =0;
        blink_state=0;  
        digitalWrite(answerPin , HIGH);
        digitalWrite(questionPin , LOW); 
        play_music("answer","-20");delay(1200);
        for (int i=0;i!=strlen(r_message);i++) {
          play_music(r_message[i]); delay(1500);}  // receive the question enter answer section
      }*/
    }
          
    
    else if (state == 3 )  { 
      if (key == SEND ) {for (int i=0;i!=strlen(r_message);i++) {  play_music(r_message[i]); delay(2200);} } //B
      else if (key >= EARS  && key <= FEET ) {
        //Serial.println(strlen(r_message));
        if( key==r_message[r_index] ) {r_index++;play_music(key);} 
        else { Send_key("ERROR"); play_music(key); delay(800);led_alarm();}
        if (r_index==strlen(r_message) ) {        
        Send_key("A_OK");              
        delay(500);
        play_music("success","-30");
        for (int i=0;i<4200;i+=300)  { 
          digitalWrite(questionPin, !digitalRead(questionPin)); 
          digitalWrite(answerPin, !digitalRead(answerPin));
          delay(300);}
        reset();
        } 
      } 
    }
     else if (state == 4 )  { 
         if (key!=RESET) play_music("wait_opponent","-30");
     }
     
  } 
}
