#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Keyboard.h>

// Hardware configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Pin definitions
#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define JOYSTICK_BUTTON_PIN 2

// I2C pins
#define SDA_PIN 8
#define SCL_PIN 9

// Menu configuration
#define MENU_ITEMS 29
#define MAX_DISPLAY_ITEMS 2
#define DEBOUNCE_DELAY 50
#define SCROLL_DELAY_INITIAL 200
#define SCROLL_DELAY_ACCELERATED 200
#define TYPING_DELAY 15

// OLED display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Cheat code structure
struct CheatCode {
  const char* name;
  const char* code;
  const char* description;
};

// GTA V Cheat Codes
const CheatCode cheatCodes[MENU_ITEMS] = {
  {"PAINKILLER", "PAINKILLER", "Full Armor & Health"},
  {"TURTLE", "TURTLE", "Full Armor"},
  {"POWERUP", "POWERUP", "Explosive Melee"},
  {"CATCHME", "CATCHME", "Super Run"},
  {"GOTGILLS", "GOTGILLS", "Infinite Oxygen"},
  {"HOPTOIT", "HOPTOIT", "Super Jump"},
  {"HOTHANDS", "HOTHANDS", "Explosive Punches"},
  {"HIGHEX", "HIGHEX", "Explosive Bullets"},
  {"INCENDIARY", "INCENDIARY", "Incendiary Bullets"},
  {"LIQUOR", "LIQUOR", "Drunk Mode"},
  {"SKYFALL", "SKYFALL", "Skyfall Spawn"},
  {"SLOWMO", "SLOWMO", "Slow Motion"},
  {"LAWYERUP", "LAWYERUP", "Wanted Level Down"},
  {"FUGITIVE", "FUGITIVE", "Max Wanted Level"},
  {"DEADEYE", "DEADEYE", "Slow Motion Aim"},
  {"SNOWDAY", "SNOWDAY", "Snowy Weather"},
  {"MAKEITRAIN", "MAKEITRAIN", "Change Weather"},
  {"BANDIT", "BANDIT", "Spawn Dune Buggy"},
  {"COMET", "COMET", "Spawn Comet Car"},
  {"ROCKET", "ROCKET", "Spawn Rapid GT"},
  {"OFFROAD", "OFFROAD", "Spawn Sanchez"},
  {"RAPIDGT", "RAPIDGT", "Spawn Rapid GT"},
  {"VINEWOOD", "VINEWOOD", "Spawn Limo"},
  {"TRASHED", "TRASHED", "Spawn Trashmaster"},
  {"BARNSTORM", "BARNSTORM", "Spawn Duster"},
  {"BUZZOFF", "BUZZOFF", "Spawn Buzzard"},
  {"DEATHCAR", "DEATHCAR", "Spawn Ruiner 2000"},
  {"BUBBLES", "BUBBLES", "Spawn Stunt Plane"},
  {"EXTINCT", "EXTINCT", "Spawn Dodo Plane"}
};

// Menu variables
int currentSelection = 0;
int scrollOffset = 0;
unsigned long lastScrollTime = 0;
unsigned long lastButtonPress = 0;
int scrollDelay = SCROLL_DELAY_INITIAL;
bool buttonState = HIGH;
bool lastButtonState = HIGH;
bool keyboardReady = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting GTA V Cheat Device...");
  
  initializePins();
  initializeDisplay();
  initializeKeyboard();
  
  showSplashScreen();
  updateDisplay();
}

void loop() {
  handleJoystick();
  handleButton();
  delay(10);
}

