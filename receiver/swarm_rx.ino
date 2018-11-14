//Final code
const int M1A = 12;  //B3
const int M1B = 10; //C5
const int M2A = 6; //B0
const int M2B = 7;//C2
const int pwm1 = 11;
const int pwm2 = 9;
const int opt = 88;
const char ledPin_White = A5;
const char ledPin_Green = A2;
const char ledPin_Yellow = A1;
const char ledPin_Red = A0;
float tune=1;

String inputString = "";        // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

volatile float theta = 0;

/*
 * Initialize the SWARM bot's motor pins and LED indicators
 */
void init_devices(void)
{
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(ledPin_White,OUTPUT);
  pinMode(ledPin_Green, OUTPUT);
  pinMode(ledPin_Red, OUTPUT);
  pinMode(ledPin_Yellow, OUTPUT);
}

/*
 * Bot motions
 */

void bot_stop()
{
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, LOW);
}


void bot_forward()
{
  digitalWrite(12,HIGH);
  digitalWrite(10,LOW);
  digitalWrite(7,HIGH);
  digitalWrite(6,LOW);
}

void bot_spot_clock()
{
  digitalWrite(M1A, HIGH);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, HIGH);
  digitalWrite(M2B, LOW);
}


void bot_spot_anti_clock()
{
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, HIGH);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, HIGH);
}

/*
 * send the parsed theta from the LiFi ciruits to the bot, 
 * simple P controller does it, setpoint theta creates a
 * turning torque for the differential drive robot
 */
void pwm_this_shit(float theta)
{
  int left  = tune*(opt - (2-tune)*theta);
  int right = tune*(opt + (2-tune)*theta);
  analogWrite(pwm1, left);
  analogWrite(pwm2, right);
  bot_forward();
  digitalWrite(ledPin_White,HIGH);
}

/*
 * OMG! The robot reached the target, do the DANCE
 */
void led_dance()
{
  for(unsigned int i=0;i<=3;i++)
  {
  digitalWrite(ledPin_White,HIGH);
  digitalWrite(ledPin_Green,HIGH);
  digitalWrite(ledPin_Yellow,HIGH);
  digitalWrite(ledPin_Red,HIGH);
  delay(200);
  digitalWrite(ledPin_Red,LOW);
  digitalWrite(ledPin_Yellow,LOW);
  digitalWrite(ledPin_Green,LOW);
  digitalWrite(ledPin_White,LOW);
  delay(200);
  }

  
  digitalWrite(10,HIGH);
  analogWrite(11,100);
  digitalWrite(12,LOW);
  digitalWrite(7,HIGH);
  analogWrite(9,100);
  digitalWrite(6,LOW); 
  delay(500);

  
  digitalWrite(10,LOW);
  analogWrite(11,100);
  digitalWrite(12,HIGH);
  digitalWrite(7,LOW);
  analogWrite(9,100);
  digitalWrite(6,HIGH);
  delay(500);

  digitalWrite(12,LOW);
  analogWrite(11,0);
  digitalWrite(10,LOW);
  digitalWrite(6,LOW);
  analogWrite(9,0);
  digitalWrite(7,LOW);

}

void setup()
{
  // initialize serial:
  init_devices();
  Serial.begin(9600);
  //reserve 200 bytes for the inputString:
  inputString.reserve(200);
  led_dance();

}

/*
 * Closed loop P controller of the robot parses strings from the LiFi receiver in UART 
 */
void loop() {
  serialEvent(); //call the function
  // print the string when a newline arrives:
  //digitalWrite(ledPin_White,LOW);
  if (stringComplete)
  {
    digitalWrite(ledPin_Yellow, HIGH);

    //Serial.println(inputString);

    String string_size_packet = inputString.substring(2, 4);
    int size_packet = string_size_packet.toInt();
    //Serial.println(size_packet);
    if (size_packet == 25)
    {
      bot_stop();
      digitalWrite(ledPin_Red, HIGH);
    }
    if (size_packet == 30)
    {
      String string_theta = inputString.substring(5, 8);
      theta = string_theta.toInt();
      digitalWrite(ledPin_Red, LOW);
      theta = theta - 180;
      if (theta > 85)
      {
        analogWrite(pwm1, tune*84);
        analogWrite(pwm2, tune*84);
        bot_spot_clock();
      }
      else if (theta < -85)
      {
        analogWrite(pwm1, tune*84);
        analogWrite(pwm2, tune*84);
        bot_spot_anti_clock();
      }
      else
      {
        pwm_this_shit(theta);
        Serial.println(theta);
      }
    }
    inputString = "";
    stringComplete = false;
  }
}

/*
 * The lengendary parser stolen from Arduino's examples 
 */
void serialEvent() {
  while (Serial.available())
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    digitalWrite(ledPin_Yellow, LOW);
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
