#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	printf(1, "hi %d\n", signal(0, 0));
	exit();	
}
