#define DATA_PIN 13
#define LATCH_PIN 12
#define CLOCK_PIN 11

class Button  
{
private:
	byte _pin;
  	bool _flagPressed = false;
  	bool _flagReleased = false;
  	bool _flagClicked = false;
  	unsigned long _upTime = 0;
  	unsigned long _delay;
  
  	void updateState()
    {
      	bool buttonState = !digitalRead(_pin);
      
      	if (millis() >= _upTime)
        {
          	if (buttonState && !_flagPressed)
            {
              	_upTime = millis() + _delay;
              	_flagPressed = true;
              	_flagReleased = false;
              	_flagClicked = true;
            }
          	else if (!buttonState && _flagPressed)
          	{
              	_upTime = millis() + _delay;
              	_flagPressed = false;
              	_flagReleased = true;
              	_flagClicked = false;
            }
        }
    }
  
public:  	
  	Button(byte pin, unsigned long delay = 20)
    {
      	_pin = pin;
      	_delay = delay;
    }
  
  	byte getPin()
    {
      	return _pin;
    }
  
  	bool isPressed()
    {
        updateState();       
      	return _flagPressed;
    }
  
  	bool wasClicked()
    {
      	bool result;
      
        updateState();
      	result = _flagClicked;
      	_flagClicked = false;
      
      	return result;
    }
  
  	bool wasReleased()
    {
      	bool result;
      
        updateState();
      	result = _flagReleased;
      	_flagReleased = false;
      
      	return result;
    }
};

enum Direction
{
    UP,
  	LEFT,
  	DOWN,
  	RIGHT
} direction;

class Position
{
private:
  	byte _row;
  	byte _column;
  
public:
    Position(byte row, byte column)
    {
		_row = row;
      	_column = column;
    }
  
  	byte getRow()
    {
      	return _row;
    }
  
  	byte getColumn()
    {
      	return _column;
    }
  
  	void setRow(byte row)
    {
      	_row = row;
    }
  
  	void setColumn(byte column)
    {
      	_column = column;
    }
};

Button buttons[] = {(7), (6), (5), (4)};
byte buttonsAmount = 4;
byte gameFieldSize = 4;
Position snake[] = {Position(0, 0), Position(0, 1), Position(0, 2)};
byte snakeSize = 3;
unsigned long snakeSpeed = 1000;
unsigned long snakeMoveTime = 0;
byte gameField[4][4] =
{
    {0, 0, 0, 0},
 	{0, 0, 0, 0},
  	{0, 0, 0, 0},
  	{0, 0, 0, 0}
};

void setup()
{
    pinMode(DATA_PIN, OUTPUT);
  	pinMode(LATCH_PIN, OUTPUT);
  	pinMode(CLOCK_PIN, OUTPUT);
  
  	direction = RIGHT;
  
  	for (int i = 0; i < buttonsAmount; i++)
  	{
    	pinMode(buttons[i].getPin(), INPUT_PULLUP);
  	}
  
  	for (int j = 0; j < snakeSize; j++)
  	{
    	gameField[0][j] = 1;
  	}
}

void move();
int convertGameFieldToBytes();
void updateDirection();
void updateGameFieldLeds();

void loop()
{
  	updateGameFieldLeds();
  	updateDirection();
  	move();
  	delay(10);
}

void move()
{
    if (millis() < snakeMoveTime)
    {
    	return;
    }
  
  	snakeMoveTime = millis() + snakeSpeed;
    gameField[snake[0].getRow()][snake[0].getColumn()] = 0;
  
  	for (int i = 0; i < snakeSize; i++)
    {
        if (i + 1 < snakeSize)
        {
          	Position temp = snake[i];
          	snake[i] = snake[i + 1];
          	snake[i + 1] = temp;
        }
    }
  
  	byte headIndex = snakeSize - 1;
    byte prevheadElemIndex = (headIndex - 1) < 0 ? 0 : (headIndex - 1);      
  	byte newRow;
  	byte newColumn;
  
    switch(direction)
    {
    	case UP:
        {         	
          	newRow = (snake[prevheadElemIndex].getRow() - 1) < 0 ? (gameFieldSize - 1) : (snake[prevheadElemIndex].getRow() - 1);
            newColumn = snake[prevheadElemIndex].getColumn();                      
        	break; 
        }
        case LEFT:
        {
            newRow = snake[prevheadElemIndex].getRow();
            newColumn = (snake[prevheadElemIndex].getColumn() - 1) < 0 ? (gameFieldSize - 1) : (snake[prevheadElemIndex].getColumn() - 1);                      
        	break; 
        }
        case DOWN:
        {
           	newRow = (snake[prevheadElemIndex].getRow() + 1) >= gameFieldSize ? 0 : (snake[prevheadElemIndex].getRow() + 1);
            newColumn = snake[prevheadElemIndex].getColumn();                      
        	break;  
        }
        default:
        {
            newRow = snake[prevheadElemIndex].getRow();
            newColumn = (snake[prevheadElemIndex].getColumn() + 1) >= gameFieldSize ? 0 : (snake[prevheadElemIndex].getColumn() + 1);                     
        	break;  
        }
    }
  
  	snake[headIndex].setRow(newRow);
    snake[headIndex].setColumn(newColumn);
  
  	gameField[snake[headIndex].getRow()][snake[headIndex].getColumn()] = 1;
}

int convertGameFieldToBytes()
{
    int result = 0;
  
  	for (int i = 0; i < gameFieldSize; i++)
  	{
  		for (int j = 0; j < gameFieldSize; j++)
  		{
      		bitWrite(result, i + j * gameFieldSize, gameField[j][i] > 0 ? 1 : 0);
    	}
  	}
  
  	return result;
}

void updateGameFieldLeds()
{
    int fieldData = convertGameFieldToBytes();
  
  	digitalWrite(LATCH_PIN, LOW);
  	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (byte)(fieldData >> 8));
  	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (byte)fieldData);
  	digitalWrite(LATCH_PIN, HIGH);
}

void updateDirection()
{
    if (buttons[UP].wasClicked() && direction != DOWN)
  	{
        direction = UP;
  	}
  	else if (buttons[LEFT].wasClicked() && direction != RIGHT)
  	{
    	direction = LEFT;
  	}
  	else if (buttons[DOWN].wasClicked() && direction != UP)
  	{
    	direction = DOWN;
  	}
  	else if (buttons[RIGHT].wasClicked() && direction != LEFT)
  	{
        direction = RIGHT;
  	}
}