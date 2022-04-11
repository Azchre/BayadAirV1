#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>

#include <SoftwareSerial.h>
#include <Keypad.h>

int stage = 1;
#include <FPM.h>

const byte ROWS = 4;
const byte COLS = 3;

SoftwareSerial fserial(11, 12);
FPM finger(&fserial);
FPM_System_Params params;

char hexaKeys[ROWS][COLS] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
  };

byte rowPins[ROWS] = {35,33,31,29};
byte colPins[COLS] = {27,25,23};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd2(rs, en, d4, d5, d6, d7); //Top LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Bottom

void setup() {
  Serial.begin(9600);
  lcd.init();  
  lcd2.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd2.setCursor(0, 0);
  lcd.print("Welcome to");
  lcd2.print("Welcome to");
  lcd.setCursor(4,1);
  lcd2.setCursor(4,1);
  lcd.print("BAYAD AIR!");
  lcd2.print("BAYAD AIR!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("1: AUTHENTICATE");
  lcd.setCursor(0,1);
  lcd.print("2: REGISTER");
}

void loop() {
 
  char customKey = customKeypad.getKey();
  if(customKey){
    if(stage == 1) {
 //To Authenticate with Fingerprint
        if(customKey = 1){
          stage = 2;
          configureFingerprint();

        }
  //To enroll fingerprint and Register User
        else {
          stage = 2;
          configureFingerprint();
          lcd.print("SCAN FINGERPRINT"); 
        }     
     };
  };
}

void configureFingerprint(){
  bool isConfiguring = true;
          while(isConfiguring){   
          lcd.clear();
          lcd.setCursor(0,0);     
          lcd.print("Please Wait...");   
          fserial.begin(57600);

             if (finger.begin()) {
               isConfiguring = false;
               finger.readParams(&params);
               lcd.clear();
               lcd.setCursor(2,0);
               lcd.print("Found Sensor");
            }else {
                lcd.clear();
                lcd.setCursor(0,0);   
                lcd.print("NoSensorFound:(");
                lcd.setCursor(0,1);
                lcd.print("Retrying...");
                delay(2000);
          }
         }
}

//fOR ENROLLING FINGERPRINT
int16_t enroll_finger(int16_t fid) {
    int16_t p = -1;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Waiting For");
  lcd.setCursor(0,1);
  lcd.print("Fingerprint");
    while (p != FPM_OK) {
        p = finger.getImage();
        switch (p) {
            case FPM_OK:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Image taken");
                break;
            case FPM_NOFINGER:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("No Finger");
                lcd.setCursor(0,1);
                lcd.print("Detected");
                break;
            case FPM_PACKETRECIEVEERR:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Communication");
                lcd.setCursor(0,1);
                lcd.print("Error");
                break;
            case FPM_IMAGEFAIL:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Imaging error");
                break;
            case FPM_TIMEOUT:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Timeout!");
                break;
            case FPM_READ_ERROR:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Got wrong PID");
                lcd.setCursor(0,1);
                lcd.print("or length!");
                break;
            default:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Unknown error");
                break;
        }
        yield();
    }
    // OK SUCCESS!
    p = finger.image2Tz(1);
    switch (p) {
        case FPM_OK:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Image converted");
            break;
        case FPM_IMAGEMESS:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Image too messy");
            return p;
        case FPM_PACKETRECIEVEERR:
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("Communication");
             lcd.setCursor(0,1);
             lcd.print("Error");
            return p;
        case FPM_FEATUREFAIL:
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("CouldNotFindFing");
             lcd.setCursor(0,1);
             lcd.print("-erprintFeatures");
            
            return p;
        case FPM_INVALIDIMAGE:
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("CouldNotFindFing");
             lcd.setCursor(0,1);
             lcd.print("-erprintFeatures");          
            return p;
        case FPM_TIMEOUT:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Timeout!");
            return p;
        case FPM_READ_ERROR:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Got wrong PID");
            lcd.setCursor(0,1);
            lcd.print("or length!");
            return p;
        default:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Unknown error");
            return p;
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Remove finger");
    delay(2000);
    p = 0;
    while (p != FPM_NOFINGER) {
        p = finger.getImage();
        yield();
    }

    p = -1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Place same");
    lcd.setCursor(0,1);
    lcd.print("finger again");
    while (p != FPM_OK) {
        p = finger.getImage();
        switch (p) {
            case FPM_OK:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Image taken");
                break;
            case FPM_NOFINGER:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("No Finger");
                lcd.setCursor(0,1);
                lcd.print("Detected");
                break;
            case FPM_PACKETRECIEVEERR:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Communication");
                lcd.setCursor(0,1);
                lcd.print("Error");
                break;
            case FPM_IMAGEFAIL:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Imaging error");
                break;
            case FPM_TIMEOUT:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Timeout!");
                break;
            case FPM_READ_ERROR:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Got wrong PID");
                lcd.setCursor(0,1);
                lcd.print("or length!");
                break;
            default:
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Unknown error");
                break;
        }
        yield();
    }

    // OK SUCCESS!

    p = finger.image2Tz(2);
    switch (p) {
        case FPM_OK:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Image converted");
            break;
        case FPM_IMAGEMESS:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Image too messy");
            return p;
        case FPM_PACKETRECIEVEERR:
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("Communication");
             lcd.setCursor(0,1);
             lcd.print("Error");
            return p;
        case FPM_FEATUREFAIL:
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("CouldNotFindFing");
             lcd.setCursor(0,1);
             lcd.print("-erprintFeatures");   
            return p;
        case FPM_INVALIDIMAGE:
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("CouldNotFindFing");
             lcd.setCursor(0,1);
             lcd.print("-erprintFeatures");   
            return p;
        case FPM_TIMEOUT:
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("Timeout!");
            return false;
        case FPM_READ_ERROR:
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("Got wrong PID");
             lcd.setCursor(0,1);
             lcd.print("or length!");
            return false;
        default:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Unknown error");
            return p;
    }


    // OK converted!
    p = finger.createModel();
    if (p == FPM_OK) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Prints matched!");
    } else if (p == FPM_PACKETRECIEVEERR) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Communication");
        lcd.setCursor(0,1);
        lcd.print("error");
        return p;
    } else if (p == FPM_ENROLLMISMATCH) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Fingerprints ");
        lcd.setCursor(0,1);
        lcd.print("did not match");
        return p;
    } else if (p == FPM_TIMEOUT) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Timeout!");
        return p;
    } else if (p == FPM_READ_ERROR) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Got wrong PID");
        lcd.setCursor(0,1);
        lcd.print("or length!");
        return p;
    } else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Unknown error");
        return p;
    }

    Serial.print("ID "); Serial.println(fid);
    p = finger.storeModel(fid);
    if (p == FPM_OK) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Stored!");
        return 0;
    } else if (p == FPM_PACKETRECIEVEERR) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Communication");
        lcd.setCursor(0,1);
        lcd.print("Error");
        return p;
    } else if (p == FPM_BADLOCATION) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Could not store");    
        return p;
    } else if (p == FPM_FLASHERR) {
        lcd.clear();
        lcd.setCursor(0,0);  
        lcd.print("Error writing");
        lcd.setCursor(0,1);
        lcd.print("to flash");
        return p;
    } else if (p == FPM_TIMEOUT) {
        lcd.clear();
        lcd.setCursor(0,0);  
        lcd.print("Timeout!");
        return p;
    } else if (p == FPM_READ_ERROR) {
        lcd.clear();
        lcd.setCursor(0,0);  
        lcd.print("Got wrong PID");
        lcd.setCursor(0,1);  
        lcd.print("or length!");
        return p;
    } else {
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print("Unknown error");
        return p;
    }
}
