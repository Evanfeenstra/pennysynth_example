/*
 Auto-harmonizing theramin
 by Evan Feenstra
 
 using Mozzi sonification library by
 Tim Barrass 2012-13, CC by-nc-sa.
 */


#include <ADC.h>  // Teensy 3.0/3.1 uncomment this line and install http://github.com/pedvide/ADC
#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/triangle_analogue512_int8.h> // wavetable
#include <tables/saw512_int8.h> // wavetables with 512 values
#include <tables/sin512_int8.h>
#include <tables/triangle512_int8.h>
#include <tables/square_analogue512_int8.h>
#include <tables/square_analogue_pos512_int8.h>//forlLFO
#include <tables/triangle_pos512_int8.h>//forLFO
#include <mozzi_midi.h> // for mtof

#define CONTROL_RATE 128 // powers of 2 please
Oscil<512, AUDIO_RATE> aOsc; // audio oscillator
Oscil <512, AUDIO_RATE> aSin(SIN512_DATA); //HARMONY OSCILLATOR

void setup(){

}

//array of semitone intervals in an octave
//discluding the leading tone
int array[6]={
  0,2,4,5,7,9};
int note;
int light;

void updateControl(){

  //THERAMIN SECTION
    light=mozziAnalogRead(1);
    note=map(light,0,1000,20,150);
    for (int j=0; j<8; j++) {  // check through 8 octaves
      for (int i=0; i<6; i++) { // check notes in each octave
        if (note==array[i]+12*j) {
          if ((i==0) || (i==3) || (i==4)) { // I, IV, V
            aOsc.setFreq(mtof(note_yo));
            aSin.setFreq(mtof(note_yo+16)); // Octave and major third
          }
          else { // ii, iii, vi
            aOsc.setFreq(mtof(note_yo));
            aSin.setFreq(mtof(note_yo+15)); // Octave and minor third
          }
        }
      }
    } 
}

int updateAudio(){
  return (aOsc.next() + aSin.next());
}

void loop(){
  audioHook();
}



