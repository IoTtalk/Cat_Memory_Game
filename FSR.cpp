#include "FSR.h"
#include "Arduino.h"


/*
class Pressure{
  public:
    Pressure(int,int*,char*);
    ~Pressure();
    char getkey();
  private:
  
  class Pressure_btn;
  Pressure_btn **_sensors;
  int _num;
  int* _init_array;
  int _index= -1 ; //the index which button is pressed   -1 mean no press
  char * _key;
};
*/
class Pressure::Pressure_btn{
  public:
    Pressure_btn();
    Pressure_btn(int);
    void setup(int);
    int getPressLevel(int);
    int press();
    void set_level(int,int);
  private:
    int _inputPin;
    unsigned long _lastDebounceTime = 0;
    unsigned long _debounceDelay = 50;
    int _btnState;
    int _lastPressState = 0;
    int _press1 = 5 ;
    int _press2 = 70 ;
};
Pressure::Pressure(int num,int* init_array,char* key): _num(num),_init_array(init_array),_key(key){
  _sensors = new Pressure_btn*[_num];
  for(int i=0;i!=_num;i++){
    _sensors[i]= new Pressure_btn(init_array[i]);
  }

}
void Pressure::Setkey(char* key){
  _key=key;
}
void Pressure::set_press_level(int list,int press1,int press2){

  _sensors[list]->set_level(press1,press2);
  
  }
void Pressure::Pressure_btn::set_level(int press1,int press2) {
  _press1=press1;_press2=press2;
}
char Pressure::getkey(){
  if ( _index >= 0 )   {
     for(int i=0;i!=_num;i++)
        if ( _sensors[i]->press() != 0 ) return NO_KEY ; 
      _index = -1 ;
      
    }
  else {
    for(int i=0;i!=_num;i++){
      int temp = _sensors[i]->press();
      if (temp  > 0 ) { _index=i ; return _key[_index*2+temp-1];}
      }
    }
    return NO_KEY ;
  }
Pressure::~Pressure(){
  for(int i=0 ;i!=_num;i++){
      delete _sensors[i];
  }
  delete [] _sensors;
}
void Pressure::Pressure_btn::setup(int inputPin) {
  _inputPin = inputPin;
}

Pressure::Pressure_btn::Pressure_btn(int inputPin): _inputPin(inputPin){ 
  }
int Pressure::Pressure_btn::getPressLevel(int _fsrReading){
    if(_fsrReading <= _press1){
      return 0;
    }
    else if(_fsrReading > _press1 and _fsrReading <= _press2 ){
      return 1;
    }
    else{
      return 2;
    }
  }
int Pressure::Pressure_btn::press(){  //-1 if no changed 0 no press 1 light press 2 heavy press
    int fsrReading = analogRead( _inputPin);
    int pressLevelState = getPressLevel(fsrReading);
    // If the switch changed, due to noise or pressing:
    if(pressLevelState != _lastPressState){
      _lastDebounceTime = millis();
    }
    //reading is been there for longer than the debounce delay
    //take it as the actual current state
    if((millis() - _lastDebounceTime) > _debounceDelay){
      //if the button state has changed:
      if(pressLevelState != _btnState){
        _btnState = pressLevelState;
        return pressLevelState;
      }
      //if the button state has no changed:
      else{
        if (_btnState == 0) return 0;
      }
    }
    // save the reading.  Next time through the loop,
    // it'll be the lastButtonState:
    _lastPressState = pressLevelState;
    return -1;
  }
  