void initializePins() {
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void initializeDisplay() {
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed!");
    while(1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  Serial.println("OLED initialized successfully!");
}

void initializeKeyboard() {
  Keyboard.begin();
  delay(1000); // Wait for USB to initialize
  keyboardReady = true;
  Serial.println("Keyboard initialized - READY!");
}

void showSplashScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("GTA V Cheat Device");
  display.println("v2.0 - WORKING!");
  display.println();
  display.println("Keyboard Ready");
  display.println("Test in GTA V!");
  display.display();
  delay(2000);
}

void handleJoystick() {
  int yValue = analogRead(JOYSTICK_Y_PIN);
  unsigned long currentTime = millis();
  
  if (currentTime - lastScrollTime < scrollDelay) return;
  
  bool scrolled = false;
  
  if (yValue < 100) {
    currentSelection--;
    if (currentSelection < 0) currentSelection = MENU_ITEMS - 1;
    scrolled = true;
  } else if (yValue > 900) {
    currentSelection++;
    if (currentSelection >= MENU_ITEMS) currentSelection = 0;
    scrolled = true;
  }
  
  if (scrolled) {
    updateDisplay();
    lastScrollTime = currentTime;
    scrollDelay = SCROLL_DELAY_ACCELERATED;
  } else {
    scrollDelay = SCROLL_DELAY_INITIAL;
  }
}

void handleButton() {
  bool reading = digitalRead(JOYSTICK_BUTTON_PIN);
  
  if (reading != lastButtonState) {
    lastButtonPress = millis();
  }
  
  if ((millis() - lastButtonPress) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        executeCheat(currentSelection);
      }
    }
  }
  
  lastButtonState = reading;
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Show status at top
  display.setCursor(0, 0);
  if (keyboardReady) {
    display.println("KEYBOARD: READY ✓");
  } else {
    display.println("KEYBOARD: NOT READY");
  }
  
  // Draw separator line
  display.drawLine(0, 15, 128, 15, SSD1306_WHITE);
  
  if (currentSelection < scrollOffset) {
    scrollOffset = currentSelection;
  } else if (currentSelection >= scrollOffset + MAX_DISPLAY_ITEMS) {
    scrollOffset = currentSelection - MAX_DISPLAY_ITEMS + 1;
  }
  
  for (int i = 0; i < MAX_DISPLAY_ITEMS; i++) {
    int itemIndex = scrollOffset + i;
    if (itemIndex >= MENU_ITEMS) break;
    
    display.setCursor(0, 16 + i * 18);
    
    if (itemIndex == currentSelection) {
      display.print("-> ");
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.print("   ");
      display.setTextColor(SSD1306_WHITE);
    }
    
    display.print(cheatCodes[itemIndex].name);
    display.print(" - ");
    
    String desc = String(cheatCodes[itemIndex].description);
    if (desc.length() > 14) {
      desc = desc.substring(0, 14) + "...";
    }
    display.print(desc);
  }
  
  // Page indicator at bottom
  if (MENU_ITEMS > MAX_DISPLAY_ITEMS) {
    display.setCursor(100, 56);
    display.print(scrollOffset + 1);
    display.print("/");
    display.print(MENU_ITEMS);
  }
  
  display.display();
}

void executeCheat(int selection) {
  if (!keyboardReady) {
    showError("KEYBOARD NOT READY", "Replug device");
    delay(2000);
    updateDisplay();
    return;
  }
  
  showSendingAnimation();
  
  // Send tilde key to open GTA V console
  Keyboard.press('`'); // GTA V console key (tilde/backtick)
  delay(100);
  Keyboard.releaseAll();
  delay(400);
  
  // Type the cheat code in UPPERCASE
  Keyboard.print(cheatCodes[selection].code);
  delay(300);
  
  // Press Enter to activate cheat
  Keyboard.press(KEY_RETURN);
  delay(100);
  Keyboard.releaseAll();
  delay(300);
  
  showSuccessIndicator();
  delay(1000);
  updateDisplay();
}

void showSendingAnimation() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print("SENDING: ");
  display.println(cheatCodes[currentSelection].name);
  display.display();
  
  // Simple animation
  for (int i = 0; i < 3; i++) {
    display.setCursor(50, 40);
    display.print("   ");
    display.setCursor(50, 40);
    for (int j = 0; j <= i; j++) display.print(".");
    display.display();
    delay(200);
  }
}

void showSuccessIndicator() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("CHEAT ACTIVATED!");
  display.println();
  display.println("Check GTA V");
  display.display();
}

void showError(const char* line1, const char* line2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(line1);
  display.println(line2);
  display.display();
}