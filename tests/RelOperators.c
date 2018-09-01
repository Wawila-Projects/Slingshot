/*******************************************
 * File: RelOperatos.py
 * A test program for relational operators
 *******************************************/

void main() {
    int x, y;
    
    srand(time(0));

    x = rand() % 100 + 1;
    y = rand() % 100 + 1;

    printf("x = %d\n", x);
    printf("y = %d\n", y);

    // Greather than
    printf ("x > y: %d\n", x > y);

    // Less than
    printf("x < y: %d\n", x < y);

    // Greather or Equal
    printf("x >= y: %d\n", x >= y);

    // Less or Equal
    printf("x <= y: %d\n", x <= y);

    // Not Equal
    printf("x != y: %d\n", x != y);

    // Equal
    printf("x == y: %d\n", x == y);
}
