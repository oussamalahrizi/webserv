#include <iostream>
#include <fcntl.h>
#include <unistd.h>


int main(int ac, char **av)
{
	int fd = open(av[1], O_RDONLY);
	if (fd < 0)
	{
		std::cout << "cant open the file" << std::endl;
		return (1);
	}
	std::cout << "file opened" << std::endl;
	close(fd);
	return (0);
}