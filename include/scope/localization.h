#ifndef SCOPE_LOCALIZATION_H_
#define SCOPE_LOCALIZATION_H_

#include <libintl.h>
#include <string>

inline char * _(const char *__msgid) {
    return dgettext(GETTEXT_PACKAGE, __msgid);
}

inline std::string _(const char *__msgid1, const char *__msgid2,
                     unsigned long int __n) {
    char buffer [256];
    if (snprintf ( buffer, 256, dngettext(GETTEXT_PACKAGE, __msgid1, __msgid2, __n), __n ) >= 0) {
        return buffer;
    } else {
        return std::string();
    }
}

#endif // SCOPE_LOCALIZATION_H_


