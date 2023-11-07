int main() {
  int x = 2;
  int y = 1;
  int z = 3;
  int t;
  if (x > y) {
    t = x;
    x = y;
    y = t;
  }
  if (x > z) {
    t = z;
    z = x;
    x = t;
  }
  if (y > z) {
    t = y;
    y = z;
    z = t;
  }
  return 0;
}
