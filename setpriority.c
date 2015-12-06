#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int setpriority(int);
  
int
main(int argc, char *argv[])
{
  if (argc!=3) {
    printf(2, "Usage: setpriority pid priority.\n");
    exit();
  }
  int priority = atoi(argv[1]);
  setpriority(int);
  exit();
}
