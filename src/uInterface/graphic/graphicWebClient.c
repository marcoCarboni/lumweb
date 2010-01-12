/*
 * graphicWebClient.c
 *
 *  Created on: 05.01.2010
 *      Author: root
 */

#include "graphic/graphicWebClient.h"
#include "graphic/graphicLib.h"
#include "graphic/graphicSettings.h"

#include "lwip/netbuf.h"
#include "lwip/api.h"

void insertGWC(gwcRow* toInsert);
void increase(tWidget *pWidget);
void decrease(tWidget *pWidget);
void backPage(tWidget *pWidget);
void continuePage(tWidget *pWidget);
void drawGWC(int offset);
void addRow(char* id, char* name, long value, char status);
void onCheckboxClick(tWidget *pWidget, unsigned long bSelected);
void loadWeb(tWidget *pWidget);

int aktPage = 1;

void loadMenu(void) {

	aktPage = 1;

	addRow("x1", "Feld 1", 1, (GWC_LABEL | GWC_NUMERIC));
	addRow("x2", "Feld 2", 2, (GWC_LABEL | GWC_NUMERIC));
	addRow("x3", "Feld 3", 3, (GWC_LABEL | GWC_NUMERIC));
	addRow("x4", "Feld 4", 0, (GWC_LABEL | GWC_BOOLEAN));
	addRow("x5", "Feld 5", 5, (GWC_LABEL | GWC_NUMERIC));
	addRow("x6", "Feld 6", 6, (GWC_LABEL | GWC_NUMERIC));

	drawGWC((aktPage - 1) * GWC_ROWS_PER_VIEW);

}

void addRow(char* id, char* name, long value, char status) {
	gwcRow *newRow = (gwcRow*) pvPortMalloc(sizeof(gwcRow));

	printf("New Row %s\n", id);

	newRow->id = id;
	newRow->name = name;
	newRow->value = value;
	newRow->status = status;

	insertGWC(newRow);
}

void insertGWC(gwcRow* toInsert) {
	gwcRow* akt = pgwcRoot;
	if (pgwcRoot == 0) {
		printf("Füge ROOT row ein (%s)\n", toInsert->name);
		pgwcRoot = toInsert;
	} else {
		while (akt->next != 0) {
			akt = akt->next;
		}
		printf("Füge Weitere row ein (%s)\n", toInsert->name);
		akt->next = toInsert;
	}
}

void drawGWC(int offset) {
	gwcRow *akt = pgwcRoot;

	int aktRow = 0;
	int top = GWC_FIRST_ROW_TOP;
	int i;

	printf("aktPage %d\noffset %d\n", aktPage, offset);

	if (offset > 0) {
		for (i = 0; akt->next && i < offset; i++) {
			akt = akt->next;
		}
	} else if (offset < 0) {
		offset = 0;
	}

	initPanel("LumWeb - The Universal Interface");

	for (i = 0; akt != 0 && i < GWC_ROWS_PER_VIEW; i++) {

		printf("Erstelle Zeile %s\n", akt->name);

		if ((akt->status & GWC_LABEL) == GWC_LABEL) {
			akt->nameLabel = addLabel(GWC_ROW_LABEL_LEFT, top,
					GWC_ROW_LABEL_WIDTH, GWC_ROW_HEIGHT, akt->name);
		}

		if ((akt->status & GWC_NUMERIC) == GWC_NUMERIC) {

			if (!akt->stringValue) {
				akt->stringValue = (char*) pvPortMalloc(
						(GWC_ROW_VALUE_MAX_LENGTH + 1) * sizeof(char));
			}
			akt->decrease = addButton(GWC_ROW_DECREASE_BUTTON_LEFT, top,
					GWC_ROW_DECREASE_BUTTON_WIDTH, GWC_ROW_HEIGHT,
					GWC_ROW_DECREASE_BUTTON_SYMBOL,
					GWC_ROW_DECREASE_AUTOREPEAT, decrease);
			snprintf(akt->stringValue, (GWC_ROW_VALUE_MAX_LENGTH + 1), "%d",
					akt->value);
			akt->valueLabel = addLabel(GWC_ROW_VALUE_LEFT, top,
					GWC_ROW_VALUE_WIDTH, GWC_ROW_HEIGHT, akt->stringValue);
			akt->increase = addButton(GWC_ROW_INCREASE_BUTTON_LEFT, top,
					GWC_ROW_INCREASE_BUTTON_WIDTH, GWC_ROW_HEIGHT,
					GWC_ROW_INCREASE_BUTTON_SYMBOL,
					GWC_ROW_INCREASE_AUTOREPEAT, increase);

		} else if (akt->status & GWC_BOOLEAN == GWC_BOOLEAN) {

			akt->checkbox = addCheckbox(GWC_ROW_CHECKBOX_LEFT, top,
					GWC_ROW_CHECKBOX_WIDTH, GWC_ROW_HEIGHT,
					(unsigned long) akt->value, onCheckboxClick);

		}

		akt = akt->next;
		top += GWC_ROW_HEIGHT + GWC_ROW_MARGIN_TOP;

	}

	if (offset > 0) {
		addButton(5, 210, 80, 30, "Back", 0, backPage);
	}
	addButton(90, 210, 140, 30, "Speichern", 0, loadWeb);
	if (akt) {
		addButton(235, 210, 80, 30, "Continue", 0, continuePage);
	}

	printf("Starte output\n");
	drawPanel();
	printf("Ouput erfolgreich\n");
}

