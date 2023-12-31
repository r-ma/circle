#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define HEADER_SIZE 54+84 // BMP header len
#define BPP 2 // expected bytes per pixel

int circle_asm(void *image, int width, int height, int xc, int yc,
            int radius, unsigned int color);
int circle_asm2(void *image, int width, int height, int xc, int yc,
            int radius, unsigned int color);

void draw_pixel(void* image, int width, int height, int x, int y,  unsigned short color);


void draw_point(void* image, int width, int height, int x, int y,  unsigned short color) {
	short* addr = (short*)(image + width * BPP * y + x * BPP);
	*addr = color;
	printf("addr:%llu\n",(unsigned long long)addr);
}


int circle_c(void *image, int width, int height, int xc, int yc,
            int radius, unsigned int color) {
	int rad2 = radius * radius;

	printf("buffer:%llu\n",(unsigned long long)image);
	for(int x=0;x<radius;x++) {
		int x2 = x * x;
		int y = sqrt(rad2-x2);
		draw_pixel(image,width,height,xc+x,yc+y,color);
		draw_pixel(image,width,height,xc-x,yc+y,color);
		draw_pixel(image,width,height,xc-x,yc-y,color);
		draw_pixel(image,width,height,xc+x,yc-y,color);

		printf("x=%d, y=%d, xc=%d, yc=%d\n",x,y,xc+x,yc+y);

	}
	return 0;
}

int circle_c2(void *image, int width, int height, int xc, int yc,
            int radius, unsigned int color) {
   int x = 0;
   int y = radius;
   int delta = 1 - 2 * radius;
   int error = 0;
   while (y >= x) {
       draw_pixel(image,width,height,xc + x, yc + y, color);
       draw_pixel(image,width,height,xc + x, yc - y, color);
       draw_pixel(image,width,height,xc - x, yc + y, color);
       draw_pixel(image,width,height,xc - x, yc - y, color);
       draw_pixel(image,width,height,xc + y, yc + x, color);
       draw_pixel(image,width,height,xc + y, yc - x, color);
       draw_pixel(image,width,height,xc - y, yc + x, color);
       draw_pixel(image,width,height,xc - y, yc - x, color);
       error = 2 * (delta + y) - 1;
       if ((delta < 0) && (error <= 0)) {
           delta += 2 * ++x + 1;
           continue;
       }
       if ((delta > 0) && (error > 0)) {
           delta -= 2 * --y + 1;
           continue;
	   break;
       }
       delta += 2 * (++x - --y);
   }
}

void debug(unsigned long long i) {
	printf("debug(%llu)\n",i);
}

void udebug(long long i) {
	printf("debug(%lld)\n",i);
}

int sqrt_c(unsigned long long i) {
	unsigned long long result = sqrt(i);
//	printf("sqrt_c(%llu) retruns %llu\n",i,result);
	return result;
}

int main(int argc, char *argv[])
{
    // if (argc < 4)
    //     return 1;
    // char *filename = argv[1];
    // int radius = atoi(argv[2]);
    // unsigned int color = atoi(argv[3]);

    char *filename = "picEx1.bmp";
    int radius = 100;
    unsigned int color = 0x3542;

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Nie można otworzyć pliku.\n");
        return 1;
    }

    // Odczyt nagłówka pliku BMP
    unsigned char bmp_header[HEADER_SIZE];
    fread(bmp_header, sizeof(unsigned char), HEADER_SIZE, file);

    // Pobranie szerokości i wysokości obrazu z nagłówka BMP
    int width = *(int*)&bmp_header[18];
    int height = *(int*)&bmp_header[22];
    int bytePerPixel = *(short*)&bmp_header[28]/8;
    printf("w:%d, h:%d, bpp:%d\n",width,height,bytePerPixel);

    // Alokacja pamięci dla obrazu
    void *image = malloc(width * height * bytePerPixel);

    // Odczyt danych obrazu
    fread(image, bytePerPixel, width * height, file);

    fclose(file);

    int xc = width / 2;   // Współrzędna x środka okręgu
    int yc = height / 2;  // Współrzędna y środka okręgu

    circle_asm2(image, width, height, xc, yc, radius, color);
    circle_c2(image, width, height, xc+20, yc+20, radius, color);

    FILE *output_file = fopen("output.bmp", "wb");
    if (output_file != NULL) {

        fwrite(bmp_header, sizeof(unsigned char), HEADER_SIZE, output_file);

        fwrite(image, bytePerPixel, width * height, output_file);

        fclose(output_file);
    } else {
        printf("Nie można otworzyć pliku do zapisu.\n");
    }

    free(image);

    return 0;
}
