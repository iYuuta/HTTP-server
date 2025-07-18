# include "../includes/Config.hpp"

int tempserver(void);
int main ()
{
    Config conf("configs/test.conf");
    tempserver();
}