#include <_mingw.h>
#if defined(_WIN32) && defined(__MINGW64_VERSION_MAJOR) && __MINGW64_VERSION_MAJOR < 3

#include <string.h>

/*
 * strtok_r code directly from glibc.git /string/strtok_r.c since windows
 * doesn't have it.
 */
char *strtok_r(char *s, const char *delim, char **save_ptr)
{
    char *token;

    if(s == NULL)
        s = *save_ptr;

    /* Scan leading delimiters.  */
    s += strspn(s, delim);
    if(*s == '\0')
    {
        *save_ptr = s;
        return NULL;
    }

    /* Find the end of the token.  */
    token = s;
    s = strpbrk(token, delim);

    if(s == NULL)
    {
        /* This token finishes the string.  */
        *save_ptr = strchr(token, '\0');
    }
    else
    {
        /* Terminate the token and make *SAVE_PTR point past it.  */
        *s = '\0';
        *save_ptr = s + 1;
    }

    return token;
}

#endif
