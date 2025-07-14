# ifndef SIZE_HPP
# define SIZE_HPP

class Size
{
    private:
        unsigned long  _size;
    public:
        Size();
	    Size(unsigned long size);
        Size &operator=(const Size &other);
};

# endif