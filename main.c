/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define CE_PORT GPIOB // PB6 chip enable (aka slave select)
#define CE_PIN GPIO_PIN_6
#define DC_PORT GPIOA // PA0 data/control
#define DC_PIN GPIO_PIN_8
#define RESET_PORT GPIOA // PA1 reset
#define RESET_PIN GPIO_PIN_9
#define GLCD_WIDTH 84
#define GLCD_HEIGHT 48
#define NUM_BANKS 6

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim16;

/* USER CODE BEGIN PV */

// table containing all of the different numbers for the puzzle
const char puzzle_piece_table[][21] = { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF }, // 'block'    0

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x0C, 0xFE, 0xFE,
				0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'top 1'    1

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x70, 0x7F, 0x7F,
				0x7F, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'bottom 1' 2

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x1C, 0x06, 0x06,
				0x0E, 0xFC, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'top 2'    3

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x78, 0x7C, 0x6E,
				0x67, 0x63, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'bottom 2' 4

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x1C, 0x0E, 0x8E,
				0x8E, 0xFC, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'top 3'    5

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x70, 0x71,
				0x71, 0x3F, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'bottom 3' 6

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x80, 0x80,
				0x80, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'top 4'    7

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
				0x01, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'bottom 4' 8

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x86, 0x86,
				0x86, 0x86, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'top 5'    9

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x71, 0x61, 0x61,
				0x61, 0x7F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'bottom 5' 10

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0x06, 0x86,
				0x86, 0x8E, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'top 6'    11

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x7F, 0x61, 0x61,
				0x61, 0x7F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'bottom 6' 12

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x86, 0xE6,
				0xF6, 0x7E, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'top 7'    13

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x7E, 0x1F, 0x07,
				0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'bottom 7' 14

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xFE, 0x86, 0x86,
				0x86, 0xFE, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'top 8'    15

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x7F, 0x61, 0x61,
				0x61, 0x7F, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'bottom 8' 16

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x86, 0x86,
				0x86, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'top 9'    17

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
				0x01, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'bottom 9' 18

		{ 0x00, 0x00, 0x00, 0x08, 0x0C, 0xFE, 0xFE, 0xFE, 0x00, 0x00, 0x00,
				0xFC, 0xFE, 0x06, 0x06, 0x06, 0xFE, 0xFC, 0x00, 0x00, 0x00 }, // 'top 10'    19

		{ 0x00, 0x00, 0x00, 0x70, 0x70, 0x7F, 0x7F, 0x7F, 0x70, 0x70, 0x00,
				0x3F, 0x7F, 0x60, 0x60, 0x60, 0x7F, 0x3F, 0x00, 0x00, 0x00 }, // 'bottom 10' 20

		{ 0x00, 0x00, 0x00, 0x08, 0x0C, 0xFE, 0xFE, 0xFE, 0x00, 0x00, 0x00,
				0x08, 0x0C, 0xFE, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'top 11'    21

		{ 0x00, 0x00, 0x00, 0x70, 0x70, 0x7F, 0x7F, 0x7F, 0x70, 0x70, 0x00,
				0x70, 0x70, 0x7F, 0x7F, 0x7F, 0x70, 0x70, 0x00, 0x00, 0x00 }, // 'bottom 11' 22

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0x06, 0x06,
				0x06, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'top 0'     23

		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x7F, 0x60, 0x60,
				0x60, 0x7F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 'bottom 0'  24

		};

