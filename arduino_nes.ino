#include <Keyboard.h>
#include <stdint.h>

// GPIO pins used for connected gamepad
#define CLOCK 21
#define LATCH 20
#define DATA  19

 enum {
   NES_BUTTON_A,
   NES_BUTTON_B,
   NES_BUTTON_SELECT,
   NES_BUTTON_START,
   NES_DPAD_UP,
   NES_DPAD_DOWN,
   NES_DPAD_LEFT,
   NES_DPAD_RIGHT
 } NES_BUTTON_MAP;

uint8_t BUTTON_STATE[16];

// NES Keyboard Map (configurable)
const int16_t NES_KEYBOARD_MAP[8][2] = {
    {NES_BUTTON_A,        0x16},
    {NES_BUTTON_B,        0x04},
    {NES_BUTTON_SELECT,   0x1D},
    {NES_BUTTON_START,    0x1B},
    {NES_DPAD_UP,         0x52},
    {NES_DPAD_DOWN,       0x51},
    {NES_DPAD_LEFT,       0x50},
    {NES_DPAD_RIGHT,      0x4F},
};

void (*PROCESS_INPUT_FN)(void);

// Delay for approx 100ns. Assumes 16Mhz AtMega CPU
#define delay100ns()
   __asm__ __volatile__ ("nop\n\t");\
   __asm__ __volatile__ ("nop\n\t");

#define SendCmdTakeSample(clock_pin, latch_pin)\
  digitalWrite(clock_pin, HIGH);\
  digitalWrite(latch_pin, HIGH);\
  delay100ns();\
  digitalWrite(latch_pin, LOW);

#define SendCmdSendData(clock_pin)\
  delay100ns();\
  digitalWrite(clock_pin, LOW);

#define SendCmdShiftOut(clock_pin)\
  delay100ns();\
  digitalWrite(clock_pin, HIGH);

void ReadInput(uint8_t *data, int num)
{
  int i;
  SendCmdTakeSample(CLOCK, LATCH);

  // Read output
  for (i = 0; i < num; i++) {
      SendCmdSendData(CLOCK);

      // 1 = Off, 0 = On... Bit must be inverted
      data[i] = (~digitalRead(DATA)) & 0x1;
      SendCmdShiftOut(CLOCK);
    }
}

void ProcessInputNESUsingKeyboard()
{
  int i;
  ReadInput((uint8_t *)&BUTTON_STATE, 8);

  for (i = 0; i < 8; i++) {
      if (BUTTON_STATE[NES_KEYBOARD_MAP[i][0]]) {
            Keyboard.press(NES_KEYBOARD_MAP[i][1]);
          } else {
            Keyboard.release(NES_KEYBOARD_MAP[i][1]);
          }
    }
}

void setup() {
  pinMode(CLOCK, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(DATA, INPUT);
}

void loop() {
  ProcessInputNESUsingKeyboard();
  delayMicroseconds(1);
}
