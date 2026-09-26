#include "audio.h"
void audio_frame(Byte *f,Byte op,Byte sound,Byte gain,Byte seq){
 f[0]=AUDIO_MAGIC;f[1]=AUDIO_VERSION;f[2]=op;f[3]=sound;
 f[4]=gain;f[5]=seq;f[6]=0;f[7]=0;
}
Byte audio_validate(const Byte *f){
 if(f[0]!=AUDIO_MAGIC||f[1]!=AUDIO_VERSION||f[6]||f[7])return AUDIO_BAD;
 if(f[2]<AUDIO_PLAY||f[2]>AUDIO_SHUTDOWN)return AUDIO_UNSUPPORTED;
 if(f[2]==AUDIO_PLAY){if((f[3]!=AUDIO_SQUEAK&&f[3]!=AUDIO_PHASER&&f[3]!=AUDIO_WHOOP)||f[4]!=255)return AUDIO_UNSUPPORTED;}
 else if(f[3]||f[4])return AUDIO_BAD;
 return 0;
}
