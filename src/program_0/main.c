#include <scwrapper.h>

int 
main(int argc, char* argv[])
{
 prints("Starting process 0\n");
 /* Try to create and run process 1. */
 if (0 != createprocess(1))
 {
  prints("createprocess failed.\n");
 }
 prints("Ending process 0\n");
 return 0;
}
