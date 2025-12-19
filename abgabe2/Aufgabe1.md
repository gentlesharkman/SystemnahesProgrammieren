a) Schauen Sie sich das Feld für die Dateigröße der Datei A.bmp im File-Header an und ermitteln Sie daraus die Byte-Reihenfolge (endianness).
78 01 00 00 sind die 4 Bytes: bmp ist little endian also für uns leserlicher 01 78 was in bits folgendem entspricht: 0000 0001 0111 1000 = 376 
Also die Datei ist 376 Bytes groß

b) Geben Sie alle Werte für den BITMAPINFOHEADER der Datei A.bmp an.
• Header-Größe:                                             40
• Bildbreite in Pixel (4 Bytes, signed):                    10
• Bildhöhe in Pixel (4 Bytes, signed):                      10
• Anzahl der Farbebenen (2 Bytes):                          1
• Bits pro Pixel (2 Bytes):                                 24
• Kompressionsmethode (4 Bytes):                            0
• Bildgröße (Größe des Bildbereichs in Bytes) (4 Bytes):    322?
• Horizontale Auflösung (DPI oder PPM) (4 Bytes, signed):   7874 
• Vertikale Auflösung (DPI oder PPM) (4 Bytes, signed):       7874
• Anzahl der Farben in der Palette (4 Bytes):               0
• Anzahl der wichtigen Farben (4 Bytes):                    0

c) Vergleichen Sie die Werte für Bildbreite und Bildhöhe der Dateien C_1.bmp und C_2.bmp und stellen Sie eine Hypothese auf, was dies bedeuten könnte.
C_1 Bildbreite: 12
C_1 Bildhöhe:   12

C_2 Bildbreite: 12 
C_2 Bildhöhe:   F4 FF FF FF aber in signed auch 0C 00 00 00 und damit 12


d) Vergleichen Sie den BITMAPINFOHEADER von C_3.bmp mit dem, der anderen Dateien. Was fällt Ihnen auf?
C3 ist viel kleiner. Andere Bildgröße und kleinere Horizontale + Vertikale Auflösung