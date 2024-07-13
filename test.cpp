#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>


#include <cstdio>

int main(int ac, char **av)
{
	if ( 1 == 0 && printf("hello\n"))
		return (1);
	return (0);
}