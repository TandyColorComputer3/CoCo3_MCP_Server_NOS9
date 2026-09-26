#ifndef DOD_LOGICAL_H
#define DOD_LOGICAL_H
#define FRAME_BYTES 6144
void wizard_frame(unsigned char *frame,unsigned char fade,unsigned char messages);
void wizard_line(unsigned char *frame,int x0,int y0,int x1,int y1,unsigned char fade);
#endif
