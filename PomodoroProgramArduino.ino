class Timer {
  private: 
    int _remainingTime = 0;
    int _targetTime = 0;
    unsigned long _beginTime = 0;   // <-- исправлено
    unsigned long _isTimerEnabled = false;

  public:
    void start(int seconds) {
      _targetTime = seconds;
      _remainingTime = _targetTime;
      _beginTime = millis();
      _isTimerEnabled = true;
      Serial.println("started");
    }

    void stop() {
      _isTimerEnabled = false;
      _remainingTime = 0;
      _targetTime = 0;
      _beginTime = 0;
    }

    void update() {
      if (_isTimerEnabled) {
        unsigned long elapsed = (millis() - _beginTime) / 1000;
        if (elapsed >= (unsigned long)_targetTime) {
          stop();
          // playSound
        } else {
          _remainingTime = _targetTime - (int)elapsed;
        }
      }
    }

    void pause(){
      if(_isTimerEnabled==false){
        return;
      }

      _isTimerEnabled = false;
    }

    void unpause(){
      if(_isTimerEnabled==true){
        return;
      }
      start(_remainingTime);
    }

    bool IsTimerEnabled() {
      return _isTimerEnabled;
    }

    int getTimeRemaining() {
      if (_remainingTime < 0) return 0;
      return _remainingTime;
    }
};

#include <LiquidCrystal.h>
#include <Keypad.h>

const byte ROWS = 2;
const byte COLS = 4;

char keys[ROWS][COLS] = {   // <-- тип char
  {'1', '2', '3', '4'},
  {'5', '6', '7', '8'}
};

byte rowPins[ROWS] = {A4, A5};
byte colPins[COLS] = {A3, A2, A1, A0};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int _selectedTime=0;

int const firstPreset = 1500;
int const secondPreset = 3000;
int const thirdPreset = 300;
int const fourthPreset = 600;

String lcdText;
String _blinkingText;

unsigned long lastBlinkTime = 0;
const long blinkInterval = 1000;  
bool isBlinking = true;
bool blinkState = true;           

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

Timer timer;

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  printTextOnLCD("NO TIMER!");
}

void loop() {
  char key = keypad.getKey();

  if (key) {   // если какая-то кнопка нажата
    Serial.print("Pressed: ");
    Serial.println(key);
    }

    switch (key) {
      case '1': selectTime(firstPreset); break;  
      case '2': selectTime(secondPreset); break;
      case '3': selectTime(thirdPreset); break;
      case '4': selectTime(fourthPreset); break;
      case '5': startTimer(_selectedTime); break;
      case '6': pauseTimer(); break;
      case '7': unpauseTimer(); break;
      case '8': stopTimer(); break;
    }

  timer.update();

  if(timer.IsTimerEnabled()){

    lcd.setCursor(0, 0);
    int remainingMinutes = timer.getTimeRemaining()/60;
    int remainingSeconds = timer.getTimeRemaining()%60;

    if(remainingMinutes>0)
    {
      printTextOnLCD(String(remainingMinutes));
      printTextOnLCD("min ");
    }

    if(remainingSeconds>0)
    {
      printTextOnLCD(String(remainingSeconds));
      printTextOnLCD("sec");
    }

    printTextOnLCD("               ");
  }

  blink();
}

void selectTime(int time){
  _selectedTime = time;

  showTime(_selectedTime);
}

void showTime(int time)
{

  clearLCDText();
  lcd.setCursor(0, 0);
  int minutes = time/60;
  int seconds = time%60;

  if (time>0)
  {
    printTextOnLCD("EST: ");

    if(minutes>0)
    {
      printTextOnLCD(String(minutes));
      printTextOnLCD("min ");
    }

    if(seconds>0)
    {
      printTextOnLCD(String(seconds));
      printTextOnLCD("sec");
    }
  }
}

void startTimer(int time)
{
  if (timer.IsTimerEnabled()){
    return;
  }

  timer.start(time);
  isBlinking = false;
}

void stopTimer()
{
  if(timer.IsTimerEnabled() == false)
  {
    return;
  }

  timer.stop();
  clearLCDText();
  printTextOnLCD("NO TIMER!");
  isBlinking = true;
}

void pauseTimer(){
  timer.pause();
  clearLCDText();
  printTextOnLCD("paused");
  isBlinking = true;
}

void unpauseTimer(){
  timer.unpause();
  isBlinking = false;//add states: paused, enabled, stopped
}

void blink(){
  if(isBlinking == false){
    return;
  }
  unsigned long now = millis();

  if (now - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = now;
    blinkState = !blinkState;
    clearLCDText();

    if(blinkState == true){
      printTextOnLCD(_blinkingText);
    }

    if(blinkState == false){
      clearLCDText();
    }
  }
}


 void printTextOnLCD(String text){
  lcd.print(text);
  lcdText += text;
  _blinkingText = lcdText;
}

void clearLCDText(){
  lcd.clear();
  lcdText="";
}
