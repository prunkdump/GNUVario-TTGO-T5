
//#ifndef _VARIOBUTTON_H_
//#define _VARIOBUTTON_H_

#if defined(ESP32)
#include <Arduino.h>
#include "HardwareConfig.h"
#include "Button.h"
//#include DEBOUNCE_MS 5

class VARIOButton {

 public:
    void begin();
    void update();

    void setWakeupButton(uint8_t button);

    // Button API
    #define DEBOUNCE_MS 5
    Button BtnA = Button(BUTTON_A_PIN, true, DEBOUNCE_MS);
    Button BtnB = Button(BUTTON_B_PIN, true, DEBOUNCE_MS);
    Button BtnC = Button(BUTTON_C_PIN, true, DEBOUNCE_MS);

    // SPEAKER

 private:
    uint8_t _wakeupPin;
};

#define STATE_PAGE_INIT    0
#define STATE_PAGE_VARIO   1
#define STATE_PAGE_CONFIG  2
#define STATE_PAGE_STAT    3

class VARIOButtonScheduleur {
	public:
    void update();
		void Set_StatePage(uint8_t state);
		
	private:
		void treatmentBtnA(bool Debounce);
		void treatmentBtnB(bool Debounce);
		void treatmentBtnC(bool Debounce);
		uint8_t StatePage = STATE_PAGE_INIT;
		
};

extern VARIOButton VarioButton;
extern VARIOButtonScheduleur ButtonScheduleur;

#else
#error “This library only supports boards with ESP32 processor.”
#endif


