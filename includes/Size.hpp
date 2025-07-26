# ifndef SIZE_HPP
# define SIZE_HPP

#include <cstdlib>

class Size
{
    private:
        size_t  _size;
    public:
        Size();
	    Size(size_t size);
        Size &operator=(const Size &other);
        size_t getSize() const;
};

# endif