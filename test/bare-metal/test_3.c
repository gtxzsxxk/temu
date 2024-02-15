//
// Created by hanyuan on 2023/10/29.
//

#define NUM 20

int main() {
    int n, m;
    int src[NUM];
    int stack[NUM];
    int stacked_car = 1;
    int stack_ptr = -1;
    int output_bitmap = 0;
    n = 5;
    m = 2;
    src[0] = 1;
    src[1] = 2;
    src[2] = 3;
    src[3] = 5;
    src[4] = 4;
    for (int i = 0; i < n; i++) {
        int dest = src[i];
        for (int j = stacked_car; j <= dest; j++) {
            stack[++stack_ptr] = j;
            stacked_car = j + 1;
            output_bitmap |= 1;
            output_bitmap <<= 1;
            if (stack_ptr >= m) {
                return -1;
            }
        }
        int pop = stack[stack_ptr--];
        if (pop != dest) {
            return -1;
        }
        output_bitmap |= 0;
        output_bitmap <<= 1;
    }
    return output_bitmap;
}