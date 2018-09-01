int a, b;

int gcd(int x, int y);

void main() {
    int x, y, z ;

    a = 28;
    b = 21;

    
    x = a;
    y = b;
    z = gcd(x, y);

    printf("GCD(%d, %d) = %d\n", a, b, z);
}

// Funcion que calcula el maximo comun divisor
int gcd(int x, int y) {
    if (y == 0) { return(x); }
    else { return( gcd(y, x % y) ); }
}