void increase(tWidget *pWidget) {
	gwcRow * akt = pgwcRoot;
	while (akt != 0) {
		if (akt->increase == (tPushButtonWidget*) pWidget) {
			break;
		}
		akt = akt->next;
	}
	if (akt->increase == (tPushButtonWidget*) pWidget) {
		akt->value++;

		if (!akt->stringValue) {
			akt->stringValue = (char*) pvPortMalloc((GWC_ROW_VALUE_MAX_LENGTH
					+ 1) * sizeof(char));
		}
		snprintf(akt->stringValue, GWC_ROW_VALUE_MAX_LENGTH + 1, "%d",
				akt->value);
		akt->valueLabel->pcText = akt->stringValue;

		WidgetPaint((tWidget*) akt->valueLabel);
	}
}

void decrease(tWidget *pWidget) {
	gwcRow * akt = pgwcRoot;
	while (akt != 0) {
		if (akt->decrease == (tPushButtonWidget*) pWidget) {
			break;
		}
		akt = akt->next;
	}
	if (akt->decrease == (tPushButtonWidget*) pWidget) {
		akt->value--;

		if (!akt->stringValue) {
			akt->stringValue = (char*) pvPortMalloc((GWC_ROW_VALUE_MAX_LENGTH
					+ 1) * sizeof(char));
		}
		snprintf(akt->stringValue, (GWC_ROW_VALUE_MAX_LENGTH + 1), "%d",
				akt->value);
		akt->valueLabel->pcText = akt->stringValue;

		WidgetPaint((tWidget*) akt->valueLabel);
	}
}

void backPage(tWidget *pWidget) {
	aktPage--;
	printf("vorherige Seite aufgerufen\n");
	drawGWC((aktPage - 1) * GWC_ROWS_PER_VIEW);
}

void continuePage(tWidget *pWidget) {
	aktPage++;
	printf("naechste Seite aufgerufen\n");
	drawGWC((aktPage - 1) * GWC_ROWS_PER_VIEW);
}

void onCheckboxClick(tWidget *pWidget, unsigned long bSelected) {
	gwcRow * akt = pgwcRoot;
	while (akt != 0) {
		if (akt->checkbox == (tCheckBoxWidget*) pWidget) {
			break;
		}
		akt = akt->next;
	}
	if (akt->checkbox == (tCheckBoxWidget*) pWidget) {
		akt->value = bSelected;
		if (bSelected) {
			akt->checkbox->usStyle |= CB_STYLE_SELECTED;
		} else {
			akt->checkbox->usStyle &= ~CB_STYLE_SELECTED;
		}
		WidgetPaint((tWidget*) akt->checkbox);
	}
	printf("Change Checkboxvalue");
}

void loadWeb(tWidget *pWidget) {

	struct netconn *conn;
	struct netbuf *inBuf;
	char pageData[1024];
	char getText[] = "GET / HTTP/1.0\r\n\r\n";
	int length, bindErr, connErr, writeErr;
	u16_t port;
	struct ip_addr remoteIP;

	printf("Starte WEB anfrage\n");
	//while (true) {
	// Create a new socket... API will always create socket 0??
	conn = netconn_new(NETCONN_TCP);
	// There is only one other possible socket open.
	conn->socket = 1;
	remoteIP.addr = htonl(0xC0A81401);

	printf("netconn_connect\n");
	connErr = netconn_connect(conn, &remoteIP, 80);
	printf("connErr = %d\n", connErr);
	if (conn != NULL && connErr == 0) {
		writeErr = netconn_write(conn, &getText, sizeof(getText),
				NETCONN_NOCOPY);
		inBuf = netbuf_new();
		while ((inBuf = netconn_recv(conn)) != NULL) {
			printf("Daten empfangen\n");
			do {
				netbuf_data(inBuf, (void **) &pageData, &length);
				printf("DATA: %s", pageData);
			} while (netbuf_next(inBuf) >= 0);
		}
		if (inBuf != NULL)
			netbuf_delete(inBuf);
	}
	if (conn != NULL) {
		while (netconn_delete(conn) != ERR_OK) {
			vTaskDelay(1);
		}
	}
	printf("fertig\n");
}