// table containing the font
const char font_table[][6] = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // space 0
		{ 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00 }, // '!' 1
		{ 0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00 }, // 'A' 2
		{ 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00 }, // 'B' 3
		{ 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00 }, // 'C' 4
		{ 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00 }, // 'D' 5
		{ 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00 }, // 'E' 6
		{ 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00 }, // 'F' 7
		{ 0x7F, 0x41, 0x59, 0x49, 0x7B, 0x00 }, // 'G' 8
		{ 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00 }, // 'H' 9
		{ 0x41, 0x41, 0x7F, 0x41, 0x41, 0x00 }, // 'I' 10
		{ 0x21, 0x41, 0x3F, 0x01, 0x01, 0x00 }, // 'J' 11
		{ 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00 }, // 'K' 12
		{ 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00 }, // 'L' 13
		{ 0x7F, 0x02, 0x04, 0x02, 0x7F, 0x00 }, // 'M' 14
		{ 0x7F, 0x06, 0x18, 0x60, 0x7F, 0x00 }, // 'N' 15
		{ 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00 }, // 'O' 16
		{ 0x7F, 0x09, 0x09, 0x09, 0x0F, 0x00 }, // 'P' 17
		{ 0x3E, 0x41, 0x41, 0x61, 0x7E, 0x00 }, // 'Q' 18
		{ 0x7F, 0x09, 0x19, 0x29, 0x47, 0x00 }, // 'R' 19
		{ 0x62, 0x4D, 0x49, 0x59, 0x23, 0x00 }, // 'S' 20
		{ 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00 }, // 'T' 21
		{ 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00 }, // 'U' 22
		{ 0x0F, 0x30, 0x40, 0x30, 0x0F, 0x00 }, // 'V' 23
		{ 0x3F, 0x40, 0x78, 0x40, 0x3F, 0x00 }, // 'W' 24
		{ 0x41, 0x36, 0x08, 0x36, 0x41, 0x00 }, // 'X' 25
		{ 0x01, 0x06, 0x78, 0x06, 0x01, 0x00 }, // 'Y' 26
		{ 0x61, 0x51, 0x49, 0x45, 0x43, 0x00 }, // 'Z' 27
		{ 0x00, 0x00, 0x7E, 0x81, 0xB5, 0xA1 }, // front of smiley face 28
		{ 0xA1, 0xB5, 0x81, 0x7E, 0x00, 0x00 }  // back  of smiley face 29

};

// used for saving where the last piece was placed
unsigned int xPosition = 0;
unsigned int yPosition = 0;

// boolean variables used to tell the system whether a piece can be moved in a certain direction
int right = 0;
int left = 0;
int up = 0;
int down = 0;

// boolean variable that tells the display functions whether the input is a piece or a number
int piece = 1;

// the array that is mixed up and the solved array that it is compared to
int numbers[] = { 10, 9, 4, 7, 11, 6, 3, 8, 5, 1, 2, 0 };
int solved[] =  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0 };
//tester[] =  { 1, 2, 3, 4, 5, 6, 8, 11, 9, 10, 7, 0 };
// numbers[] = { 10, 9, 4, 7, 11, 6, 3, 8, 5, 1, 2, 0 };

// keeps track of the location of the blank space and which number slid most recently
int blankIndex = 11;
int slidingNumber;

// keeps track of the time
int timer = 0;

