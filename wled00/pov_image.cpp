#include "wled.h"
#include "FX.h"
#include <PNGdec.h>
#include <AnimatedGIF.h>

AnimatedGIF gif;
PNG png;
File f;
Segment s;

void * fileOpen(const char *filename, int32_t *size) {
    f = WLED_FS.open(filename);
    *size = f.size();
    return &f;
}

void fileClose(void *handle) {
    if (f) f.close();
}

void fileWrite(const char * path, const char * message) {
    f = WLED_FS.open(path, FILE_WRITE);
    f.print(message);
    f.close();
}

void fileRead(const char * path, const char * message) {
    f = WLED_FS.open(path);
    f.print(message);
    f.close();
}

int32_t pngRead(PNGFILE *handle, uint8_t *buffer, int32_t length) {
    if (!f) return 0;
    return f.read(buffer, length);
}

int32_t pngSeek(PNGFILE *handle, int32_t position) {
    if (!f) return 0;
    return f.seek(position);
}

void pngDraw(PNGDRAW *pDraw) {
    uint16_t usPixels[SEGLEN];
    png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_LITTLE_ENDIAN, 0xffffffff);
    for(int x=0; x < SEGLEN; x++) {
	uint16_t color = usPixels[x];
	byte r = ((color >> 11) & 0x1F);
	byte g = ((color >> 5) & 0x3F);
	byte b = (color & 0x1F);
	SEGMENT.setPixelColor(x, RGBW32(r, g, b, 0));
    }
    busses.show();
}

int32_t gifRead(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;
    iBytesRead = iLen;
    File *f = static_cast<File *>(pFile->fHandle);
    // Note: If you read a file all the way to the last byte, seek() stops working
    if ((pFile->iSize - pFile->iPos) < iLen)
	iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    if (iBytesRead <= 0)
	return 0;
    iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
    pFile->iPos = f->position();
    return iBytesRead;
}

int32_t gifSeek(GIFFILE *pFile, int32_t iPosition)
{ 
    int i = micros();
    File *f = static_cast<File *>(pFile->fHandle);
    f->seek(iPosition);
    pFile->iPos = (int32_t)f->position();
    i = micros() - i;
    return pFile->iPos;
}

void gifDraw(GIFDRAW *pDraw) {
    uint8_t r, g, b, *s, *p, *pPal = (uint8_t *)pDraw->pPalette;
    int x, y = pDraw->iY + pDraw->y;
    
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) {
	p = &pPal[pDraw->ucBackground * 3];
	r = p[0]; g = p[1]; b = p[2];
	for (x=0; x<pDraw->iWidth; x++)
	{
	    if (s[x] == pDraw->ucTransparent) {
		SEGMENT.setPixelColor(x, RGBW32(r, g, b, 0));
	    }
	}
	pDraw->ucHasTransparency = 0;
    }

    if (pDraw->ucHasTransparency) {
	const uint8_t ucTransparent = pDraw->ucTransparent;
	for (x=0; x<pDraw->iWidth; x++)	{
	    if (s[x] != ucTransparent) {
		p = &pPal[s[x] * 3];
		SEGMENT.setPixelColor(x, RGBW32(p[0], p[1], p[2], 0));
	    }
	}
    }

    else // no transparency, just copy them all
    {
	for (x=0; x<pDraw->iWidth; x++)
	{
	    p = &pPal[s[x] * 3];
	    SEGMENT.setPixelColor(x, RGBW32(p[0], p[1], p[2], 0));
	}
    }
    busses.show();
}

void pov_image() {
    const char * filepath = SEGMENT.file.c_str();
    int rc = png.open(filepath, fileOpen, fileClose, pngRead, pngSeek, pngDraw);
    if (rc == PNG_SUCCESS) {
	rc = png.decode(NULL, 0);
	png.close();
    }

    /*
    gif.begin(GIF_PALETTE_RGB888); // request 24-bit palette
    rc = gif.open("/test.gif", fileOpen, fileClose, gifRead, gifSeek, gifDraw);
    if (rc) {
	while (gif.playFrame(true, NULL)){
	}
	gif.close();
    }
    //*/
}
