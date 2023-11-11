#include <assert.h>
int a = 3;
// sysy中并没有定义assert的实现, 这里的assert只是说明不同作用域中下的`a`是不同的

int main() {
  // 作用域
  int a = 2;
  {
    int a = 1;
    {
      int a = 0;
      assert(a == 0);
    }
    assert(a == 1);
  }
  assert(a == 2);
  return 0;
}