// tells the while loop when the puzzle was completed and stops interrupts from working
int done = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */
void SPI_write(unsigned char data);
void GLCD_data_write(unsigned char data);
void GLCD_command_write(unsigned char data);
void GLCD_init(int normal);
void GLCD_setCursor(unsigned char x, unsigned char y);
void GLCD_clear(void);
void GLCD_putPiece(int puzzle_piece_table_row, int piece);
void GLCD_putchar(int font_table_row);
void DisplayNumber(int number, unsigned int x, unsigned int y, int piece);
void DisplayBlank(unsigned int x, unsigned int y, int piece);
// from https://www.includehelp.com/code-snippets/c-program-to-compare-two-arrays.aspx
char compareArray(int a[],int b[],int size);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// all of the interrupt instructions
// depending on what button was pushed the blankIndex variable will be changed by 1 or 4
// the slidingNumber variable is then assigned to that new blankIndex
// the values in the array are then switched (0 with whatever number was there prior)
// the blank space and number is then displayed on the screen and the x and y position variables are updated
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	// Slide Left
	if(!done){
		if (GPIO_Pin == GPIO_PIN_0 && left) {
			blankIndex += 1;
			slidingNumber = numbers[blankIndex];
			numbers[blankIndex] = 0;
			numbers[blankIndex - 1] = slidingNumber;
			DisplayNumber(slidingNumber, xPosition, yPosition, piece);
			xPosition += (unsigned int) 21;
			DisplayNumber(0, xPosition, yPosition, piece);

		}

		// Slide Down
		else if (GPIO_Pin == GPIO_PIN_1 && down) // change 'X' to pin number
				{
			blankIndex -= 4;
			slidingNumber = numbers[blankIndex];
			numbers[blankIndex] = 0;
			numbers[blankIndex + 4] = slidingNumber;
			DisplayNumber(slidingNumber, xPosition, yPosition, piece);
			yPosition -= (unsigned int) 2;
			DisplayNumber(0, xPosition, yPosition, piece);

		}

		// Slide Up
		else if (GPIO_Pin == GPIO_PIN_4 && up) // change 'X' to pin number
				{
			blankIndex += 4;
			slidingNumber = numbers[blankIndex];
			numbers[blankIndex] = 0;
			numbers[blankIndex - 4] = slidingNumber;
			DisplayNumber(slidingNumber, xPosition, yPosition, piece);
			yPosition += (unsigned int) 2;
			DisplayNumber(0, xPosition, yPosition, piece);

		}

		// Slide Right
		else if (GPIO_Pin == GPIO_PIN_10 && right) {
			blankIndex -= 1;
			slidingNumber = numbers[blankIndex];
			numbers[blankIndex] = 0;
			numbers[blankIndex + 1] = slidingNumber;
			DisplayNumber(slidingNumber, xPosition, yPosition, piece);
			xPosition -= (unsigned int) 21;
			DisplayNumber(0, xPosition, yPosition, piece);

		}

		for (int i=1 ; i<700 ; i++); // help with button bouncing
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim16); // Start timer
  	int normal = 0; // declares whether screen is shown inverted or normal
	GLCD_init(normal); // initialize the screen as inverted
	GLCD_clear(); // clear the screen

	// Display each puzzle piece
	for (int i = 0; i < 12; i++) {
		DisplayNumber(numbers[i], xPosition, yPosition, piece);
		xPosition += (unsigned int) 21;

		// Resets the x when x = 63 and adds 2 to y
		if (xPosition > (unsigned int) 63) {
			xPosition = 0;
			yPosition += 2;
		}
	}

	// sets cursor at the blankIndex position
	xPosition = 63;
	yPosition = 4;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (!done) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		// switch case that reads the blankIndex and changes the boolean values for whether a piece can be moved in a certain direction
		switch (blankIndex) {
		case 0:
			left = 1;
			right = 0;
			up = 1;
			down = 0;
			break;
		case 1:
			left = 1;
			right = 1;
			up = 1;
			down = 0;
			break;
		case 2:
			left = 1;
			right = 1;
			up = 1;
			down = 0;
			break;
		case 3:
			left = 0;
			right = 1;
			up = 1;
			down = 0;
			break;
		case 4:
			left = 1;
			right = 0;
			up = 1;
			down = 1;
			break;
		case 5:
			left = 1;
			right = 1;
			up = 1;
			down = 1;
			break;
		case 6:
			left = 1;
			right = 1;
			up = 1;
			down = 1;
			break;
		case 7:
			left = 0;
			right = 1;
			up = 1;
			down = 1;
			break;
		case 8:
			left = 1;
			right = 0;
			up = 0;
			down = 1;
			break;
		case 9:
			left = 1;
			right = 1;
			up = 0;
			down = 1;
			break;
		case 10:
			left = 1;
			right = 1;
			up = 0;
			down = 1;
			break;
		case 11:
			left = 0;
			right = 1;
			up = 0;
			down = 1;
			break;
		}

		// checks to see if number array is equal to the solved array
		if(compareArray(numbers, solved,12)==0){
			normal = 1;
			GLCD_init(normal); // initialize the screen as normal
			GLCD_clear();

			// ends while loops and stops interrupts from being executed
			done = 1;

			// an array used to hold the individual digits of the end timer. Initialized at 0.
			int timerArray[3];
			timerArray[0] = 0;
			timerArray[1] = 0;
			timerArray[2] = 0;

			int i = 2;
			// from https://www.log2base2.com/c-examples/loop/split-a-number-into-digits-in-c.html
			while(timer > 0) //do till timer greater than  0
			    {
			        int digit = timer % 10;  //split last digit from number
			        timerArray[i] = digit;

			        timer /= 10;    //divide timer by 10.
			        i--;
			    }

			// prepares the digits and font to display in the middle of the screen
			xPosition = 11;

			// changes piece variable to false so just the number will be displayed
			piece = 0;

			// displays the end time on screen
			for(i = 0; i <= 2; i++){
				if(timerArray[i] == 0){ // the digit 0 was added later to table so we need to change value to 12
					timerArray[i] = 12;
				}
				DisplayNumber(timerArray[i], xPosition, 2, piece);
				xPosition += 8;
			}

			// Displays "SECONDS"
			GLCD_setCursor(xPosition+1, 3);
			GLCD_putchar(20);
			GLCD_putchar(6);
			GLCD_putchar(4);
			GLCD_putchar(16);
			GLCD_putchar(15);
			GLCD_putchar(5);
			GLCD_putchar(20);

			// Displays "GREAT JOB!! :)"
			GLCD_setCursor(1, 4);
			GLCD_putchar(8);
			GLCD_putchar(19);
			GLCD_putchar(6);
			GLCD_putchar(2);
			GLCD_putchar(21);
			GLCD_putchar(0);
			GLCD_putchar(11);
			GLCD_putchar(16);
			GLCD_putchar(3);
			GLCD_putchar(1);
			GLCD_putchar(1);
			GLCD_putchar(0);
			GLCD_putchar(28);
			GLCD_putchar(29);

		}

	}
	while(1);
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 7999;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 9999;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA4 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void SPI_write(unsigned char data) {
// Chip Enable (low is asserted)
	HAL_GPIO_WritePin(CE_PORT, CE_PIN, GPIO_PIN_RESET);
// Send data over SPI1
	HAL_SPI_Transmit(&hspi1, (uint8_t*) &data, 1, HAL_MAX_DELAY);
// Chip Disable
	HAL_GPIO_WritePin(CE_PORT, CE_PIN, GPIO_PIN_SET);
}

