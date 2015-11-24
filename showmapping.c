#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int showmapping(long int, long int);
  
int
main(int argc, char *argv[])
{
  printf(2, "Running showmapping...\n");
  if (argc!=3) {
    printf(2, "Showmapping needs 3 arguments.\n");
    exit();
  }
  long int low = atoi16(argv[1]);
  long int high = atoi16(argv[2]);
  showmapping(low, high);
  exit();
}
