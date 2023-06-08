//#  MathTricks 4 Arduino (c) Ian jun23
#include <avr/power.h>

//##Hardware - not needed  we are just on serial


//##Game Configuration
const int BOARD_ROWS_MIN = 4;
const int BOARD_COLS_MIN = 4;
const int BOARD_ROWS_MAX = 15; //we dont have much RAM here
const int BOARD_COLS_MAX = 15;







//##game globals
//board
byte board[BOARD_ROWS_MAX][BOARD_COLS_MAX];
byte act[BOARD_ROWS_MAX][BOARD_COLS_MAX];
/*
   board MxN  with digits
   act holds code for math
  0 +
  1 - 
  2 *
  3 /

  +10 player 1 has passed there
  +20 player 2 has passed there

*/

//board size
byte rows = BOARD_ROWS_MIN;
byte cols = BOARD_COLS_MIN;

//players human or computer
bool p1computer = false;
bool p2computer = false;



// players positions
byte p1row = 0;
byte p1col = 0;
byte p2row = BOARD_ROWS_MIN;
byte p2col = BOARD_COLS_MIN;



// Player Scores this game
int playerMoves = 0;
long int player1score = 0;
long int player2score = 0;

// games stats
int plays = 0;
int p1wins = 0;
int p2wins = 0;



//user input 
int promptUser(String prompt, int minValue, int maxValue) {
  int value = 0;
  bool validInput = false;

  while (!validInput) {
    Serial.print(prompt);

    while (Serial.available() == 0) {
      // Wait for input
    }

    value = Serial.parseInt();
    Serial.println(value);

    if (value >= minValue && value <= maxValue) {
      validInput = true;
    } else {
      Serial.print("Invalid input. Please enter a value between ");
      Serial.print(minValue);
      Serial.print(" and ");
      Serial.println(maxValue);
    }

    // Clear the serial buffer
    while (Serial.available() > 0) {
      Serial.read();
    }
  }

  return value;
}


// Display the game board
void displayBoard() {
 


  // Display column numbers on top row
  for (int col = 0; col < cols; col++) {
    Serial.print("\t");
    Serial.print(col + 1);
  }
  Serial.println();

  for (int row = 0; row < rows; row++) {
    // Display row number
    Serial.print(row + 1);
    Serial.print("\t");

    for (int col = 0; col < cols; col++) {
      // Display math action symbol followed by digit
      int digit = board[row][col];
      int action = act[row][col];
      if (action > 19) // player already moved from here so show 2p
      {
        action = action - 20;
        Serial.print("P2");
      }
      if (action > 9) 
      {
        action = action - 10;
        Serial.print("P1");
      }

      switch (action) {
        case 0:  // Sum
          Serial.print("+");
          break;
        case 1:  // Subtraction
          Serial.print("-");
          break;
        case 2:  // Multiplication
          Serial.print("*");
          break;
        case 3:  // Division
          Serial.print("/");
          break;
        default:            
          Serial.print("?");
          Serial.print(action);
          break;
      }

      Serial.print(digit);
      //current position cursor
      if ((col==p1col && row==p1row) || (col==p2col && row==p2row))
        Serial.print(">");
      Serial.print("\t");
    }

    Serial.println();
  }
}

void displayStats()
{
  Serial.println("--------------");
  Serial.print("Game number: ");
  Serial.println(plays);
  Serial.print("Player 1 has wins:");
  Serial.println(p1wins);
  Serial.print("Player 2 has wins:");
  Serial.println(p2wins);

}
void displayScore()
{
  if (player1score< player2score)
  {
    Serial.print("Player2: ");
    Serial.print(player2score);
    Serial.print(" Player1: ");
    Serial.print(player1score);
  }
  else
  {
    Serial.print("Player1: ");
    Serial.print(player1score);
    Serial.print(" Player2: ");
    Serial.print(player2score);
  }
  Serial.println(" points");
}
void displayGameOver()
{
  Serial.println("Game OVER !!!");
  if (player1score == player2score)
    Serial.println("Nobody is winner!");
  else
  if (player1score > player2score)
  {
    p1wins ++;
    Serial.println("Player1 WINS!!");
  }
  else
  {
    p2wins ++;
    Serial.println("Player2 WINS!!");
  }

  Serial.println("^^^^^^^^^^^");
  Serial.println("***********");


  player1score = 0;
  player2score = 0;
  playerMoves = 0;

}

  




