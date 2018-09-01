/*
 * WhileTest.c
 * Sample program for testing while statement
 */

void main() {
    int total;
    int count;
    
    srand(time(0));

    total = 0;
    count = 0;

    while (count < 3) {
        int number;
        
        number = rand() % 100 + 1;
        printf("Number [%d] = %d\n", count, number);
        count = count + 1;
        total = total + number;
    }
    printf ("The total is %d\n", total);
}
