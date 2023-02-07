#include <ESP32Servo.h>

TaskHandle_t Serial_comm;
TaskHandle_t Relay01_ctrl;
TaskHandle_t Relay02_ctrl;
TaskHandle_t Servo01_ctrl;

const int Serial_comm_prior = 0;
const int Servo01_ctrl_prior = 2;
const int Relay01_ctrl_prior = 1;
const int Relay02_ctrl_prior = 1;

const int BUFFER_SIZE = 30; //Serial size input message buffer
const int DEVICE_ID_SIZE = 6; //Device ID buffer
const int PROGRAM_NUM_SIZE = 4; //Program number buffer
const int PROGRAM_AUX_NUM_SIZE = 7; //Program auxilary number buffer
const int Relay01 = 2; //IO port for relay 1
const int Relay02 = 4; //IO port for relay 2
const int Servo01 = 13; //IO port for servo 1

char buf[BUFFER_SIZE]; //Array for input message
char device_ID[DEVICE_ID_SIZE]; //Size of device id
char program_num[PROGRAM_NUM_SIZE]; //Size of program number on the device
char program_aux_num[PROGRAM_AUX_NUM_SIZE]; //Size of program auxilary number for the program
int program_num_dec; //program number converted to integer
int program_aux_num_dec;
int RELEASE_BUTTON = 0; //Anti-clock wise rotate
int STOP = 90; //Stop rotate
int PRESS_BUTTON = 180; //Clock wise rotate
int Press_time = 120; //motor press button time
int Release_time = 100; //motor release button time
int Servo01_busy = 0;

enum Relay01_state{ON,OFF}Relay01_currently;
enum Relay02_state{ON,OFF}Relay02_currently;
enum Servo_state{PRESS,RELEASE,PRESS_RELEASE_COUNT,PRESS_UNTIL}Servo_currently;

Servo myservo; //ประกาศตัวแปรแทน Servo

void setup()
{
  Serial.begin(115200);
  pinMode(Relay01, OUTPUT);
  pinMode(Relay02, OUTPUT);
  myservo.attach(Servo01);
  delay(500); //wait for hardware inintial

  xTaskCreatePinnedToCore(SerialComm,"Serial_comm",10000,NULL,Serial_comm_prior,&Serial_comm,0);

}

void SerialComm( void * parameter )
{
//  Serial.print("Task1 is running on core ");
//  Serial.println(xPortGetCoreID());

  for(;;)

  {
    // check if data is available and not overflow from our holding buffer size
    if (Serial.available() > 0 && Serial.available() <= BUFFER_SIZE)
    {
      // read the incoming bytes: in format of XXXYYY where: XXX = device_name, YYY = program_number
      int rlen = Serial.readBytesUntil('\n', buf, BUFFER_SIZE); //Read every byte in message until \n is found
      // Serial.print("Whole buffer : "); Serial.println(buf); //Print all bytes in buffer
      // Serial.flush();

      //get device name
      for(int i = 0; i < DEVICE_ID_SIZE-1; i++)
      {
        device_ID[i] = buf[i]; //buffer [0~4] >> device ID
      }
      device_ID[DEVICE_ID_SIZE-1] = '\0';
      // Serial.print("Device ID : "); Serial.println(device_ID);
      // Serial.flush();

      //get program number of the device
      for(int i = 0; i < PROGRAM_NUM_SIZE-1; i++)
      {
        program_num[i] = buf[i + 5]; //buffer [5~7] >> Program number
      }
      program_num[PROGRAM_NUM_SIZE-1] = '\0'; //Stop convert here
      program_num_dec = atoi(program_num); //convert char to int
      // Serial.print("Device program no. : "); Serial.println(program_num_dec);
      // Serial.flush();

      //get auxilary number of the program
      for(int i = 0; i < PROGRAM_AUX_NUM_SIZE-1; i++)
      {
        program_aux_num[i] = buf[i + 8]; //buffer [8~13] >> Program auxilary number
      }
      program_aux_num[PROGRAM_AUX_NUM_SIZE-1] = '\0'; //Stop convert here

      int AUX_NUM_ZERO_CHK = atoi(program_aux_num);

      if(AUX_NUM_ZERO_CHK > 0)
      {
        program_aux_num_dec = AUX_NUM_ZERO_CHK; //convert char to int
        // Serial.print("Program auxilary no. : "); Serial.println(program_aux_num_dec);
        // Serial.flush();
      }

      //Relay 01 control start here:
      if(device_ID[0] == 'R' && device_ID[1] == 'L' && device_ID[2] == 'Y' && device_ID[3] == '0' && device_ID[4] == '1') //compare if input message == "RLY01" or not
      {
        // vTaskResume(Relay01_ctrl);
        // Serial.println("Relay01 control access");
        // Serial.flush();
        switch(program_num_dec)
        {
          case 1: //Program number 1
          {
            Relay01_currently = ON;
            // Serial.print("Relay01_currently : "); Serial.println("ON"); //debug
            break;
          }

          case 2:
          {
            Relay01_currently = OFF;
            // Serial.print("Relay01_currently : "); Serial.println("OFF"); //debug
            break;
          }
        }

      xTaskCreatePinnedToCore(RLY01_control,"Control_relay01_manual",1000,(void*)&Relay01_currently,Relay01_ctrl_prior,&Relay01_ctrl,0);
      Serial.println("OK");

      }

      //Relay 02 control start here:
      if(device_ID[0] == 'R' && device_ID[1] == 'L' && device_ID[2] == 'Y' && device_ID[3] == '0' && device_ID[4] == '2') //compare if input message == "RLY02" or not
      {
        // vTaskResume(Relay01_ctrl);
        // Serial.println("Relay01 control access");
        // Serial.flush();
        switch(program_num_dec)
        {
          case 1: //Program number 1
          {
            Relay02_currently = ON;
            // Serial.print("Relay02_currently : "); Serial.println("ON"); //debug
            break;
          }

          case 2:
          {
            Relay02_currently = OFF;
            // Serial.print("Relay02_currently : "); Serial.println("OFF"); //debug
            break;
          }
        }

      xTaskCreatePinnedToCore(RLY02_control,"Control_relay02_manual",1000,(void*)&Relay02_currently,Relay02_ctrl_prior,&Relay02_ctrl,0);
      Serial.println("OK");

      }

      // Servo control start here:
      else if(device_ID[0] == 'S' && device_ID[1] == 'V' && device_ID[2] == 'O' && device_ID[3] == '0' && device_ID[4] == '1') //compare if input message == "RLY01" or not
      {
        // vTaskResume(Servo01_ctrl);
        // Serial.println("Servo01 control access");
        // Serial.flush();
        switch(program_num_dec)
        {
          case 1: //Program number 1
          {
            Servo_currently = PRESS;
            // Serial.print("Servo_currently : "); Serial.println("PRESS"); //debug
            // Serial.flush();
            break;
          }

          case 2:
          {
            Servo_currently = RELEASE;
            // Serial.print("Servo_currently : "); Serial.println("RELEASE"); //debug
            // Serial.flush();
            break;
          }

          case 3:
          {
            Servo_currently = PRESS_RELEASE_COUNT;
            // Serial.print("Servo_currently : "); Serial.println("PRESS_RELEASE_COUNT"); //debug
            // Serial.flush();
            break;
          }

          case 4:
          {
            Servo_currently = PRESS_UNTIL;
            // Serial.print("Servo_currently : "); Serial.println("PRESS_UNTIL"); //debug
            // Serial.flush();
            break;
          }

        }

      if (Servo01_busy == 1)
      {
        vTaskDelete(Servo01_ctrl);
        Servo01_busy = 0;
      }
      if (Servo02_busy == 1)
      {
        vTaskDelete(Servo01_ctrl);
        Servo01_busy = 0;
      } 
      xTaskCreatePinnedToCore(SVO01_control,"Control_servo01_manual",1000,(void*)&Servo_currently,Servo01_ctrl_prior,&Servo01_ctrl,0);
      Serial.println("OK");
        
      }

      //clear buffers
      for(int i = 0; i < BUFFER_SIZE; i++)
      {
        buf[i] = '\x00';
      }

      for(int i = 0; i < DEVICE_ID_SIZE-1; i++)
      {
        device_ID[i] = '\x00';
      }

      for(int i = 0; i < PROGRAM_NUM_SIZE-1; i++)
      {
        program_num[i] = '\x00';
      }

      for(int i = 0; i < PROGRAM_AUX_NUM_SIZE-1; i++)
      {
        program_aux_num[i] = '\x00';
      }

      program_num_dec = 0;
    }

    //In case massive incomming data accidently breaks into RX port
    else if (Serial.available() > 0 && Serial.available() > BUFFER_SIZE)
    {
      Serial.println("OVER BUFFER");
      Serial.end();
      delay(100);
      Serial.begin(115200);
    }
    
    delay(20);

  }
}


