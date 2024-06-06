double mx_pow(double n, unsigned int pow) {
    if (pow == 0) {
        return 1;
    }
    double t = n;
    for (unsigned int i = 1; i < pow; i++) {
        n *= t;
    }
    return n;
}
