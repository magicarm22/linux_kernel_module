#include "libmylib.h"

char    *ft_strnew(size_t size)
{
    char    *arr;

    arr = (char *)malloc(size * sizeof(*arr) + 1);
    if (arr == NULL)
        return (NULL);
    bzero((void *)arr, size * sizeof(*arr) + 1);
    return (arr);
}

static int  ft_number_size(int n)
{
    int count;

    count = 0;
    if (n == 0)
        return (1);
    while (n != 0)
        n = n / 10 + count++ * 0;
    return (count);
}

char        *ft_itoa(int n)
{
    int     size;
    int     copy_n;
    char    *s;

    copy_n = n;
    size = n < 0 ? ft_number_size(n) + 1 : ft_number_size(n);
    s = ft_strnew(size);
    if (s == NULL)
        return (NULL);
    while (size--)
    {
        s[size] = n % 10 < 0 ? -(n % 10) + 48 : n % 10 + 48;
        n /= 10;
    }
    s[size + 1] = s[size + 1] == '0' && copy_n != 0 ? '-' : s[size + 1];
    return (s);
}