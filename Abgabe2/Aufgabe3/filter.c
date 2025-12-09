//Generelle Filter methode:

//Filternamen: smooth, sharp, edge, emboss


//nur Bitmaps mit 24 Bit pro Pixel, ohne Farbpalette und ohne Komprimierung
//Ist es mind 3x3 Pixel
//Apply Filter
//Neue Datei erstellen: geänderter Header

#include <fcntl.h>;
#include <unistd.h>;
#include <errno.h>;
#include <stdlib.h>;

void filter(char filename, char filtername){ //Parameter: Dateiname, Filtername
    //Kann Datei geöffnet werden?
    int bpm = open(filename, O_RDONLY);
    if (bpm == -1) {
        if (errno == ENOENT) {
            printf("Error: File does not exist");
    }
        else {
            printf("Error: Could not open file, code %n", errno);
    }
        exit(EXIT_FAILURE);
    }
    //Stimmt die Signatur?
    

}