// Function to initialize the game 
void initializeGame() {
  //welcome message to the user
  Serial.println("MathTricks 4 Arduino (c) Ian jun23");
  Serial.println("---");
  Serial.println("This is board game for player vs computer or two players.");
  Serial.println("At first you will need to chose the size of the board [NxM] and players:");
  Serial.println("Pressing RESET will restart.");

  rows = promptUser("Enter the number of rows (between " + String(BOARD_ROWS_MIN) + " and " + String(BOARD_ROWS_MAX) + "): ", BOARD_ROWS_MIN, BOARD_ROWS_MAX);
  cols = promptUser("Enter the number of columns (between " + String(BOARD_COLS_MIN) + " and " + String(BOARD_COLS_MAX) + "): ", BOARD_COLS_MIN, BOARD_COLS_MAX);

  int pmode = promptUser("Choose: 0,1,2,3  where player 1 and player 2 will be: 0- human vs human, 1- human vs computer, 2- computer vs human, 3- computer vs computer: ", 0, 3);
  switch (pmode)
  {
    case 1: p2computer = true; break;
    case 2: p1computer = true; break;
    case 3: p1computer = true; p2computer=true;
  }

}

// Initialize the game board with the specified dimensions rows cols
void initializeBoard() {
   int maxRand = max(rows, cols);  //bigger board - bigger bonuces

   for (int i = 0 ; i < rows; i++)
     for(int j = 0; j < cols; j++ )
     {
       board[i][j] = random(maxRand);
       if (board[i][j] != 0)
         act[i][j] = random(4);
       else
         act[i][j] = random(3); //div by 0 will be avoided
     }


  //settle players
  board[0][0] = 0;
  act[0][0] = 10;
  p1col = 0;
  p1row = 0;
  
  board[rows-1][cols-1] = 0;
  act[rows-1][cols-1] = 20;
  p2col=cols -1;
  p2row= rows -1;
  
}
 
// Function to perform the player's move
void humanMakeMove(int player) {
  int oldcol, oldrow;
  int newcol, newrow;
  char key; 
  if (player == 1)
  {
    oldcol = p1col;
    oldrow = p1row;
  }
  else
  {
    oldcol = p2col;
    oldrow = p2row;
  }

  do 
  {
    do
    {
      Serial.print("Player ");
      Serial.print(player);
      Serial.print(" make your move (directions: qwe a d zxc): ");
    
      while (Serial.available() == 0) {
       // Wait for input
      }

      key = Serial.read();
      delay(2); //!!! WHY 
      Serial.println(key);

      if (!(key == 'q' || key == 'w' || key == 'e' || key == 'a' ||  key == 'd' || key == 'z' || key == 'x' || key == 'c')) {
        Serial.println("Invalid input. Please enter again ");
      }

      // Clear the serial buffer
      while (Serial.available() > 0) {
         Serial.read();
      }
      
     
    } while (!(key == 'q' || key == 'w' || key == 'e' || key == 'a' ||  key == 'd' || key == 'z' || key == 'x' || key == 'c'));


     switch(key)  
     {
       case 'q': newrow = oldrow -1; newcol = oldcol - 1; break;//up left
       case 'w': newrow = oldrow -1; newcol = oldcol; break;//up 
       case 'e': newrow = oldrow -1; newcol = oldcol + 1; break;//up right
       case 'a': newrow = oldrow; newcol = oldcol - 1; break;// left
       case 'd': newrow = oldrow; newcol = oldcol + 1; break;// right
       case 'z': newrow = oldrow +1; newcol = oldcol - 1; break;//down left
       case 'x': newrow = oldrow +1; newcol = oldcol; break;//down
       case 'c': newrow = oldrow +1; newcol = oldcol + 1; break;//down right

     
     }
     if (!isValidMove(newrow, newcol))
       Serial.println("This is not possible move. Try again!");
     key ='s';
    

  } while (!isValidMove(newrow, newcol));
  // mark this move on board and move on
  if (player == 1)
  {
    
    p1col = newcol;
    p1row = newrow;

    
    int digit = board[newrow][newcol];
    switch (act[newrow][newcol]) {
      case 0:  // Addition
        player1score += digit;
      break;
      case 1:  // Subtraction
        player1score -= digit;
      break;
      case 2:  // Multiplication
        player1score *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          player1score /= digit;
        }
      break;
    }
    act[newrow][newcol] += 10;
  }
  else
  {
    p2col = newcol;
    p2row = newrow;
    int digit = board[newrow][newcol];
    switch (act[newrow][newcol]) {
      case 0:  // Addition
        player2score += digit;
      break;
      case 1:  // Subtraction
        player2score -= digit;
      break;
      case 2:  // Multiplication
        player2score *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          player2score /= digit;
        }
      break;
    }
    act[newrow][newcol] += 20;
  }
  




}

