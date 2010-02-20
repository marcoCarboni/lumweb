/**
 * \addtogroup Graphic
 * @{
 *
 * \file touchAction.h
 * \author Anziner, Hahn
 * \brief
 *
 */

#ifndef TOUCHACTIONS_H_
#define TOUCHACTIONS_H_

#include "grlib/grlib.h"
#include "grlib/widget.h"

#include "graphic/gui/displayBasics.h"

void vTouchLoadMenu(tWidget *pWidget);
void vTouchStoreValues(tWidget *pWidget);
void vPageUp(tWidget *pWidget);
void vPageDown(tWidget *pWidget);
void vHyperlinkAction(tWidget *pWidget);
void vCheckboxAction(tWidget* pWidget, unsigned long status);
void vIntegerEditorAction(tWidget *pWidget);
void vFloatEditorAction(tWidget *pWidget);
void vTimeEditorAction(tWidget *pWidget);

#endif /* TOUCHACTIONS_H_ */
