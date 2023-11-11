// const char*
// 不是sysy标准中定义的类型，sysy也没有extern这个声明外部函数的关键字,
// 另外sysy也没有... 这个不定长参数的语法
// 所以这个测试例基于一个sysy拓展的实现链接到glibc或者是其他c库实现打印
extern int printf(const char *fmt, ...);

void print(int num) { printf("%d", num); }

int main() {
  int num = 21051244;
  print(num);
  return 0;
}
