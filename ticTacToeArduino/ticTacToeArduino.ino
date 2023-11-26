#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SDA 13 // Define SDA pins
#define SCL 14 // Define SCL pins
LiquidCrystal_I2C lcd(0x27, 16, 2);

char board[3][3];
int xWins = 0;
int oWins = 0;

void setup() {
  Wire.begin(SDA, SCL); // attach the IIC pin
  if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }
  lcd.init();       // LCD driver initialization
  lcd.backlight();  // Open the backlight
  lcd.setCursor(0, 0);
}

void loop() {
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
        displayWinner(winner);
        break;
      }

    }

    delay(300); // Delay between games
  }
  lcd.setCursor(12, 1);
  lcd.print("DONE");
  delay(1000000);
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

  return ' '; // No winner yet
}

// Function to display the winner on the LCD
void displayWinner(char winner) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("X wins: " + String(xWins));
  lcd.setCursor(0, 1);
  lcd.print("O wins: " + String(oWins));
}
bool i2CAddrTest(uint8_t addr) {
  Wire.begin();
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  return false;
}


