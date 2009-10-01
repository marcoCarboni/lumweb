/*
 * graphicObjects.h
 *
 *  Created on: 23.09.2009
 *      Author: root
 */

#ifndef GRAPHICOBJECTS_H_
#define GRAPHICOBJECTS_H_

static const unsigned char g_pucRIT128x96x4HorizontalInc[] = { 0xA0, 0x52 };

#define NULL (void *) 0

#define UP GPIO_PIN_0
#define DOWN GPIO_PIN_1
#define RIGHT GPIO_PIN_3
#define LEFT GPIO_PIN_2
#define SELECT GPIO_PIN_1

struct goButton {
	int height;
	int width;
	int left;
	int top;
	char unsigned* value;
	struct goButton * next;
};

static const unsigned char pucButtonNormal[] = { 0x06, 0x06, 0x06, 0x06 };
static const unsigned char pucButtonSelected[] = { 0x06, 0x0C, 0x06, 0x0C };
static const unsigned char pucButtonClicked[] = { 0x0C, 0x0C, 0x06, 0x06 };
static const unsigned char pucButtonDeactivated[] = { 0x0F, 0x0F, 0x0F, 0x0F };

typedef struct goButton * pgoButton;

void goInit(void); // Initializes the GraphicLibary

pgoButton goNewButton(int height, int width, int left, int top,
		char unsigned* value); // Creates a new Button

pgoButton goDrawNewButton(int height, int width, int left, int top,
		char unsigned* value); // Creates a new Button

void goDrawButton(pgoButton btn, unsigned const char* type); // Draws the Button

void goDrawButtons(void); // Draws all Buttons

void goDeleteButton(pgoButton btn); // Deletes the Button

void goStartListener(void); // Starts the Listener

static const unsigned char goButtonUp[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
		0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0x0F, 0xFF, 0xFF, 0xF0, 0xFF, 0x0F, 0xFF,
		0xFF, 0x0F, 0xFF, 0xF0, 0xFF, 0xFF, 0x0F, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };

static const unsigned char goButtonDown[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0xF0, 0xFF, 0xFF, 0x0F,
		0xFF, 0xF0, 0xFF, 0xFF, 0xF0, 0xFF, 0x0F, 0xFF, 0xFF, 0xF0, 0xFF, 0x0F,
		0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

#endif /* GRAPHICOBJECTS_H_ */
