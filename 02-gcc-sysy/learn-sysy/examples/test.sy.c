void print(int a, int b, int c) {}

int main() {
  int i = 0;
  int j = 0;
  // sysy标准中没有定义for 控制流语句,
  for (i = 1; i <= 9; i++) {
    for (j = 1; j <= i; j++) {
      print(i, j, i * j);
    }
  }
}
