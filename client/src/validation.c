// #include "validation.h"
// #include <ctype.h>
// #include <string.h>
// #include <regex.h>

// bool validate_username(const char *username)
// {
//     regex_t regex;
//     // Тільки латинські літери, апостроф і дефіс
//     int ret = regcomp(&regex, "^[A-Za-z'-]+$", REG_EXTENDED);
//     if (ret)
//         return false;

//     ret = regexec(&regex, username, 0, NULL, 0);
//     regfree(&regex);
//     return ret == 0;
// }

// bool validate_email(const char *email)
// {
//     regex_t regex;
//     // Простий email-патерн (мінімум перевірки: символ @)
//     int ret = regcomp(&regex, "^[a-z0-9._%+-]+@[a-z0-9.-]+\\.[a-z]{2,}$", REG_EXTENDED | REG_ICASE);
//     if (ret)
//         return false;

//     ret = regexec(&regex, email, 0, NULL, 0);
//     regfree(&regex);
//     return ret == 0;
// }

// bool validate_password(const char *password)
// {
//     int len = strlen(password);
//     if (len < 6 || len > 20)
//         return false;

//     bool has_upper = false, has_lower = false, has_digit = false, has_special = false;
//     for (int i = 0; i < len; ++i)
//     {
//         if (isupper(password[i]))
//             has_upper = true;
//         else if (islower(password[i]))
//             has_lower = true;
//         else if (isdigit(password[i]))
//             has_digit = true;
//         else if (ispunct(password[i]))
//             has_special = true;
//         else
//             return false; // заборона нелатинських символів
//     }

//     return has_upper && has_lower && has_digit && has_special;
// }
