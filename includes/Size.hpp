# ifndef SIZE_HPP
# define SIZE_HPP

# include <iostream>

class Size
{
    private:
        size_t  _size;
    public:
        Size( void );
        Size( size_t size );
        Size &operator=(const Size &other);
};

# endif