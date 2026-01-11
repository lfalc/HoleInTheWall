/*
 * GameFrame.c
 *
 *  Created on: Nov 14, 2025
 *
 */
#include <string.h> //for strlen
#include <stdio.h>
#include <stdint.h> //for uint types
#include "sensor.h"
#include "USART2.h"
#include "GameFrame.h"
#include "DMA1.h"
#include "GameTimer.h"
#include <stdlib.h>

static const char pc = '^';
volatile uint8_t PC_pos = 1;
uint8_t score = 0;
uint8_t lives = 3;
uint8_t endGameFlag = 0;
uint8_t currNumWalls = 0;
uint8_t wallDistCounter = wallDistance;
uint16_t numBytes = 0;


//externs
volatile uint8_t DMA_busy = 0;
char frameString[frameSize] = {};


struct Wall {
	//posWall is which row (vertically) the wall exists within
	uint8_t posWall;
	uint8_t posHoleStart;
	uint8_t posHoleEnd;

};

struct Wall walls[maxNumWalls] = {};


uint8_t rand_range(uint8_t min, uint8_t max) {
	if (min > max) return min;
	else return (rand() % (max-min+1)) + min;
}

//for use during initialization
size_t hideCursor(char *dest) {

	char hide[] = "\e[?25l";
	memcpy(dest,hide,sizeof(hide));
	return(sizeof(hide));
}


size_t clearScreen(char *dest) {

	//[2J Clear entire screen
	char clear[] = "\e[2J";
	memcpy(dest,clear,sizeof(clear));

	return sizeof(clear);

}

size_t setScrollRegion(char *dest) {

	char commandString[32] = {};
	sprintf(commandString,"\e[2;%dr",terminalHeight-1);
	memcpy(dest,commandString,strlen(commandString));
	return strlen(commandString);
}


void Game_Init() {

	uint16_t numBytes = 0;
	numBytes += clearScreen(frameString+numBytes);
	numBytes += hideCursor(frameString+numBytes);
	numBytes += setScrollRegion(frameString+numBytes);
	Enable_DMA(numBytes);

	walls[0].posWall = 2;
	walls[0].posHoleStart = 30;
	walls[0].posHoleEnd = 50;
	currNumWalls++;

}

void updatePos() {

	//called from sensor.h
	uint32_t PC_ticks = get_ticks();

	//clamping values that are returned outside of col 1-80
	if (PC_ticks < MIN_TICK_BOUNDARY) {
		PC_ticks = MIN_TICK_BOUNDARY;
	} else if (PC_ticks > MAX_TICK_BOUNDARY) {
		PC_ticks = MAX_TICK_BOUNDARY;
	}

	//mapping position to 80 tile spaces
	float offset = (PC_ticks-MIN_TICK_BOUNDARY);
	float tickRange = MAX_TICK_BOUNDARY - MIN_TICK_BOUNDARY;
	PC_pos = (uint8_t) (offset / tickRange * (wallWidth-1)) + 1;

}



void generateNewWall(struct Wall *wall) {

	uint8_t holeSize = rand_range(minHoleSize, maxHoleSize);
	wall->posHoleStart = rand_range(1, wallWidth - holeSize);
	wall->posHoleEnd = wall->posHoleStart + holeSize;
	wall->posWall = wallStartRow;

}


void updateGame(uint8_t speed) {

	if (endGameFlag==1) {
		GameTimer_Disable();
	}


	if (wallDistCounter == 0 && currNumWalls < maxNumWalls) {
		//this generate populates the walls buffer from fresh over time
		generateNewWall(&walls[currNumWalls]);
		currNumWalls++;
		wallDistCounter = wallDistance;
	}

	wallDistCounter--;


	for (int i = 0; i < currNumWalls; i++) {

		walls[i].posWall += speed;
		if (walls[i].posWall == terminalHeight-1) {

			//check if wall is in last position, if it is, then check to increment score
			if (PC_pos < walls[i].posHoleStart || PC_pos > walls[i].posHoleEnd) {
				lives--;
			} else score++;


		}

		if (walls[i].posWall >= terminalHeight) {
						//this generate overwrites an old wall that has reached the bottom
						generateNewWall(&walls[i]);
					}

	}

	if (lives==0) {
		endGameFlag = 1;
	}

	updatePos();

}


