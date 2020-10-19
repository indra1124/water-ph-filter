/*
* Konfigurasi pin pada perangkat ke arduino
* 
* Digital:
* Water Level Minimum     -   pin 2
* Water Level Maksimum    -   pin 3
* Pompa Masukan           -   pin 4
* Pompa Pembuangan        -   pin 5
* 
* Analog:
* Sensor PH -> P0         -   pin A0
* 
*/
  
const int  WATER_LEVEL_MIN = 2;
const int  WATER_LEVEL_MAX = 3;

const int  WATER_PUMP_IN   = 4;
const int  WATER_PUMP_OUT  = 5;

const int  WATER_PH_SENSOR = A0;

unsigned long int run_time = 0;
unsigned long int avgValue;

bool refilling = false;
int buffer_data[10], temp;

void setup() {
  Serial.begin(9600);

  Serial.println("Inisialisasi konfigurasi perangkat pada kontroller...");
  pinMode(WATER_LEVEL_MIN, INPUT);
  pinMode(WATER_LEVEL_MAX, INPUT);

  pinMode(WATER_PUMP_IN, OUTPUT);
  pinMode(WATER_PUMP_OUT, OUTPUT);

  pinMode(WATER_PH_SENSOR, INPUT);

  digitalWrite(WATER_PUMP_IN, LOW);
  digitalWrite(WATER_PUMP_OUT, LOW);
  Serial.println("Selesai melakukan konfigurasi.");
}

void loop() 
{
  /*
  * Mengambil nilai dari sensor.
  */

  for(int i = 0; i < 10; i++) { 
    buffer_data[i] = analogRead(WATER_PH_SENSOR);// membaca water sensor
    delay(10);
  }
  
  for(int i = 0; i < 9; i++) {
    for(int j = i+1; j < 10; j++) if(buffer_data[i] > buffer_data[j]) {
      temp = buffer_data[i];
      buffer_data[i] = buffer_data[j];
      buffer_data[j] = temp;
    }
  }
  avgValue = 0;

  for(int i = 2; i < 8; i++) {
    avgValue += buffer_data[i];
  }

  float pHVol = (float)avgValue* 5.0 / 1024 / 6;//kalkulasi sensor ph
  float ph_value = -5.70 * pHVol + 21.34;

  if(run_time < millis()) {
    String dataOut = "PH: " + String(ph_value) + " || " + "S-Min: " + digitalRead(WATER_LEVEL_MIN) + " || " + "S-Max: " + digitalRead(WATER_LEVEL_MAX) + " || " + "Refill: " + refilling;
    Serial.println(dataOut);

    run_time = millis() + 500;
  }
 
  if(ph_value < 2 || ph_value > 8) {
    if(!refilling) {
      refilling = true;
      
      digitalWrite(WATER_PUMP_IN, LOW);
      digitalWrite(WATER_PUMP_OUT, HIGH);   
      Serial.println("[debug]: Water doesn't clean, turn on out pump!");   
    }
  }

  if(refilling && digitalRead(WATER_PUMP_OUT) == HIGH && digitalRead(WATER_LEVEL_MIN) == LOW) {
      digitalWrite(WATER_PUMP_IN, HIGH);
      digitalWrite(WATER_PUMP_OUT, LOW);
      Serial.println("[debug]: Filling water ...");
  }

  if(refilling && digitalRead(WATER_PUMP_IN) == HIGH && digitalRead(WATER_LEVEL_MAX) == HIGH) {
      refilling = false;
      
      digitalWrite(WATER_PUMP_IN, LOW);
      Serial.println("[debug]: Full!");
  }
}
