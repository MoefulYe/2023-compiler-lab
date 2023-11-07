#include <assert.h>
int a = 3;

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
