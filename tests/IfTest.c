/*******************************************
 * File: IfTest.c
 * A test program for if statement
 *******************************************/

void main() {
    int x, y;
    
    srand(time(0));
    
    x = rand() % 100 + 1;
    y = rand() % 100 + 1;

    printf("X = %d, Y = %d\n", x, y);

    if (x == y)
        printf ("X is equal to Y\n");
    else
        printf("X is not equal to Y\n");
}
