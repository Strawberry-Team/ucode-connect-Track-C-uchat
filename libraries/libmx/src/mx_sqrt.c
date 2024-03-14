int mx_sqrt(int x) {
    if (x <= 0)
        return 0;

    int left = 1;
    int right = x;

    while (left <= right) {
        int mid = (right + left) / 2;
        long long square = (long long) mid * mid;

        if (square == x)
            return mid;
        else if (square < x)
            left = mid + 1;
        else
            right = mid - 1;
    }

    return 0;
}
