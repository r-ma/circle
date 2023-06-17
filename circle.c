#include <stdio.h>
#include <stdlib.h>

void circle(void *image, int width, int height, int xc, int yc,
            int radius, unsigned int color);

int main(int argc, char *argv[])
{
    // if (argc < 4)
    //     return 1;
    // char *filename = argv[1];
    // int radius = atoi(argv[2]);
    // unsigned int color = atoi(argv[3]);

    char *filename = "picEx1.bmp";
    int radius = 20;
    unsigned int color = 200;

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Nie można otworzyć pliku.\n");
        return 1;
    }

    // Odczyt nagłówka pliku BMP
    unsigned char bmp_header[54];
    fread(bmp_header, sizeof(unsigned char), 54, file);

    // Pobranie szerokości i wysokości obrazu z nagłówka BMP
    int width = *(int*)&bmp_header[18];
    int height = *(int*)&bmp_header[22];

    // Alokacja pamięci dla obrazu
    void *image = malloc(width * height * sizeof(unsigned char));

    // Odczyt danych obrazu
    fread(image, sizeof(unsigned char), width * height, file);

    fclose(file);

    int xc = width / 2;   // Współrzędna x środka okręgu
    int yc = height / 2;  // Współrzędna y środka okręgu

    circle(image, width, height, xc, yc, radius, color);

    FILE *output_file = fopen("output.bmp", "wb");
    if (output_file != NULL) {

        fwrite(bmp_header, sizeof(unsigned char), 54, output_file);

        fwrite(image, sizeof(unsigned char), width * height, output_file);

        fclose(output_file);
    } else {
        printf("Nie można otworzyć pliku do zapisu.\n");
    }

    free(image);

    return 0;
}