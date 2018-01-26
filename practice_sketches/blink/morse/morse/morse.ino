#include <stdio.h>
#include <string.h>

const int CHAR_2_MORSE[][4] = {
  //  A    ,     B    ,     C    ,     D    ,     E    ,     F    ,     G    ,     H    ,     I    ,     J    ,     K    ,     L    ,     M
  {1,3,0,0}, {3,1,1,1}, {3,1,3,1}, {3,1,1,0}, {1,0,0,0}, {1,1,3,1}, {3,3,1,0}, {1,1,1,1}, {1,1,0,0}, {1,3,3,3}, {3,1,3,0}, {1,3,1,1}, {3,3,0,0},
  //  N    ,     O    ,     P    ,     Q    ,     R    ,     S    ,     T    ,     U    ,     V    ,     W    ,     X    ,     Y    ,     Z
  {3,1,0,0}, {3,3,3,0}, {1,3,3,1}, {3,3,1,3}, {1,3,1,0}, {1,1,1,0}, {3,0,0,0}, {1,1,3,0}, {1,1,1,3}, {1,3,3,0}, {3,1,1,3}, {3,1,3,3}, {3,3,1,1}
};

const int DOT_TIME = 300;
const int LED_PIN = 13;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(9600);
}

void loop()
{
  char text_to_blink[] = "hello world"; //String to be converted.
  
  char c;
  
  for(int i = 0; i < strlen(text_to_blink); i++)
  {
    c = text_to_blink[i];
    
    if(c>='a' && c<='z'){
      mctoblink(c-32);
    }
    else if (c>='A' && c<='Z' || c == ' '){
      mctoblink(c);
    }
    else {
      //Do nothing.
    }
   
  }

  Serial.println();
  digitalWrite(LED_PIN, LOW);
  delay(3000); // Wait for 3000 millisecond(s)(3 seconds)
}

void mctoblink(char c)
{
  //Serial.print(c);
  if(c == ' '){
    for(int i = 0; i < 7; i++){
      digitalWrite(LED_PIN, HIGH);
      delay(DOT_TIME);
      digitalWrite(LED_PIN, LOW);
      delay(DOT_TIME);
    }
    Serial.print("  /  ");
  } else {
    Serial.print("(");
    Serial.print(c);
    Serial.print(")");
    Serial.print("{");
    for(int i = 0; i < 4; i++){
      digitalWrite(LED_PIN, HIGH);
      delay(CHAR_2_MORSE[c-65][i] * DOT_TIME);
      Serial.print(CHAR_2_MORSE[c-65][i]);
      digitalWrite(LED_PIN, LOW);
      delay(DOT_TIME);
      if(i < 3)
        Serial.print(",");
    }
      Serial.print("}  ");
      digitalWrite(LED_PIN, LOW);
      delay(DOT_TIME * 3);
  }

  return;
}
