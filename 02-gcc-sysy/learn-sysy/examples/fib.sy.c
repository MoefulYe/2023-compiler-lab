// 定义常量
const int I_DONT_KNOW = -1;
// 定义函数
int fib(int val) {
  // 分支语句
  if (val == 0) {
    return 0;
  } else if (val == 1) {
    return 1;
  } else {
    if (val == 2) {
      return 1;
    } else {
      if (val == 3) {
        return 2;
      } else {
        return I_DONT_KNOW;
      }
    }
  }
}
