a) Schauen Sie sich die Pixeldaten von A.bmp und B.bmp an und machen Sie sich bewusst, welche Werte für die Bildbreite und Bildhöhe festgelegt sind. Anhand dieser Dimensionen, versuchen Sie nun zu verstehen, wie die blauen, grünen und roten Pixel jeweils abgespeichert werden. Was fällt Ihnen auf?
Hinweis: A.bmp hat vier Reihen blaue Pixel, B.bmp hingegen nur drei.

A ist 10x10. B ist 9x9
Nach den beiden Header Blöcken fängt .bmp von unten nach oben ab zu speichern. Also erst werden die 4 Zeilen blau gespeichert, dann die 3 grünen dann die 3 roten. Und dazu wird für jeden pixel 3 bytes gebraucht. Diese sind aber nicht in der reihnfolge r g b sondern auch umgedreht, also erst wird der blau wert gespeichert dann gelb dann rot. 
Also bei RGB würde FF 00 00 für Rot stehen. Hier in der Codierung aber für Blau weil das erster gespeichert wird
Und noch dazu: bmp speichert pro zeile ein vielfaches von 4 (modulo 4) bytes, da bei A die maßen 10x10 sind, wird für die erste zeile 10 pixel * 3 byte pro pixel = 30 byte. Aber 30 ist kein vielfaches von 4, also werden noch 2 Bytes drangehängt an die zeile.
deswegen ergibt sich auch für die Bildgröße in den Headerinformationen 10 * 32 bytes = 320 (also in den header steht 322, keine ahnung wo nochmal die 2 bytes herkommen) und nicht wie es vielleicht pixelhöhe * pixelbreite * bytes pro pixel was 10 * 10 * 3 was 300 entsprechen würde.



b) Versuchen Sie nachfolgend die Pixeldaten in C_1.bmp und C_2.bmp zu interpretieren. Er-
klären Sie, warum der Computer das Bild in beiden Fällen gleich darstellen kann und welche
grundlegende Idee dahintersteckt.

das eine wird bottom-up gespeichert das andere up-bottom.
liegt alles an den 4 bytes von bildhöhe 
wenn bildhöhe positiv ist dann wird von unten nach oben gespeichert.
wenn bildhöhe negativ ist wird von oben-unten gespeichert.


c) Die Datei C_3.bmp ist wesentlich kleiner als C_1.bmp und C_2.bmp, obwohl der gleiche Bildinhalt dargestellt wird. Wie wird in C_3.bmp der Bildinhalt komprimiert und anhand von welchen Werten könnte der Computer dies feststellen? Sie müssen vollständig erklären, wie das Bild rekonstruiert werden kann!

C_3 hat 8 zusätzliche Bytes bevor das Bild beginnt, da dort die Farbpalette definiert wird. C_3 hat weniger bits pro pixel (1 vs 24) und nur 1 Farbebene, das erkennt man im Infoheader. Durch die Breite und Höhe kann der Computer errechnen wie das Bild größer gescalt werden muss??