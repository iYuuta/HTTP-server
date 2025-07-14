#include "../../includes/Size.hpp"


Size::Size()
{
	_size = 0;
}

Size::Size(unsigned long size)
{
	_size = size;
}

Size& Size::operator=(const Size& other)
{
	_size = other._size;
	return (*this);
}
