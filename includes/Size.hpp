# ifndef SIZE_HPP
# define SIZE_HPP

#include <cstdlib>
#include <iostream>

class Size
{
    private:
        ssize_t  _size;
    public:
        Size();
	    Size(ssize_t size);
        Size &operator=(const Size &other);
        ssize_t getSize() const;
};

# endif