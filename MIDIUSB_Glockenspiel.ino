#include "MIDIUSB.h"

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

int mapping[] = {47,37,49,35,33,51,31,53,29,27,38,25,
40,23,42,22,24,44,26,46,28,30,48,32,50,34,52,36};
int min_velocity[] = {2682,2484,2704,2892,3152,3209,
3251,2791,2564,3083,3005,2911,3139,3405,3665,3246,3670,
3542,3392,3685,3402,3432,3534,3132,3562,3002,3698,3644};
int velocity_scale = 10;
int velocity_offset = 32;

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void setup() {
//  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  for(int i=22;i<54;i++){
    pinMode(i, OUTPUT);
  }
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void loop() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0) {
//      Serial.print("Received: ");
//      Serial.print(rx.header, DEC);
//      Serial.print("-");
//      Serial.print(rx.byte1, DEC);
//      Serial.print("-");
//      Serial.print(rx.byte2, DEC);
//      Serial.print("-");
//      Serial.println(rx.byte3, DEC);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1);
      digitalWrite(LED_BUILTIN, LOW);
//      int event_type = int(rx.header);
      int channel = int(rx.byte1);
      int note = int(rx.byte2);
      int velocity = int(rx.byte3);
      if(rx.header == 0x09){
        while(note>71){note-=12;}
        while(note<44){note+=12;}
        if(note>43 & note<72){
          int index = note-44;
          int pin = mapping[index];
          digitalWrite(pin, HIGH);
          delayMicroseconds(min_velocity[index]
            + velocity_scale * (velocity - velocity_offset));
          digitalWrite(pin, LOW);
        }
      }
    }
  } while (rx.header != 0);
}
