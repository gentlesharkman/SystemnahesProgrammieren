#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    for (int i = 1; i < 3; i++) {
        fork();
    }
    /*Beim ersten mal wird 2 mal Hello! ausgegeben, das es den Prozess dann 2 mal gibt, einmal en Parent und einmal das geforckte child*/
    /*Beim zweiten mal wird es 4 mal ausgegeben, da wir durch 3 forks 3 Child prozesse erzeugen + parent = 4*/

    printf("Hello!\n");
    sleep(1);
}