void computerMakeMove(int player)
{
  int oldcol, oldrow;
  int newcol, newrow;
  long int myscore;
  long int higherscore = WINT_MIN;

  if (player == 1)
  {
    oldcol = p1col;
    oldrow = p1row;
    myscore = player1score;
  }
  else
  {
    oldcol = p2col;
    oldrow = p2row;
    myscore = player2score;
  }

  int checkrow = oldrow;
  int checkcol = oldcol;
  int thisscore;
  bool makeOneMove = false;
  // 8 directions to check
  //up
  checkrow = oldrow - 1 ;
  checkcol = oldcol;
  thisscore = myscore;
  if(isValidMove(checkrow, checkcol))  
  {
    int digit = board[checkrow][checkcol];
    switch (act[checkrow][checkcol]) {
      case 0:  // Addition
        thisscore += digit;
      break;
      case 1:  // Subtraction
        thisscore -= digit;
      break;
      case 2:  // Multiplication
        thisscore *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          thisscore /= digit;
        }
      break;
    }
  
    if (thisscore >= higherscore || makeOneMove == false)
    {
      newcol = checkcol;
      newrow = checkrow;
      higherscore = thisscore;
      makeOneMove = true;
    }
  }
  //
//up left
  checkrow = oldrow - 1 ;
  checkcol = oldcol -1;
  thisscore = myscore;
  if(isValidMove(checkrow, checkcol))  
  {
    int digit = board[checkrow][checkcol];
    switch (act[checkrow][checkcol]) {
      case 0:  // Addition
        thisscore += digit;
      break;
      case 1:  // Subtraction
        thisscore -= digit;
      break;
      case 2:  // Multiplication
        thisscore *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          thisscore /= digit;
        }
      break;
    }
    if (thisscore >= higherscore || makeOneMove == false)
    {
      newcol = checkcol;
      newrow = checkrow;
      higherscore = thisscore;
      makeOneMove = true;
    }
  }
  //
  //up right
  checkrow = oldrow - 1 ;
  checkcol = oldcol + 1;
  thisscore = myscore;
  if(isValidMove(checkrow, checkcol))  
  {
    int digit = board[checkrow][checkcol];
    switch (act[checkrow][checkcol]) {
      case 0:  // Addition
        thisscore += digit;
      break;
      case 1:  // Subtraction
        thisscore -= digit;
      break;
      case 2:  // Multiplication
        thisscore *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          thisscore /= digit;
        }
      break;
    }
    if (thisscore >= higherscore || makeOneMove == false)
    {
      newcol = checkcol;
      newrow = checkrow;
      higherscore = thisscore;
      makeOneMove = true;
    }
  }
  //
  //left 
  checkrow = oldrow;
  checkcol = oldcol -1;
  thisscore = myscore;
  if(isValidMove(checkrow, checkcol))  
  {
    int digit = board[checkrow][checkcol];
    switch (act[checkrow][checkcol]) {
      case 0:  // Addition
        thisscore += digit;
      break;
      case 1:  // Subtraction
        thisscore -= digit;
      break;
      case 2:  // Multiplication
        thisscore *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          thisscore /= digit;
        }
      break;
    }
    if (thisscore >= higherscore || makeOneMove == false)
    {
      newcol = checkcol;
      newrow = checkrow;
      higherscore = thisscore;
      makeOneMove = true;
    }
  }
  //
  //right
  checkrow = oldrow;
  checkcol = oldcol+1;
  thisscore = myscore;
  if(isValidMove(checkrow, checkcol))  
  {
    int digit = board[checkrow][checkcol];
    switch (act[checkrow][checkcol]) {
      case 0:  // Addition
        thisscore += digit;
      break;
      case 1:  // Subtraction
        thisscore -= digit;
      break;
      case 2:  // Multiplication
        thisscore *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          thisscore /= digit;
        }
      break;
    }
    if (thisscore >= higherscore || makeOneMove == false)
    {
      newcol = checkcol;
      newrow = checkrow;
      higherscore = thisscore;
      makeOneMove = true;
    }
  }
  //
  //down 
  checkrow = oldrow + 1 ;
  checkcol = oldcol;
  thisscore = myscore;
  if(isValidMove(checkrow, checkcol))  
  {
    int digit = board[checkrow][checkcol];
    switch (act[checkrow][checkcol]) {
      case 0:  // Addition
        thisscore += digit;
      break;
      case 1:  // Subtraction
        thisscore -= digit;
      break;
      case 2:  // Multiplication
        thisscore *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          thisscore /= digit;
        }
      break;
    }
    if (thisscore >= higherscore || makeOneMove == false)
    {
      newcol = checkcol;
      newrow = checkrow;
      higherscore = thisscore;
      makeOneMove = true;
    }
  }
  //
  //down left 
  checkrow = oldrow + 1 ;
  checkcol = oldcol -1;
  thisscore = myscore;
  if(isValidMove(checkrow, checkcol))  
  {
    int digit = board[checkrow][checkcol];
    switch (act[checkrow][checkcol]) {
      case 0:  // Addition
        thisscore += digit;
      break;
      case 1:  // Subtraction
        thisscore -= digit;
      break;
      case 2:  // Multiplication
        thisscore *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          thisscore /= digit;
        }
      break;
    }
    if (thisscore >= higherscore || makeOneMove == false)
    {
      newcol = checkcol;
      newrow = checkrow;
      higherscore = thisscore;
      makeOneMove = true;
    }
  }
  //
  //down right
  checkrow = oldrow + 1 ;
  checkcol = oldcol + 1;
  thisscore = myscore;
  if(isValidMove(checkrow, checkcol))  
  {
    int digit = board[checkrow][checkcol];
    switch (act[checkrow][checkcol]) {
      case 0:  // Addition
        thisscore += digit;
      break;
      case 1:  // Subtraction
        thisscore -= digit;
      break;
      case 2:  // Multiplication
        thisscore *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          thisscore /= digit;
        }
      break;
    }
    if (thisscore >= higherscore || makeOneMove == false)
    {
      newcol = checkcol;
      newrow = checkrow;
      higherscore = thisscore;
      makeOneMove = true;
    }
  }
  //


  // COMPUTER mark this move on board and move on
  if (player == 1)
  {
    
    p1col = newcol;
    p1row = newrow;

    
    int digit = board[newrow][newcol];
    switch (act[newrow][newcol]) {
      case 0:  // Addition
        player1score += digit;
      break;
      case 1:  // Subtraction
        player1score -= digit;
      break;
      case 2:  // Multiplication
        player1score *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          player1score /= digit;
        }
      break;
    }
    act[newrow][newcol] += 10;
  }
  else
  {
    p2col = newcol;
    p2row = newrow;
    int digit = board[newrow][newcol];
    switch (act[newrow][newcol]) {
      case 0:  // Addition
        player2score += digit;
      break;
      case 1:  // Subtraction
        player2score -= digit;
      break;
      case 2:  // Multiplication
        player2score *= digit;
      break;
      case 3:  // Division
        if (digit != 0) {
          player2score /= digit;
        }
      break;
    }
    act[newrow][newcol] += 20;
  }

 // end of thinking.... :)
}
// check if given is valid move
bool isValidMove(int row, int col)
{
  if (row<0) return false;
  if (row>rows-1) return false;
  if (col<0) return false;
  if (col>cols-1) return false;

  if (act[row][col] > 9) return false;

  return true;
}