void RLY01_control(void * parameter)
{
  // Serial.print("Relay PG : "); Serial.println(*((int*)parameter)); //debug
  for(;;)
  {
      if(*((int*)parameter) == 0)
      {
        digitalWrite(Relay01, HIGH);
        delay(100);
      }

      else if(*((int*)parameter) == 1)
      {
        digitalWrite(Relay01, LOW);
        delay(100);
      }
  }
}

void RLY02_control(void * parameter)
{
  // Serial.print("Relay PG : "); Serial.println(*((int*)parameter)); //debug
  for(;;)
  {
      if(*((int*)parameter) == 0)
      {
        digitalWrite(Relay02, HIGH);
        delay(100);
      }

      else if(*((int*)parameter) == 1)
      {
        digitalWrite(Relay02, LOW);
        delay(100);
      }
  }
}

void SVO01_control(void * parameter)
{
  // Serial.print("Motor PG : "); Serial.println(*((int*)parameter)); //debug
    Servo01_busy = 1;
    if(*((int*)parameter) == 0)
    {
      myservo.write(PRESS_BUTTON);
      delay(Press_time);
      myservo.write(STOP);
      delay(100);
      // vTaskDelete(Servo01_ctrl);
    }

    else if(*((int*)parameter) == 1)
    {
      myservo.write(RELEASE_BUTTON);
      delay(Release_time);
      myservo.write(STOP);
      delay(100);
      // vTaskDelete(Servo01_ctrl);
    }

    else if(*((int*)parameter) == 2)
    {
      for(int i = 0; i < program_aux_num_dec; i++)
      {
        myservo.write(PRESS_BUTTON);
        delay(Press_time);
        myservo.write(STOP);
        delay(1100);
        myservo.write(RELEASE_BUTTON);
        delay(Release_time);
        myservo.write(STOP);
        delay(1100);
      }
      // vTaskDelete(Servo01_ctrl);
    }

    else if(*((int*)parameter) == 3)
    {
      myservo.write(PRESS_BUTTON);
      delay(Press_time);
      myservo.write(STOP);
      delay(program_aux_num_dec*1000);
      myservo.write(RELEASE_BUTTON);
      delay(Release_time);
      myservo.write(STOP);
      delay(100);
      // vTaskDelete(Servo01_ctrl);
    }

    Servo01_busy = 0;
    vTaskDelete(Servo01_ctrl);
}


void loop() {
  
}