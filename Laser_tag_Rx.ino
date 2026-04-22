#define LDR_PIN A0     
#define LED_PIN 8      

const int THRESHOLD_OFFSET = 100; 
int baseLightLevel = 0;           

unsigned long lastHitTime = 0;
const int HIT_LED_DURATION = 1000; 
bool isHit = false;


const int BIT_TIME = 20;

void setup() {
  Serial.begin(9600); 
  pinMode(LED_PIN, OUTPUT); 
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Запуск Приемника RX (Синхрослово 1011). Начинаю калибровку...");
  
 
  long sumLight = 0;
  for (int i = 0; i < 50; i++) 
  {
    sumLight += analogRead(LDR_PIN);
    delay(10);
  }
  baseLightLevel = sumLight / 50;
  
  Serial.print("Калибровка завершена. Базовый фон АЦП: ");
  Serial.println(baseLightLevel);
  Serial.println("Ожидание передачи...");
}

void loop() 
{
  int currentLight = analogRead(LDR_PIN);
  int threshold = baseLightLevel + THRESHOLD_OFFSET;
  
  
  if (currentLight > threshold && !isHit) 
  {
    

    delay(BIT_TIME / 2); 
    int val1 = analogRead(LDR_PIN);
    
    delay(BIT_TIME);
    int val2 = analogRead(LDR_PIN);
    
    delay(BIT_TIME);
    int val3 = analogRead(LDR_PIN);
    
    delay(BIT_TIME);
    int val4 = analogRead(LDR_PIN);
    
    bool bit1 = val1 > threshold;

    bool bit2 = val2 < (val1 - 20);
    bool bit3 = val3 > threshold;
    bool bit4 = val4 > threshold;
    
 
    if (bit1 && bit2 && bit3 && bit4) 
    {
      Serial.println(">>> ЗАСЧИТАНО ПОПАДАНИЕ (СИНХРОСЛОВО 1011) <<<");
      isHit = true;
      lastHitTime = millis();
      digitalWrite(LED_PIN, HIGH); 
    } else 
    {
      
      Serial.println("Ложный блик (Синхрослово не совпало).");
      delay(BIT_TIME * 2); 
    }
  }

  
  if (isHit && (millis() - lastHitTime > HIT_LED_DURATION)) 
  {
    digitalWrite(LED_PIN, LOW);
    isHit = false;
  }
  
  delay(2); 
}
