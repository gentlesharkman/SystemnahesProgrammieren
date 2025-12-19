#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define BMP_HEADER_SIZE 54

int main(int argc, char *argv[])
{

    if (argc <3)
    {
        printf("Usage: %s <file.bmp> <filter>\n", argv[0]);
        printf("Available filters: edge, smooth\n");
        exit(EXIT_FAILURE);
    }

    char *input_filename = argv[1];
    char *filter = argv[2];

    int fd = open(input_filename, O_RDWR);
    if (fd == -1)
    {
        if (errno == ENOENT)
        {
            printf("Error: File does not exist");
        }
        exit(EXIT_FAILURE);
    }

    unsigned char bmp_header[BMP_HEADER_SIZE];

    // Read the bitmap header
    ssize_t bytes_read = read(fd, bmp_header, BMP_HEADER_SIZE);
    // If there are less bytes read, then the file is not a valid bitmap file
    if (bytes_read != BMP_HEADER_SIZE)
    {
        printf("Error: Invalid bitmap header\n");
        exit(EXIT_FAILURE);
    }

    // Check the file signature to make sure it's a bitmap file
    if (bmp_header[0] != 'B' || bmp_header[1] != 'M')
    {
        printf("Error: It's not a bitmap image\n");
        exit(EXIT_FAILURE);
    }

    // Check 24 Bits pro Pixel
    if (bmp_header[28] != 24)
    {
        printf("Error: The Pixel Depth is not 24\n");
        exit(EXIT_FAILURE);
    }

    // Get the image size, width, height and bit depth
    uint32_t image_size = *(uint32_t *)&bmp_header[2];
    int32_t width = *(int32_t *)&bmp_header[18];
    int32_t height = *(int32_t *)&bmp_header[22];
    uint16_t bit_depth = *(uint16_t *)&bmp_header[28];


    // Allocate memory for the pixel data
    unsigned char *pixel_data = (unsigned char *)malloc(image_size - BMP_HEADER_SIZE);
    // If the memory allocation fails, exit the program
    if (pixel_data == NULL)
    {
        printf("Error: Failed to allocate memory for pixel data\n");
        exit(EXIT_FAILURE);
    }
    else if (width < 3 || height < 3)
    {
        printf("Error: Image too small. Atleast 3x3 to apply filter\n");
        exit(EXIT_FAILURE);
    }

    // Read the pixel data
    bytes_read = read(fd, pixel_data, image_size - BMP_HEADER_SIZE);
    if (bytes_read != image_size - BMP_HEADER_SIZE)
    {
        printf("Error: Failed to read pixel data\n");
        exit(EXIT_FAILURE);
    }

    // Bytes pro Pixel
    int bytes_per_pixel = bit_depth / 8; // Careful: this only works for bit depths which are a multiples of 8
    // Pixel-Bytes pro Reihe
    int pixel_bytes_per_row = width * bytes_per_pixel;

    // Total Bytes pro Reihe mit Padding
    int total_bytes_per_row = (pixel_bytes_per_row + 3) & ~3;

    // Neue Breite und Höhe (gecroppt)
    int cropped_width = width - 2;
    int cropped_height = height - 2;
    int cropped_pixel_bytes_per_row = cropped_width * bytes_per_pixel;
    int cropped_total_bytes_per_row = (cropped_pixel_bytes_per_row + 3) & ~3;

    // Speicher für gefiltertes, gecropptes Bild
    unsigned char* cropped_data = malloc(cropped_total_bytes_per_row * cropped_height);

    for (int y = 1; y < height - 1; y++)
    {
        int src_offset = y * total_bytes_per_row + 1 * bytes_per_pixel;
        int dst_offset = (y - 1) * cropped_total_bytes_per_row;

        memcpy(cropped_data + dst_offset,
               pixel_data + src_offset,
               cropped_pixel_bytes_per_row);
    }

    // 3x3 Kernel
    const int EDGE_KERNEL[3][3] = {
        {-1, -1, -1},
        {-1, 8, -1},
        {-1, -1, -1}};

    const int SMOOTH_KERNEL[3][3] = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}};

    const int (*kernel)[3];
    if (strcmp(filter, "edge") == 0){
        kernel = EDGE_KERNEL;
    }
    else if (strcmp(filter, "smooth") == 0) {
        kernel = SMOOTH_KERNEL;
    };

    for (int y = 0; y < cropped_height; y++) {
        int orig_row = (height - 1) - (y + 1);
        for (int x = 0; x < cropped_width; x++) {
            int orig_col = x + 1;
            int sumR = 0, sumG = 0, sumB = 0;
            for(int fy = -1; fy <= 1; fy++){
                int neigh_row = orig_row + fy;
                int row_offset = neigh_row * total_bytes_per_row;
                for(int fx = -1; fx <= 1; fx++){
                    int neigh_col = orig_col + fx;
                    int neigh_idx = row_offset + neigh_col * bytes_per_pixel;

                    int R = neigh_idx + 2;
                    int G = neigh_idx + 1;
                    int B = neigh_idx;

                    int k = kernel[fy + 1][fx + 1];

                    sumR += pixel_data[R] * k;
                    sumG += pixel_data[G] * k;
                    sumB += pixel_data[B] * k;
                }
            }

            if (kernel == EDGE_KERNEL)
            {
                if (sumG < 0) sumG = 0; else if (sumG > 255) sumG = 255;
                if (sumR < 0) sumR = 0; else if (sumR > 255) sumR = 255;
                if (sumB < 0) sumB = 0; else if (sumB > 255) sumB = 255;
            }
            else if (kernel == SMOOTH_KERNEL)
            {
                sumR = (int)(sumR / 9);
                sumG = (int)(sumG / 9);
                sumB = (int)(sumB / 9);
            }

            int dst_idx = y * cropped_total_bytes_per_row + x * bytes_per_pixel;

            cropped_data[dst_idx + 2] = sumR;
            cropped_data[dst_idx + 1] = sumG;
            cropped_data[dst_idx + 0] = sumB;

        }
    }

    // Neuen BMP Header erstellen
    unsigned char new_header[BMP_HEADER_SIZE];
    memcpy(new_header, bmp_header, BMP_HEADER_SIZE);

    // Neue Dimensionen setzen
    *(int32_t *)&new_header[18] = cropped_width;
    *(int32_t *)&new_header[22] = - cropped_height;

    // Neue Bildgröße
    uint32_t new_image_size = cropped_total_bytes_per_row * cropped_height + BMP_HEADER_SIZE;
    *(uint32_t *)&new_header[2] = new_image_size;
    *(uint32_t *)&new_header[34] = cropped_total_bytes_per_row * cropped_height;

    // Ausgabe-Datei erstellen

    char output_filename[256]; 
    char *dot = strrchr(input_filename, '.'); 
    if (dot != NULL && strcmp(dot, ".bmp") == 0) { 
        *dot = '\0'; // .bmp entfernen 
    }
    
    snprintf(output_filename, sizeof(output_filename), "%s_%s.bmp", input_filename, filter);
    int out_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1)
    {
        perror("Error creating output file");
        free(cropped_data);
        free(pixel_data);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Header + Pixel schreiben
    write(out_fd, new_header, BMP_HEADER_SIZE);
    write(out_fd, cropped_data, cropped_total_bytes_per_row * cropped_height);

    printf("Created filtered and cropped image: %s\n", output_filename);

    free(cropped_data);
    free(pixel_data);
    close(fd);
    close(out_fd);
}