#include <Wire.h>
#include <Adafruit_LSM6DS3.h>
#include <Adafruit_Sensor.h>


Adafruit_LSM6DS3 lsm6ds3;

#define LASER_PIN  12    
#define LED_PIN    8      


#define THRUST_THRESHOLD 5.0 

float baseAccel = 0.0; 




unsigned long lastShotTime = 0;
const int COOLDOWN_MS = 500; 


void setup() 
{
  Serial.begin(9600); 
  
 

  if (!lsm6ds3.begin_I2C(0x6B))
  {
    Serial.println("Ошибка: не удалось найти датчик GY-LSM6DS3!");
    while (1)
    {
      delay(10); 
    }
  }
  
  Serial.println("Акселерометр подключён. Начало калибровки...");
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); 
  
 
  lsm6ds3.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
  lsm6ds3.setAccelDataRate(LSM6DS_RATE_104_HZ);

  
  float sumAccel = 0;
  for (int i = 0; i < 50; i++) 
  {
    sensors_event_t a, g, t;
    lsm6ds3.getEvent(&a, &g, &t);
    sumAccel += sqrt(a.acceleration.x * a.acceleration.x + 
                     a.acceleration.y * a.acceleration.y + 
                     a.acceleration.z * a.acceleration.z);
    delay(10);
  }
  baseAccel = sumAccel / 50.0;
  
  Serial.print("Калибровка завершена. Базовый вектор: ");
  Serial.print(baseAccel);
  Serial.println(" м/с^2");
  Serial.println("Лазертаг TX готов к стрельбе!");
}

void loop() 
{
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  

  lsm6ds3.getEvent(&accel, &gyro, &temp);

 
  float totalAccel = sqrt(accel.acceleration.x * accel.acceleration.x + 
                          accel.acceleration.y * accel.acceleration.y + 
                          accel.acceleration.z * accel.acceleration.z);


  if (abs(totalAccel - baseAccel) > THRUST_THRESHOLD) 
  {
    if (millis() - lastShotTime > COOLDOWN_MS) 
    {
      
      Serial.print("Рывок обнаружен: ");
      Serial.print(totalAccel);
      Serial.println(" м/с^2 -> Выстрел!");

      digitalWrite(LED_PIN, HIGH); 


      const int BIT_TIME = 20;
      digitalWrite(LASER_PIN, HIGH); delay(BIT_TIME); 
      digitalWrite(LASER_PIN, LOW);  delay(BIT_TIME); 
      digitalWrite(LASER_PIN, HIGH); delay(BIT_TIME); 
      digitalWrite(LASER_PIN, HIGH); delay(BIT_TIME); 
      digitalWrite(LASER_PIN, LOW);                   
      
      
      lastShotTime = millis();
      delay(500);
      digitalWrite(LED_PIN, LOW); 

    }
  } 
  else 
  {

    static int debugCounter = 0;
    if (debugCounter++ > 50) 
    {
      Serial.print("Дельта_Ускорения:");
      Serial.println(abs(totalAccel - baseAccel));
      debugCounter = 0;
    }
  }
  

  delay(10);
}