// Function to check if the player has any valid moves
bool hasValidMoves(int row, int col) {
  // 8 directions to check
  
  //up
  if (isValidMove(row-1, col)) return true;
  // up left
  if(isValidMove(row-1, col-1)) return true;
  // up right
  if (isValidMove(row-1, col+1)) return true;
  // left
  if (isValidMove(row, col-1)) return true;
  // right
  if (isValidMove(row, col+1)) return true;
  //down
  if(isValidMove(row+1, col)) return true;
  // down left
  if(isValidMove(row+1, col-1)) return true;
  // down right
  if(isValidMove(row+1, col+1)) return true;
   
  return false;
}

void setup() {  
  clock_prescale_set(clock_div_2);
  Serial.begin(9600);
  randomSeed(analogRead(A0));

  // Initialize the game and board
  initializeGame();
}

void loop() { 
  // Display the initial board
  initializeBoard();
  displayBoard();
  plays++;
  displayStats();

  bool gameover = false;
  while(!gameover)
  {
    if (hasValidMoves(p1row, p1col))
    {
      if (!p1computer)
        humanMakeMove(1);
      else
        computerMakeMove(1); 
    }
    else
      gameover = true;  
    displayBoard();
    displayScore();   
    if (hasValidMoves(p2row, p2col) && !gameover)
    {
      if (!p2computer)
        humanMakeMove(2);
      else
        computerMakeMove(2);  
  
    }
    else
      gameover = true;

    displayBoard();
    displayScore();  
  }

  displayGameOver();  
}

  

