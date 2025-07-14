#include "../../includes/Size.hpp"


Size::Size()
{
    _size = 0;
}

Size::Size( size_t size )
{
    _size = size;
}

Size &Size::operator=(const Size &other)
{
    _size = other._size;
    return (*this);
}