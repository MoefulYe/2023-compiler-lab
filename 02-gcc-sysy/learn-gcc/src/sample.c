#ifdef NEG
#define M -4
#else
#define M 4
#endif /* ifdef NEG */

int main(int argc, char *argv[]) {
  int a = M;
  if (a)
    a = a + 4;
  else
    a = a * 4;
  return 0;
}