void GLCD_data_write(unsigned char data) {
// Switch to "data" mode (D/C pin high)
	HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
// Send data over SPI
	SPI_write(data);
}

void GLCD_command_write(unsigned char data) {
// Switch to "command" mode (D/C pin low)
	HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_RESET);
// Send data over SPI
	SPI_write(data);
}

void GLCD_init(int normal) {
// Keep CE high when not transmitting
	HAL_GPIO_WritePin(CE_PORT, CE_PIN, GPIO_PIN_SET);
// Reset the screen (low pulse - down & up)
	HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, GPIO_PIN_SET);
// Configure the screen (according to the datasheet)
	GLCD_command_write(0x21); // enter extended command mode
	GLCD_command_write(0xB0); // set LCD Vop for contrast (this may be adjusted)
	GLCD_command_write(0x04); // set temp coefficient
	GLCD_command_write(0x15); // set LCD bias mode (this may be adjusted)
	GLCD_command_write(0x20); // return to normal command mode
	if(normal){
		GLCD_command_write(0x0C); // set display normal mode
	} else {
		GLCD_command_write(0x0D); // set display inverted mode
	}
}

void GLCD_setCursor(unsigned char x, unsigned char y) {
	GLCD_command_write(0x80 | x); // column
	GLCD_command_write(0x40 | y); // bank
}

void GLCD_clear(void) {
	int i;
	for (i = 0; i < (GLCD_WIDTH * NUM_BANKS); i++) {
		GLCD_data_write(0x00); // write zeros
	}
	GLCD_setCursor(0, 0); // return cursor to top left
}

void GLCD_putPiece(int puzzle_piece_table_row, int piece) {
	int i;
	if(piece){ // if piece is true then it displays the full 21 hex values in the table row
		for (i = 0; i < 21; i++) {
			GLCD_data_write(puzzle_piece_table[puzzle_piece_table_row][i]);
		}
	} else { // if just a digit then it displays the 7 hex values containing the outline of the digit
		//8-14
		for (i = 7; i <= 13; i++) {
			GLCD_data_write(puzzle_piece_table[puzzle_piece_table_row][i]);
		}
	}
}

void GLCD_putchar(int font_table_row){
	int i;
	for (i=0; i<6; i++){
	 GLCD_data_write(font_table[font_table_row][i]);
	 }
}

// used to not have to use so many of the putPiece functions and covers two banks rather than 1.
void DisplayNumber(int number, unsigned int x, unsigned int y, int piece) {
	if (number == 0) {
		DisplayBlank(x, y, piece);
	} else {
		GLCD_setCursor(x, y);
		GLCD_putPiece(((number * 2) - 1), piece);
		GLCD_setCursor(x, y + 1);
		GLCD_putPiece((number * 2), piece);
	}
}

// used to display the blank space
void DisplayBlank(unsigned int x, unsigned int y, int piece) {
	GLCD_setCursor(x, y);
	GLCD_putPiece(0, piece);
	GLCD_setCursor(x, y + 1);
	GLCD_putPiece(0, piece);
}

//function to compare array elements from https://www.includehelp.com/code-snippets/c-program-to-compare-two-arrays.aspx
char compareArray(int a[],int b[],int size)	{
	int i;
	for(i=0;i<size;i++){
		if(a[i]!=b[i])
			return 1;
	}
	return 0;
}

// Callback: timer has rolled over
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 // Check which version of the timer triggered this callback and toggle LED
 if (htim == &htim16)
 {
 timer ++;
 }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
