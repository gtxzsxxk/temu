#include <math.h>

#define MAX 20  //最大计算阶数，可以更改

//两个函数的声明
int DET(int arr1[MAX][MAX], int n);

int Minor(int arr1[MAX][MAX], int i, int n);

int DET(int arr1[MAX][MAX], int n) {
    int i, M, sum = 0;//i是第一行的列指标，M是余子式的值，sum是行列式的计算值
    if (n == 1)//一阶行列式直接得出结果
        return arr1[0][0];
    else if (n > 1) {
        for (i = 0; i < n; i++)//按第一行展开
        {
            M = Minor(arr1, i, n);
            sum += (int) pow((double) -1, (double) (i + 2)) * arr1[0][i] * M;
        }
    }
    return sum;
}


int Minor(int arr1[MAX][MAX], int i, int n) {
    int j, k, result;
    int arr2[MAX][MAX];

    //以下为构造余子式的过程。由于C语言的特性，这个过程会比较复杂，需要观察余子式与原行列式的关系。
    for (j = 0; j < n - 1; j++) {
        for (k = 0; k < n - 1; k++) {
            if (k < i)
                arr2[j][k] = arr1[j + 1][k];
            else if (k >= i)
                arr2[j][k] = arr1[j + 1][k + 1];
        }
    }

    return DET(arr2, n - 1);//构造完后，余子式是一个新的行列式，返回DET函数进行计算。
}


int main() {
    int arr1[MAX][MAX] = {
            {1, 1, 4},
            {5, 1, 4},
            {9, 8, 5}
    };


    return DET(arr1, 3);
}
