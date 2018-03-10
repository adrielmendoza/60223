#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const int Vbw = A0;
const int Vps = A1;
const int switchPin = 10;
const int noButton = 9;
const int yesButton = 8;

long currRVal = 0;
long lastSeen[10] = {};
int nextPos = 0;
bool anyZero = false;
long currAvg = 0;

void setup()
{
  pinMode(Vbw, INPUT);
  pinMode(Vps, INPUT);
  pinMode(switchPin, INPUT);
  pinMode(yesButton, INPUT);
  pinMode(noButton, INPUT);
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("Please supply");
  lcd.setCursor(3,1);
  lcd.print("resistance");
  Serial.begin(9600);
}


void loop()
{  
  int switchVal = digitalRead(switchPin);
  Serial.print(switchVal);
  // RESISTOR INPUT MODE
  if (switchVal)
  {
    double voltVal = analogRead(Vbw);
    double voltSupplyVal = analogRead(Vps);
    double actualVoltVal = 5.0 * voltVal / voltSupplyVal;
    Serial.print("actualVoltVal: ");
    Serial.print(actualVoltVal);
    //Serial.println(voltSupplyVal);
    long newRVal = 0;
    if (actualVoltVal < 5) 
    {
      // Resistance equation
      double coeff = 5.0 / (5.0 - actualVoltVal) - 1.0;
      Serial.print(", coeff: ");
      Serial.print(coeff);
      newRVal = 10000 * coeff;
      Serial.print(", newRVal: ");
      Serial.print(newRVal);
    }
  
    // add newRVal to lastSeen array
    lastSeen[nextPos] = newRVal;
    
    // move position to be added in lastSeen
    nextPos = (nextPos == 9) ? 0 : nextPos + 1;
  
    // check if any zeros in lastSeen
    for (int i = 0; i < 10 && !anyZero; i++) {
      if (lastSeen[i] == 0) 
      {
        anyZero = true;
      }
    }
  
    long average = 0;
  
    // if no zeroes in lastSeen, calculate average of lastSeen
    if (!anyZero)
    {
      long sum = 0;
      for (int i = 0; i < 10; i++) {
        sum = sum + lastSeen[i];
      }
      average = sum / 10;
      if (currAvg != average && average) {
        currRVal = newRVal;
        currAvg = average;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Resistor Value:");
        lcd.setCursor(0,1);
        lcd.print(currRVal);
        lcd.print(" -> ");
        average ? lcd.print(makeGuess(average)) : lcd.print("?");
      }
    }
  
    // reset anyZero
    anyZero = false;
  
    Serial.print(", average: ");
    Serial.println(average);
    
    if (currRVal != newRVal)
    {    
      currRVal = newRVal;
      lcd.clear();
      if (newRVal > 0) 
      {
        
        lcd.setCursor(0,0);
        lcd.print("Resistor Value:");
        lcd.setCursor(0,1);
        lcd.print(currRVal);
        lcd.print(" -> ");
        average ? lcd.print(makeGuess(average)) : lcd.print("?");
      }
      else
      {
        // reset lastSeen array
        for (int i = 0; i < 10; i++) {
          lastSeen[i] = 0;
        }
        lcd.setCursor(1,0);
        lcd.print("Please supply");
        lcd.setCursor(3,1);
        lcd.print("resistance");
      }
    }
    delay(500);
  }
  // COLOR CODE INPUT MODE
  else
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("First color:");
    String color1 = askColor();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Second color:");
    String color2 = askColor();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Third color:");
    String color3 = askColor(); 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(color1);
    lcd.print(" ");
    lcd.print(color2);
    lcd.setCursor(0,1);
    lcd.print(color3);
    delay(1000);
    lcd.print(" -> ");
    lcd.print(colorsToResistor(color1, color2, color3));
    while (!digitalRead(switchPin) && !digitalRead(yesButton)
       && !digitalRead(noButton)) {
        Serial.println("still here");
    }
    while (digitalRead(yesButton)) {}
    lcd.clear();
    }
}

