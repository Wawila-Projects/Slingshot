/*
 * WhileTest.c
 * Sample program for testing while statement
 */

 void main() {
     int i;
     int local [10];
     int count;

     
     srand(time(0));

     local[0] = 75;
     local[1] = 30;
     local[2] = 420;
     local[3] = rand();
     local[4] = rand();
     local[5] = rand();
     local[6] = rand();
     local[7] = rand();
     local[8] = 2;
     local[9] = 45;


    count = sizeof(local) / sizeof(int);

     printf("count: %d\n", count);

    for(i = 0; i < 10; i++) {
        count += local[i];
        printf("Local[%d] = %d\n", i, local[i]);
    }

    printf("Count: %d Average: %d", count, count/10);
}
