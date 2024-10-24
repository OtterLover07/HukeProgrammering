const int potPin = A0;
const int tempPin = A1;


void setup() {
  for(int i=4; i<=7; i++){
    pinMode(i, OUTPUT);
  }

  pinMode(potPin, INPUT);
  pinMode(tempPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.print("DangerTemp:");
  Serial.print(tempThreashold());
  Serial.print(" CurrentTemp:");
  Serial.println(measureTemp());

  if (measureTemp() >= tempThreashold()) {

    while (measureTemp() >= tempThreashold()) {
    strobe();

    Serial.print("DangerTemp:");
    Serial.print(tempThreashold());
    Serial.print(" CurrentTemp:");
    Serial.println(measureTemp());
    }
    for(int i=4; i<=7; i++){
      digitalWrite(i, LOW);
      delay(50);
    }
  }
}

//Custom functions begin here
inline void digitalToggle(byte pin){
  digitalWrite(pin, !digitalRead(pin));
}

void strobe() {
  for(int i=4; i<=7; i++){
    digitalToggle(i);
    delay(20);
  }
}

int tempThreashold(){
  return map(analogRead(potPin), 0, 1000, 20, 40);
}

float measureTemp(){
    int Vo;
  float R1 = 10000; // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741; //steinhart-hart coeficients for thermistor
  Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0); //calculate resistance on thermistor
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); // temperature in Kelvin
  float temp = T - 273.15; //convert Kelvin to Celcius
  return temp;
}