// given an average, guess what the color code resistance is 
String makeGuess (long average) 
{
  if (0 < average && average < 30) 
    return "10"; 
  else if (30 <= average && average < 50)
    return "33";
  else if (50 <= average && average < 70)
    return "47";
  else if (70 <= average && average < 120)
    return "100";
  else if (120 <= average && average < 200)
    return "150";
  else if (200 <= average && average < 240)
    return "220";
  else if (240 <= average && average < 300)
    return "270";
  else if (300 <= average && average < 400)
    return "330";
  else if (400 <= average && average < 575)
    return "470";
  else if (575 <= average && average < 840)
    return "680"; 
  else if (840 <= average && average < 1250)
    return "1K";
  else if (1250 <= average && average < 1750)
    return "1.5K";
  else if (1750 <= average && average < 2100)
    return "2K";
  else if (2100 <= average && average < 2450)
    return "2.2K";
  else if (2450 <= average && average < 2850)
    return "2.7K";
  else if (2850 <= average && average < 4000)
    return "3.3K";
  else if (4000 <= average && average < 4900)
    return "4.7K";
  else if (4900 <= average && average < 5350)
    return "5.1K";
  else if (5350 <= average && average < 7800)
    return "5.6K";
  else if (7800 <= average && average < 12500)
    return "10K";
  else if (12500 <= average && average < 18500)
    return "15K";
  else if (18500 <= average && average < 27500)
    return "22K";
  else if (27500 <= average && average < 40000)
    return "33K";
  else if (40000 <= average && average < 73500)
    return "47K";
  else if (73500 <= average && average < 285000)
    return "100K";
  else if (285000 <= average && average < 735000)
    return "470K";
  else if (735000 <= average && average < 5000000)
    return "1M";
  else if (average >= 5000000)
    return "10M";
  else 
    return "???";
}

