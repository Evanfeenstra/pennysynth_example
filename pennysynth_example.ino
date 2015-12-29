/*
 Auto-harmonizing theramin
 by Evan Feenstra
 
 using Mozzi sonification library by
 Tim Barrass 2012-13, CC by-nc-sa.
 
 Circuit on a Teensy3.2:
   Photocell from pin 14 to +5V
   10k resistor from pin 14 to ground
   Potentiometer on pin 15 to select waveforms
   
 For 44.1 Khz, 16 bit audio use Teensy 3, and install http://github.com/pedvide/ADC
*/

#include <ADC.h>  // comment this line out if not using teensy
#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/triangle_analogue512_int8.h> // wavetables for oscillators
#include <tables/saw512_int8.h>
#include <tables/sin512_int8.h>
#include <tables/triangle512_int8.h>
#include <tables/square_analogue512_int8.h>
#include <mozzi_midi.h> // for mtof
#include <LowPassFilter.h>

#define CONTROL_RATE 128 // powers of 2 please
Oscil<512, AUDIO_RATE> aOsc; // main oscillator
Oscil <512, AUDIO_RATE> aSin(SIN512_DATA); //Harmony oscillator
Oscil <512, CONTROL_RATE> kLFO(TRIANGLE512_DATA); //LFO waveform
LowPassFilter lpf;  //Low Pass filter

void setup(
 kLFO.setFreq(2.63f);
 lpf.setResonance(100);  //LPF
 pinMode(0, INPUT_PULLUP);
){}

//array of semitone intervals in an octave
//discluding the leading tone
int semi[6]={
  0,2,4,5,7,9};
int note;

void updateControl(){

  //Theramin
    int light=mozziAnalogRead(0); //photocell on analog pin 0
    note=map(light,0,1000,20,150);
    for (int j=0; j<8; j++) {  // 8 octaves
      for (int i=0; i<6; i++) { // 6 notes in each octave
        if (note==semi[i]+12*j) {
          if ((i==0) || (i==3) || (i==4)) { // I, IV, V
            aOsc.setFreq(mtof(note));
            aSin.setFreq(mtof(note+16)); // Octave and major third
          }
          else { // ii, iii, vi
            aOsc.setFreq(mtof(note));
            aSin.setFreq(mtof(note+15)); // Octave and minor third
          }
        }
      }
    } 
    
    // FILTER and LFO
     int knob1 = mozziAnalogRead(7); // knob on analog pin 7
     int light1=mozziAnalogRead(1); //photocell on analog pin 1
     float lfo_speed=(light1)/100;
     kLFO.setFreq(lfo_speed);
     if (digitalRead(0)==0) {
       float lfo_amount=(mozziAnalogRead(1)/100+1);
       byte cutoff_freq = kLFO.next(); // the lfo waveform mods the filter cutoff
       lpf.setCutoffFreq(cutoff_freq/lfo_amount); //and the frequency modded by light1
   
       if (cutoff_freq>63) { 
         digitalWrite(2, HIGH); 
       }  // set the LED on pin 2 on
       if (cutoff_freq<64) { 
         digitalWrite(2, LOW); 
       }
     }
     else {
       lpf.setCutoffFreq(knob1/10);
     }
    
    //change the oscillator waveform with knob on analog pin 1
    mode = map(mozziAnalogRead(0), 0, 1023, 0, 3); // automatically map an input value to an output range
    if (mode == 0) {
      aOsc.setTable(SIN512_DATA);
    }
    else if (mode == 1) {
      aOsc.setTable(SAW512_DATA);
    }
    else if (mode == 2) {
      aOsc.setTable(TRIANGLE512_DATA);
    }
    else if (mode == 3) {
      aOsc.setTable(SQUARE_ANALOGUE512_DATA); 
      aSin.setTable(SAW512_DATA);
    }
}

int updateAudio(){
  //add together the oscillators
  return (int)lpf.next(aOsc.next() + aSin.next());
}

void loop(){
  audioHook();
}



