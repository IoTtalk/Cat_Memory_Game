const char NO_KEY= '\0';

class Pressure{
  public:
    Pressure(int,int*,char*);
    ~Pressure();
    char getkey();
    void set_press_level(int,int,int);
    void Setkey(char*);
  private:
  
  class Pressure_btn;
  Pressure_btn **_sensors;
  int _num;
  int* _init_array;
  int _index= -1 ; //the index which button is pressed   -1 mean no press
  char * _key;
};