String askColor ()
{
  while (digitalRead(noButton)) {
    if (digitalRead(switchPin)) {
      return "X";
    }
  }
  lcd.setCursor(0,1);
  lcd.print("BLACK?");
  // pause execution until either button is pressed
  while (buttonsOpen()) {
    if (digitalRead(switchPin)) {
      return "X";
    }
  }
  if (digitalRead(yesButton)) 
  {
    lcd.setCursor(0,1);
    lcd.print("BLACK!");
    delay(1000);
    // wait for button to be unpressed
    while (digitalRead(yesButton)) {
      if (digitalRead(switchPin)) {
        break;
      }
    }
    return "BLACK";
  }
  else if (digitalRead(noButton))
  {
    while (digitalRead(noButton)) {
      if (digitalRead(switchPin)) {
        return "X";
      }
    }
    lcd.setCursor(0,1);
    lcd.print("BROWN?");
    while (buttonsOpen()) {
      if (digitalRead(switchPin)) {
        return "X";
      }
    }
    if (digitalRead(yesButton)) 
    {
      lcd.setCursor(0,1);
      lcd.print("BROWN!");
      delay(1000);
      // wait for button to be unpressed
      while (digitalRead(yesButton)) {
        if (digitalRead(switchPin)) {
          return "X";
        } 
      }
      return "BROWN";
    }
    else if (digitalRead(noButton))
    {
      while (digitalRead(noButton)) {
        if (digitalRead(switchPin)) {
          return "X";
        } 
      }
      lcd.setCursor(0,1);
      lcd.print("RED?  ");
      while (buttonsOpen()) {
        if (digitalRead(switchPin)) {
          return "X";
        } 
      }
      if (digitalRead(yesButton)) 
      {
        lcd.setCursor(0,1);
        lcd.print("RED!");
        delay(1000);
        // wait for button to be unpressed
        while (digitalRead(yesButton)) {
          if (digitalRead(switchPin)) {
            return "X";
          } 
        }
        return "RED";
      }
      else if (digitalRead(noButton))
      {
        while (digitalRead(noButton)) {
          if (digitalRead(switchPin)) {
            return "X";
          } 
        }
        lcd.setCursor(0,1);
        lcd.print("ORANGE?");
        while (buttonsOpen()) {
          if (digitalRead(switchPin)) {
            return "X";
          } 
        }
        if (digitalRead(yesButton)) 
        {
          lcd.setCursor(0,1);
          lcd.print("ORANGE!");
          delay(1000);
          // wait for button to be unpressed
          while (digitalRead(yesButton)) {
            if (digitalRead(switchPin)) {
              return "X";
            } 
          }
          return "ORANGE";
        }
        else if (digitalRead(noButton))
        {
          while (digitalRead(noButton)) {
            if (digitalRead(switchPin)) {
              return "X";
            } 
          }
          lcd.setCursor(0,1);
          lcd.print("YELLOW?");
          while (buttonsOpen()) {
            if (digitalRead(switchPin)) {
              return "X";
            } 
          }
          if (digitalRead(yesButton)) 
          {
            lcd.setCursor(0,1);
            lcd.print("YELLOW!");
            delay(1000);
            // wait for button to be unpressed
            while (digitalRead(yesButton)) {
              if (digitalRead(switchPin)) {
                return "X";
              } 
            }
            return "YELLOW";
          }
          else if (digitalRead(noButton)) 
          {
            while (digitalRead(noButton)) {
              if (digitalRead(switchPin)) {
                return "X";
              } 
            }
            lcd.setCursor(0,1);
            lcd.print("GREEN? ");
            while (buttonsOpen()) {
              if (digitalRead(switchPin)) {
                return "X";
              } 
            }
            if (digitalRead(yesButton)) 
            {
              lcd.setCursor(0,1);
              lcd.print("GREEN!");
              delay(1000);
              // wait for button to be unpressed
              while (digitalRead(yesButton)) {
                if (digitalRead(switchPin)) {
                  return "X";
                } 
              }
              return "GREEN";
            }
            else if (digitalRead(noButton))
            {
              while (digitalRead(noButton)) {
                if (digitalRead(switchPin)) {
                  return "X";
                } 
              }
              lcd.setCursor(0,1);
              lcd.print("BLUE?   ");
              while (buttonsOpen()) {
                if (digitalRead(switchPin)) {
                  return "X";
                } 
              }
              if (digitalRead(yesButton)) 
              {
                lcd.setCursor(0,1);
                lcd.print("BLUE!");
                delay(1000);
                // wait for button to be unpressed
                while (digitalRead(yesButton)) {
                  if (digitalRead(switchPin)) {
                    return "X";
                  } 
                }
                return "BLUE";
              }
              else if (digitalRead(noButton))
              {
                while (digitalRead(noButton)) {
                  if (digitalRead(switchPin)) {
                    return "X";
                  } 
                }
                lcd.setCursor(0,1);
                lcd.print("VIOLET?");
                while (buttonsOpen()) {
                  if (digitalRead(switchPin)) {
                    return "X";
                  } 
                }
                if (digitalRead(yesButton)) 
                {
                  lcd.setCursor(0,1);
                  lcd.print("VIOLET!");
                  delay(1000);
                  // wait for button to be unpressed
                  while (digitalRead(yesButton)) {
                    if (digitalRead(switchPin)) {
                      return "X";
                    } 
                  }
                  return "VIOLET";
                }
                else if (digitalRead(noButton))
                {
                  while (digitalRead(noButton)) {
                    if (digitalRead(switchPin)) {
                      return "X";
                    } 
                  }
                  lcd.setCursor(0,1);
                  lcd.print("GRAY?  ");
                  while (buttonsOpen()) {
                    if (digitalRead(switchPin)) {
                      return "X";
                    } 
                  }
                  if (digitalRead(yesButton)) 
                  {
                    lcd.setCursor(0,1);
                    lcd.print("GRAY!");
                    delay(1000);
                    // wait for button to be unpressed
                    while (digitalRead(yesButton)) {
                      if (digitalRead(switchPin)) {
                        return "X";
                      } 
                    }
                    return "GRAY";
                  }
                  else if (digitalRead(noButton))
                  {
                    while (digitalRead(noButton)) {
                      if (digitalRead(switchPin)) {
                        return "X";
                      } 
                    }
                    lcd.setCursor(0,1);
                    lcd.print("WHITE?");
                    while (buttonsOpen()) {
                      if (digitalRead(switchPin)) {
                        return "X";
                      } 
                    }
                    if (digitalRead(yesButton)) 
                    {
                      lcd.setCursor(0,1);
                      lcd.print("WHITE!");
                      delay(1000);
                      // wait for button to be unpressed
                      while (digitalRead(yesButton)) {
                        if (digitalRead(switchPin)) {
                          return "X";
                        } 
                      }
                      return "WHITE";
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  // restart if all NO's
  return askColor();
}

bool buttonsOpen ()
{
  if (digitalRead(yesButton) || digitalRead(noButton)) 
    return false;
  return true;
}

String colorsToResistor 
(String color1, String color2, String color3)
{
  int firstTwo = colorToLong(color1) * 10 + colorToLong(color2);
  int exponent = colorToLong(color3);
  
  if (exponent == 2) 
    return String(colorToLong(color1)) + "." 
         + String(colorToLong(color2)) + "K";
  if (exponent == 3)
    return String(firstTwo) + "K";
  if (exponent == 4)
    return String(colorToLong(color1)) 
         + String(colorToLong(color2)) + "0K";
  if (exponent == 5)
    return String(colorToLong(color1)) + "." 
         + String(colorToLong(color2)) + "M";  
  if (exponent == 6)
    return String(firstTwo) + "M";    
  if (exponent == 7)
    return String(colorToLong(color1)) 
         + String(colorToLong(color2)) + "0M"; 
  if (exponent == 8)
    return String(colorToLong(color1)) + "." 
         + String(colorToLong(color2)) + "B";   
  if (exponent == 9)
    return String(firstTwo) + "B";   
  long ans = firstTwo * pow(10,exponent);
  return String(ans);
}

long colorToLong (String color)
{
  if (color == "BLACK")
    return 0;
  else if (color == "BROWN")
    return 1;
  else if (color == "RED")
    return 2;
  else if (color == "ORANGE")
    return 3;
  else if (color == "YELLOW")
    return 4;
  else if (color == "GREEN")
    return 5;
  else if (color == "BLUE")
    return 6;
  else if (color == "VIOLET")
    return 7;
  else if (color == "GRAY")
    return 8;
  // should be WHITE
  return 9;
}

