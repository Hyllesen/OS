#include <scwrapper.h>

int
main(int argc, char* argv[])
{
 prints("Starting process 1\n");
  /* Try to create and run process 2. */
 if (0 != createprocess(2))
 {
  prints("createprocess failed.\n");
 }
 prints("Ending process 1\n");
}
