#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;


#define LEDC_CHANNEL_0     0
#define LEDC_TIMER_13_BIT  13
#define LEDC_BASE_FREQ 10000
#define LEDC_CHANNEL_1     1
#define LEDC_CHANNEL_2     2
#define LEDC_CHANNEL_3     3
#define LEDC_CHANNEL_4     4
#define MOTOR_NORDEN_PIN   12
#define MOTOR_SUEDEN_PIN   15
#define MOTOR_WESTEN_PIN   33
#define MOTOR_OSTEN_PIN   32
#define ZIEL 255
#define STOPP 251

int stateBT = 0;
int motorNorden = 0;
int motorSueden = 0;
int motorWesten = 0;
int motorOsten = 0;
int motorNorden_old = 0;
int motorSueden_old = 0;
int motorWesten_old = 0;
int motorOsten_old = 0;

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) 
{
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (8191 / valueMax) * min(value, valueMax);
  ledcWrite(channel, duty);
}

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  // function implementation
  if (event == ESP_SPP_SRV_OPEN_EVT) 
  {
    // handling of client connection event
    Serial.println("BT Serial connection opened!");
  } else 
  {
    //struct spp_cl_init_evt_param p = *param;
    if (event == ESP_SPP_DATA_IND_EVT) 
    {
      //Serial.print("BT callback event"); Serial.print(event); Serial.print(param->data_ind.len); Serial.println();
      uint8_t *data = param->data_ind.data;
      for (int i = 0; i < param->data_ind.len; i++) 
      {
        if(i >= 4)
          break;
        Serial.print("value "); Serial.print(i); Serial.print(": "); Serial.print(data[i]); Serial.println();
        
        if(data[i]!=STOPP)
        {
          if(i==0)
          {
            motorNorden = (2+data[i])*250/12;
            if(data[i] == 0)
              motorNorden = 0;
            if(motorNorden_old <= 71 && motorNorden > 71)     motorNorden=150;       //Starthilfe
            motorNorden_old = motorNorden;
            Serial.print("motorNorden: ");
            Serial.println(motorNorden);
          }
          else if(i==1)
          {
            motorSueden = (2+data[i])*250/12;
            if(data[i] == 0)
              motorSueden = 0;
            if(motorSueden_old <= 71 && motorSueden > 71)     motorSueden=150;       //Starthilfe
            motorSueden_old = motorSueden;
            Serial.print("motorSueden: ");
            Serial.println(motorSueden);
          }
          else if(i==2)
          {
            motorWesten = (2+data[i])*250/12;
            if(data[i] == 0)
              motorWesten = 0;
            if(motorWesten_old <= 71 && motorWesten > 71)     motorWesten=150;       //Starthilfe
            motorWesten_old = motorWesten;
            Serial.print("motorWesten: ");
            Serial.println(motorWesten);
          }
          else if(i==3)
          {
            motorOsten = (2+data[i])*250/12;
            if(data[i] == 0)
              motorOsten = 0;
            if(motorOsten_old <= 71 && motorOsten > 71)     motorOsten=150;       //Starthilfe
            motorOsten_old = motorOsten;
            Serial.print("motorOsten: ");
            Serial.println(motorOsten);
          }  
        }
        else if(data[i]==STOPP)
        {
          motorNorden = 0;
          motorSueden = 0;
          motorWesten = 0;
          motorOsten  = 0;
          break;
        }
        else  Serial.println("#1 Error");
      }

      ledcAnalogWrite(LEDC_CHANNEL_1, motorNorden);
      ledcAnalogWrite(LEDC_CHANNEL_2, motorSueden);
      ledcAnalogWrite(LEDC_CHANNEL_3, motorWesten);
      ledcAnalogWrite(LEDC_CHANNEL_4, motorOsten);
    }
  }
}

 
void setup() 
{
  Serial.begin(115200);
  SerialBT.begin("Touchnik"); //Bluetooth device name
  SerialBT.register_callback(callback);
  Serial.println("The device started, now you can pair it with bluetooth!");

  ledcSetup(LEDC_CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcSetup(LEDC_CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcSetup(LEDC_CHANNEL_3, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcSetup(LEDC_CHANNEL_4, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(MOTOR_NORDEN_PIN, LEDC_CHANNEL_1);
  ledcAttachPin(MOTOR_SUEDEN_PIN, LEDC_CHANNEL_2);
  ledcAttachPin(MOTOR_WESTEN_PIN, LEDC_CHANNEL_3);
  ledcAttachPin(MOTOR_OSTEN_PIN, LEDC_CHANNEL_4);
  pinMode(MOTOR_NORDEN_PIN, OUTPUT);
  pinMode(MOTOR_SUEDEN_PIN, OUTPUT);
  pinMode(MOTOR_WESTEN_PIN, OUTPUT);
  pinMode(MOTOR_OSTEN_PIN, OUTPUT);
}
 
void loop() 
{}
