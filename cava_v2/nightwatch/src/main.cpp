#include "parser/parse.h"

int main(int argc, const char *argv[]) {
  int result;
  ava::cava::process(argc, argv, &result);
  return result;
}
