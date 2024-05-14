#include <iostream>


enum method
{
	GET,
	POST,
	DELETE,
	OTHER
};

class Request
{
	public:
		Request() {}
		~Request() {}
		static method GetType()
		{
			return (GET);
		}
};

int main()
{
	method type = Request::GetType();
	if (type == GET)
		std::cout << "type is get" << std::endl;
	return (0);
}