void transmit(char *buffer) {

	int i = 0;

	while(buffer[i] != '\0') {
		while (!(USART2->ISR & USART_ISR_TXE));
		USART2->TDR = buffer[i++];
	}

}

void transmitFrame(char *buffer, uint16_t numBytes) {

	DMA_busy = 1;
	Enable_DMA(numBytes);

}




//  move cursur down n lines
// ^[[<n>B

size_t cursorMove(char *dest, uint8_t col, uint8_t row) {
    // Build VT100 absolute cursor movement sequence
	// supports up to 8 bytes of commands
    char cursorSeq[16] = {};

    sprintf(cursorSeq, "\e[%d;%dH",row,col);
    // \e indicates an escape sequence to handle (equivalent to '\033')
    // [H resets cursor to starting position
    // [%dB moves cursor down by %d lines
    // [%dC moves cursor right by %d lines
    // [<row>;<col>H uses absolute positioning without the need for above commands
    // adding +1 for conversion from 0 indexing to 1 indexing format
    // feeding row 0, col 0 will result in a shift


    // Copy the generated sequence into the frame buffer
    memcpy(dest, cursorSeq, strlen(cursorSeq));
    return strlen(cursorSeq);

}



size_t insertLine(char *dest) {

	//[L inserts a new line
	char insert[] = "\e[L";
	memcpy(dest,insert,sizeof(insert));

	return sizeof(insert);

}

size_t clearLine(char *dest) {

	//[2k Clears current line
	char insert[] = "\e[2K";
	memcpy(dest,insert,sizeof(insert));

	return sizeof(insert);

}



size_t addWall(char *dest, struct Wall *wall) {

	char wallString[wallWidth] = {};

	uint8_t nLeft = wall->posHoleStart;
	uint8_t nCenter = wall->posHoleEnd-wall->posHoleStart;
	uint8_t nRight = wallWidth - nCenter - nLeft;

	memset(wallString, '_', nLeft);
	memset(wallString+nLeft, ' ', nCenter);
	memset(wallString+nCenter+nLeft, '_', nRight);

	memcpy(dest,wallString,wallWidth);
    return wallWidth;

}


size_t insertInfo(char *dest) {

	//assumes cursor is already at correct position
	char infoString[32] = {};
	sprintf(infoString, "LIVES:%u SCORE:%u",lives,score);
	memcpy(dest,infoString,strlen(infoString));
	return strlen(infoString);

}


size_t addPC(char *dest){

	uint8_t nBytes = 0;
	nBytes = cursorMove(dest,PC_pos,terminalHeight-1);
	//this insets ^ to represent the player character to frame buffer
	memcpy(dest+nBytes, &pc, sizeof(char));
	return sizeof(char) + nBytes;
}


void buildFrame() {

	//necessary wait condition to allow synchronization (memory unchanged until transfer complete)
	while(DMA_busy){}
	numBytes = 0;

	numBytes += cursorMove(frameString+numBytes,1,terminalHeight-1);
	numBytes += addPC(frameString+numBytes);

	numBytes += cursorMove(frameString+numBytes,1,1);
	numBytes += clearLine(frameString+numBytes);
	numBytes += insertInfo(frameString+numBytes);

	for(int i = 0; i < currNumWalls; i++) {

		if (walls[i].posWall < terminalHeight) {
			numBytes += cursorMove(frameString+numBytes,1,walls[i].posWall);
			numBytes += addWall(frameString+numBytes,&walls[i]);
		}


	}

	//could use a dedicated VT100 command here for home position, would require an additional command implementation
	numBytes += cursorMove(frameString+numBytes,1,2);
	numBytes += insertLine(frameString+numBytes);

	Enable_DMA(numBytes);
	updateGame(1);

}





