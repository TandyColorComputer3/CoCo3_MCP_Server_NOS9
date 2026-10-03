#ifndef DOD_LOGICAL_H
#define DOD_LOGICAL_H
#define FRAME_BYTES 6144
void wizard_frame(unsigned char *frame,unsigned char fade,unsigned char messages);
void wizard_copyright(unsigned char *frame);
void wizard_line(unsigned char *frame,int x0,int y0,int x1,int y1,unsigned char fade);
/* Same original-derived rasterizer with bounded foreground service points.
 * The callback may present the completed status heart, never the partial line. */
unsigned char wizard_line_progress(unsigned char *frame,int x0,int y0,int x1,int y1,
                                   unsigned char fade,unsigned char (*progress)(void *),void *context);
#endif
