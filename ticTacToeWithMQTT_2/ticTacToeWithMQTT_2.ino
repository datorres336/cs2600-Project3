#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <WiFi.h>

#define SDA 13 // Define SDA pins
#define SCL 14 // Define SCL pins

// Update with your WiFi credentials
const char* ssid = "Frontier9216";
const char* password = "7748652375";

const char* mqttServer = "davidtorres.duckdns.org";
const int mqttPort = 1883;
const char* mqttTopic = "ticTacToe2";

WiFiClient espClient;
PubSubClient client(espClient);

LiquidCrystal_I2C lcd(0x27, 16, 2);

char board[3][3];
int xWins = 0;
int oWins = 0;
int draws = 0;
String message = "";

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to MQTT broker
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(mqttTopic); // Subscribe to the desired topic
    } else {
      Serial.print("Failed to connect to MQTT broker, retrying in 5 seconds...");
      delay(5000);
    }
  }

  Wire.begin(SDA, SCL); // attach the IIC pin
  if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }
  lcd.init();       // LCD driver initialization
  lcd.backlight();  // Open the backlight
  lcd.setCursor(0, 0);
} // end void setup()

void loop() {
  client.loop();

  // Simulate 100 games
  for (int game = 1; game <= 100; game++) {
    // Initialize the board
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        board[i][j] = ' ';
      }
    }

    // Simulate player moves
    for (int move = 1; move <= 9; move++) {
      // Alternate between Player X and Player O
      if (move % 2 == 1) {
        playerXMove();
      } else {
        playerOMove();
      }

      // Check for a winner after each move
      char winner = checkWinner();
      if (winner != ' ') {
        //displayWinner(winner);
        break;
      }

    }
    //Publish the current state to the MQTT topic
    publishGameState();
    //client.publish(mqttTopic, "test publish");

    delay(300); // Delay between games
  }
  lcd.setCursor(12, 1);
  //lcd.print("DONE");
  delay(1000);
}

void publishGameState() {
  // Prepare the message content (you might want to adjust this based on your needs)
  message = "WX:" + String(xWins) + " WO:" + String(oWins) + " D:" + String(draws);
  Serial.println("REAL VALUES???? " + message);
  // Publish the message to the MQTT topic
  client.publish(mqttTopic, message.c_str());
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.println("Message received!");
  
  // This function is called when a message is received on the subscribed topic
  payload[length] = '\0';  // Null-terminate the payload to make it a valid string
  String message2 = String((char*)payload);

  Serial.print("Received message on topic ");
  Serial.print(topic);
  Serial.println(": ");
  Serial.println(message2);

  // Update the LCD with the received data
  updateLCD(message);
}

void updateLCD(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Received Data:");
  lcd.setCursor(0, 1);
  lcd.print(message);
}

// Function to check if a position on the board is empty
bool isPositionEmpty(int row, int col) {
  return (board[row][col] == ' ');
}

// Function for Player X to make a move
void playerXMove() {
  int row, col;

  do {
    // Randomly select a position
    row = random(3);
    col = random(3);
  } while (!isPositionEmpty(row, col));

  // Place X on the board
  board[row][col] = 'X';
}

// Function for Player O to make a move
void playerOMove() {
  int row, col;

  do {
    // Randomly select a position
    row = random(3);
    col = random(3);
  } while (!isPositionEmpty(row, col));

  // Place O on the board
  board[row][col] = 'O';
}

// Function to check for a winner
char checkWinner() {
  // Check rows and columns
  for (int i = 0; i < 3; i++) {
    if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') {
      if (board[i][0] == 'O') oWins++;
      else xWins++;
      return board[i][0]; // Winner in a row
    }
    if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ') {
      if (board[i][0] == 'O') oWins++;
        else xWins++;
      return board[0][i]; // Winner in a column
    }
  }

  // Check diagonals
  if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') {
    if (board[0][0] == 'O') oWins++;
      else xWins++;
    return board[0][0]; // Winner in the main diagonal
  }
  if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ') {
    if (board[2][0] == 'O') oWins++;
      else xWins++;
    return board[0][2]; // Winner in the other diagonal
  }

  if (isBoardFull()) {
    draws++;
  }
  return ' '; // No winner yet
}

// Function to display the winner on the LCD
void displayWinner(char winner) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Xwin " + String(xWins));
  lcd.setCursor(0, 1);
  lcd.print("Owin " + String(oWins));

  lcd.setCursor(8, 0);
  lcd.print("Draws " + String(draws));
}
bool i2CAddrTest(uint8_t addr) {
  Wire.begin();
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  return false;
}

bool isBoardFull() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (board[i][j] == ' ') {
        return false; // There is an empty position
      }
    }
  }
  return true; // Board is full
}


