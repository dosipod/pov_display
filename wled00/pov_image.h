#ifndef POV_IMAGE_MODE
#define POV_IMAGE_MODE

#include <PNGdec.h>
#include <AnimatedGIF.h>
void * fileOpen(const char *filename, int32_t *size);
void fileClose(void *handle);
void fileWrite(const char * path, const char * message);
void fileRead(const char * path, const char * message);
int32_t pngRead(PNGFILE *handle, uint8_t *buffer, int32_t length);
int32_t pngSeek(PNGFILE *handle, int32_t position);
void pngDraw(PNGDRAW *pDraw);

int32_t gifRead(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
int32_t gifSeek(GIFFILE *pFile, int32_t iPosition);
void gifDraw(GIFDRAW *pDraw);

void pov_image();

#endif
