#include "Keypad4x4.h"
#include "8051.h"
#define TIMER_VAL 15536
#define TEMPO 5
#define NORMAL 0
#define RECORD 1
#define DISPLAY 2
#define LOW_KEY 0
#define NORMAL_KEY 1
#define HIGH_KEY 2
#define REST 12

unsigned short F2T(unsigned char scale, unsigned short notef, signed char displacement);

void setTimer();

void initialize();

void delay(unsigned char s);

void debounce();

void show_digits();

signed char key;
unsigned char mode;
unsigned char time_count;
unsigned char record_index;
signed char displacement;
unsigned short note;
unsigned char scale;
unsigned char digits[] = {0x03, 0x9F, 0x24, 0x0C, 0x98, 0x48, 0x40, 0x1F, 0x00, 0x08, 0x10, 0xC0, 0xFF};
                        // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, b

__xdata int record_note[100]; // xdata can saves 1024B
__xdata unsigned char record_scale[100];
unsigned short note_frequency[] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 0};
                                // DO, DO#, RE, RE#, MI, FA, FA#, SO, SO#, LA, LA#, SI

// T = 65536 - N = 65536 - Clock_frequency / (2 * Note_frequency)
unsigned short F2T(unsigned char scale, unsigned short key, signed char displacement)
{
    if (note_frequency[key] == 0)       // don't display any sound
        return 0;
    else {
        if (displacement == 1 && key == 11) {           // if key + displacement exceed upper boundary, increase scale to higher scale
            scale *= 2;
            key = 0;
        }
        else if (displacement == -1 && key == 0) {      // if key + displacement exceed lower boundary, decrease scale to lower scale
            scale /= 2;
            key = 11;
        }
        else {
            key = key + displacement;
        }
        return 65536 - (500000 / (note_frequency[key] * scale));    // calculate interrupt frequnecy with formula
    }
}


// initial timer
void setTimer()
{
    // set timer 0, timer 1
    TMOD = 0x11;    // turn timer 0 & timer 1 on
                    // GATE=0, CT=0, (M1,M0)=01
                    // GATE=0, CT=0, (M1,M0)=01
                    // mode 1: 16bits (65535)

    TH0 = 0x9E;             //(65536-25000) / 256;
    TL0 = 0x58;             //(65536-25000) % 256;

    TR0 = 1;        // enable timer 0
    TR1 = 1;        // enable timer 1
}


// iniital variables
void initialize()
{
    key = -1;
    mode = NORMAL;
    time_count = 0;
    record_index = 0;;
    displacement = 0;
    note = 0;
    scale = 2;
}


// generate short time interval
void delay(unsigned char s)
{
    unsigned char i;
    unsigned int j;
    for(i = 0; i < s; i++)
        for (j = 0; j < 1024; j++) {}
}


// prevent extra activations from triggering too often
void debounce()
{
    while (pushKey() != -1);  // Wait until all keys released
    delay(3);
}


// show mode, displacement, key and scale digits
void show_digits()
{
    P1 = (P1 & 0xF0);
    P1 += 0xE;  // D1

    if (mode == NORMAL || mode == RECORD)
        P2 = digits[scale];
    else if (mode == DISPLAY)
        P2 = digits[record_scale[record_index]];
    delay(5);   // show the digits currenctly playing

    P1 = (P1 & 0xF0);
    P1 += 0x7;  // D4
    P2 = digits[mode];
    delay(5);   // show present mode

    P1 = (P1 & 0xF0);
    P1 += 0xD;  // D2
    if (mode == NORMAL || mode == RECORD) {
        if (key >= 0 && key <= 11)
            P2 = digits[key];
    }
    else if (mode == DISPLAY)
        P2 = digits[record_note[record_index]];
    delay(5);   // show the digits currenctly playing

    P1 = (P1 & 0xF0);
    P1 += 0xB;  // D3
    P2 = digits[displacement + 1];
    delay(5);   // show the displacement now using  
}


int main()
{
    IE = 0x8A;              // 1000 1010
                            // EA=1 (Enable Interrupts)
                            // ET1=1 (Enable timer 1 interrupt)
                            // ET0=1 (Enable timer 0 interrupt)

    P1_4 = 1;               // initial speaker to down

    initialize();
    setTimer();
    
    while (1) {
        key = pushKey();            // get pressed key
        switch (key) {
            case 12:                // change scale if pressed C
                if (scale < 8)
                    scale *= 2;
                else
                    scale = 2;
                
                debounce();
                break;
            case 13:                // change displacement if pressed D
                if (displacement < 1)
                    displacement += 1;
                else
                    displacement = -1;

                debounce();
                break;
            case 14:                // change mode to RECORD if pressed E
                record_index = 0;
                if (mode != RECORD) {       // detect to turn down or turn on RECORD
                    mode = RECORD;
                    for (unsigned char i = 0; i < 100; i++)         // initial record note
                        record_note[i] = REST;
                    for (unsigned char i = 0; i < 100; i++)         // initial record scale
                        record_scale[i] = 1;
                }
                else
                    mode = NORMAL;
                
                debounce();
                break;
            case 15:                // change mode to DISPLAY if pressed F
                record_index = 0;
                note = 0;
                if (mode != DISPLAY)
                    mode = DISPLAY;             
                else
                    mode = NORMAL;
                
                debounce();
                break;
        }


        show_digits();          // show digits on 7-seg LED

        switch (mode) {
            case NORMAL:
            case RECORD:        // if mode is normal or record, use key to caculate interrupt frequency
                if (key >= 0 && key <= 11)
                    note = F2T(scale, key, displacement);
                break;
            case DISPLAY:       // if mode is display, use record to caculate interrupt frequency
                note = F2T(record_scale[record_index], record_note[record_index], displacement);
                break;
        }
    }
}


void timer1_interrupt(void) __interrupt 3
{
    TF1 = 0;            // turn overflow bits off

    if (mode == DISPLAY || (key >= 0 && key <= 11)) {          // set frequency to note to play correct sound
        if (note == 0)
                P1_4 = 1;
        else {
            TH1 = note / 256;
            TL1 = note % 256;
            P1_4 = !P1_4;  
        }
    }   
}

// unsigned char out = 0;
void timer0_interrupt(void) __interrupt 1
{
    TF0 = 0;            // turn overflow bits off

    time_count += 1;
    if (time_count == TEMPO) {       // after 5 times (0.125s) count += 1
        if (mode == RECORD) {
            if(key >= 0 && key <= 11) {
                record_note[record_index] = key;
                record_scale[record_index] = scale;
            }
        }
        record_index++;
        if (record_index >= 100)    // replay if exceed boundary
            record_index = 0;
        time_count = 0;             // reset time_count
    }


    // reset timer 0
    TH0 = 0x9E;         // (65536-25000) / 256;
    TL0 = 0x58;         // (65536-25000) % 256;
    // each interrupt represnt 0.025s
}
