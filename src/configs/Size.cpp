#include "../../includes/Size.hpp"

Size::Size()
{
	_size = -1;
}

Size::Size(ssize_t size)
{
	_size = size;
}

Size& Size::operator=(const Size& other)
{
	if (&other != this)
		_size = other._size;
	return (*this);
}

ssize_t Size::getSize() const {
	return _size;
}
