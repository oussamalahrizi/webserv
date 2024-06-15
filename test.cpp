#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

void ft_putchar(char c)
{
	write(1, &c, 1);
}

void ft_putnbr(int x)
{
	long n = x;
	if (n < 0)
	{
		ft_putchar('-');
		n = -n;
	}
	if (n <= 9)
	{
		ft_putchar(n + 48);
	}
	else
	{
		ft_putnbr(n / 10);
		ft_putnbr(n % 10);
	}
}

int main(int ac, char **av)
{
	ft_putnbr(INT32_MIN);
	ft_putchar('\n');
	return (0);
}