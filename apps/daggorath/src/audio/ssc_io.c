#include "platform.h"
/* CMOC lacks volatile: explicit loads/stores; no interrupt masking. */
Byte ssc_read_io(Word addr){Byte value;asm{
 ldx :addr
 ldb ,x
 stb :value
 }return value;}
void ssc_write_io(Word addr,Byte value){asm{
 ldx :addr
 ldb :value
 stb ,x
 }}
