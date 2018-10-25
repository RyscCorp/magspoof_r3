/*
 * MagSpoof - "wireless" magnetic stripe/credit card emulator
 *
 * Original by Samy Kamkar
 * MagSpoofPI by Salvador Mendoza
 *
 * http://samy.pl/magspoof
 * https://github.com/salmg/MagSpoofPI
 * https://netxing.wordpress.com/2016/08/27/magspoofpi/
 *
 */


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

                            // min 5 IPS, max 50 IPS
#define SWIPE_SPEED   10    // inches per second

#define bool uint8_t
#define HIGH 1
#define LOW 0

#define interrupts() sei()
#define no_interrupts() cli()

/*
#define SINGLE_POLE   // only drive one side of the coil
                      // as described in http://www.idogendel.com/en/archives/507
*/

#define COIL_A PB0
#define COIL_B PB1
#define BUTTON_PIN PB2
#define LED_PIN PB3 
   
// I/O macros
#define PIN_HIGH(pin)   PORTB |= (HIGH << pin)
#define PIN_LOW(pin)    PORTB &= ~(HIGH << pin)
#define PINREAD(pin)    PINB & (HIGH << pin)

const uint16_t leading_zeros[3] = { 10, 20, 10 };
const uint16_t trailing_zeros[3] = { 10, 10, 10 };
const uint8_t track_density[3] = { 210, 75, 210 }; // bits per inch for each track
const uint8_t track_sublen[] = { 32, 48, 48 };
const uint8_t track_bitlen[] = { 7, 5, 5 };

uint32_t track_period_us[3];  // bit period of each track in microseconds
                              // initialized in calc_track_periods()

typedef struct {
  char *tracks[3];  // track data
} card;

// your magstripe card tracks go here
const card test_card = {
  {
    "%B4444444444444444^ABE/LINCOLN^291110100000931?",
    ";4444444444444444=29111010000093100000?",
    0
  },
};

void calc_track_periods()
{
  for (int i = 0; i < 3; i++)
    track_period_us[i] = (uint32_t) (1e6 / (SWIPE_SPEED * track_density[i]));
}

void static inline burn_cycles(uint32_t cycles)
{
  while (cycles > 0) {
    __asm__ volatile ("nop");
    cycles--;
  }
}

void static inline delay_us(uint32_t us)
{
  // obtained divisor of 20 by checking bit period with scope
  // assume loop in burn_cycles requires 20 clock cycles
  uint32_t cycles = (us * (F_CPU / 1000000)) / 20;
  burn_cycles(cycles);
}

void set_pin(uint8_t pin, bool state){
  if (state) {
    PORTB |= (1 << pin); // high
  }
  else {
    PORTB &= ~(1 << pin); // low
  }
}

void blink(int times)
{
  int i;
  for (i = 0; i < times; i++) {
    PIN_HIGH(LED_PIN);
    _delay_ms(200);
    PIN_LOW(LED_PIN);
    _delay_ms(200);
   } 
}

void setup_io()
{
  no_interrupts();

  DDRB |= (1 << COIL_A); // output
  DDRB |= (1 << COIL_B); // output
  DDRB |= (1 << LED_PIN); // output
  // DDRB = 0 by default so

  // BUTTON_PIN configured as input
  PORTB |= (1<<BUTTON_PIN); // set pull-up 
}

void await_button_press()
{
  while (PINREAD(BUTTON_PIN)) // button pin is high when not pressed
    _delay_ms(100);
}

static bool f2f_pole; // used to track signal state during playback (F2F encoding)

static inline void invert_coil_pole()
{
  f2f_pole ^= 1;
  set_pin(COIL_A, f2f_pole);
#ifndef SINGLE_POLE
  set_pin(COIL_B, !f2f_pole);
#endif
}

void f2f_play_bit(bool bit, uint32_t period_us)
{
  // play F2F encoded bit
  // see http://www.edn.com/Home/PrintView?contentItemId=4426351
  const uint32_t half_period = period_us / 2;
  // invert magnetic pole at start of period
  invert_coil_pole();

  delay_us(half_period);

  // at half period, determine whether to invert pole
  if (bit)
    invert_coil_pole();

  delay_us(half_period);
}

void play_zeros(uint8_t n, uint32_t period_us)
{
  for (int i = 0; i < n; i++)
    f2f_play_bit(0, period_us); 
}

void play_byte(
  uint8_t byte, 
  uint8_t n_bits, 
  uint32_t period_us,
  uint8_t *lrc
)
{
    bool parity = 1;
    for (int i = 0; i < n_bits; i++) {
      bool bit = byte & 1;
      parity ^= bit;
                                                // TIMING SENSITIVE
      f2f_play_bit(bit, period_us - 30);        // subtract 30us to compensate for delay 
                                                // caused by extra processing within this loop
      byte >>= 1;
      if (lrc)
        *lrc ^= bit << i;
    }
    f2f_play_bit(parity, period_us);  // last bit is parity
}

void play_track(card *c, uint8_t track_num)
{
  char *track = c->tracks[track_num];

  if (!track) return; // check that the track exists

  uint32_t period_us = track_period_us[track_num];
  char *track_byte;
  uint8_t lrc = 0;

  // lead playback with zeros
  play_zeros(leading_zeros[track_num], period_us);

  // play back track data
  for (track_byte = track; *track_byte != 0; track_byte++) {
    uint8_t tb = *track_byte - track_sublen[track_num];
    play_byte(tb, track_bitlen[track_num] - 1, period_us, &lrc);
  }

  // play LRC
  play_byte(lrc, track_bitlen[track_num] - 1, period_us, 0);

  // end playback
  play_zeros(trailing_zeros[track_num], period_us);
}

void play_card(card *c)
{
  // let user know playback has begun by turning on LED
  PIN_HIGH(LED_PIN);
  PIN_LOW(COIL_A);
  PIN_LOW(COIL_B);

  f2f_pole = 0;

  for (int i = 0; i < 3; i++)
    play_track(c, i);

  // turn off LED and make sure coils are off
  PIN_LOW(LED_PIN);
  PIN_LOW(COIL_A);
  PIN_LOW(COIL_B);
}

int main(void)
{
  setup_io();
  calc_track_periods();

  // blink three times to show we initialized
  blink(3);

  while (1) {
    await_button_press();
    play_card(&test_card);
  }
}

