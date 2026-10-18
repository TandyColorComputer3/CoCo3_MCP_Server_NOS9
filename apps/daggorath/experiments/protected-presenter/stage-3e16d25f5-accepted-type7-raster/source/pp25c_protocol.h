#ifndef PP25C_PROTOCOL_H
#define PP25C_PROTOCOL_H
/* Application-specific D.25C test-format identifier; not a generic bpp code. */
typedef unsigned char PP25CByte;
#define PP25C_FRAME 16
#define PP25C_MAGIC 0xb6
#define PP25C_VERSION 1
#define PP25C_OP_PRESENT 5
#define PP25C_FORMAT_TYPE7_32X8_2BPP 0x72
#define PP25C_X 64
#define PP25C_Y 64
#define PP25C_WIDTH 32
#define PP25C_HEIGHT 8
#define PP25C_ERR_PROTOCOL 187
static void pp25c_make_present(PP25CByte *f){
 PP25CByte i;for(i=0;i<PP25C_FRAME;i++)f[i]=0;
 f[0]=PP25C_MAGIC;f[1]=PP25C_VERSION;f[2]=PP25C_OP_PRESENT;f[3]=1;
 f[7]=PP25C_X;f[9]=PP25C_Y;f[10]=PP25C_WIDTH;f[11]=PP25C_HEIGHT;
 f[12]=PP25C_FORMAT_TYPE7_32X8_2BPP;
}
static PP25CByte pp25c_validate_present(const PP25CByte *f){
 PP25CByte i;
 if(f[0]!=PP25C_MAGIC||f[1]!=PP25C_VERSION||f[2]!=PP25C_OP_PRESENT||f[3]!=1)return PP25C_ERR_PROTOCOL;
 if(f[4]||f[5]||f[6]||f[7]!=PP25C_X||f[8]||f[9]!=PP25C_Y||
    f[10]!=PP25C_WIDTH||f[11]!=PP25C_HEIGHT||f[12]!=PP25C_FORMAT_TYPE7_32X8_2BPP)return PP25C_ERR_PROTOCOL;
 for(i=13;i<PP25C_FRAME;i++)if(f[i])return PP25C_ERR_PROTOCOL;
 if((PP25C_X+PP25C_WIDTH)>640||(PP25C_Y+PP25C_HEIGHT)>200||
    PP25C_Y>=152)return PP25C_ERR_PROTOCOL;
 return 0;
}
#endif
