#include "re.h"

#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>

#include "error.h"

bool matches_regex(char *str, char *exp) {
    regex_t re;
    int res;

    if (regcomp(&re, exp, REG_EXTENDED|REG_ICASE|REG_NOSUB|REG_NEWLINE)) {
        die("Could not compile search regex.");
    }

    res = regexec(&re, str, 0, NULL, 0);
    regfree(&re);

    switch (res) {
        case 0:
            return true;
        case REG_NOMATCH:
            return false;
        default:
            die("Regex match failed.");
    }

    /* Never reached, but keeps -Wreturn-type happy */
    return false;
}
