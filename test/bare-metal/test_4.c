int arr[6];

int main() {
    int max = 999;
    arr[0] = 1;
    arr[1] = 9;
    arr[2] = 2;
    arr[3] = 6;
    arr[4] = 1;
    arr[5] = 8;
    for (int i = 0; i < 6; i++) {
        if (arr[i] < max) {
            max = arr[i];
        }
    }
    return max;
}