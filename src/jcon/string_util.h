#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include "jcon.h"

#include <QVariantList>

class QString;

namespace jcon {

/**
 * Convert a QVariantList to a QString
 *
 * @param[in] l   The QVariantList to convert
 * @param[in] sep The separator to insert between element strings
 *
 * @returns A string with the string representations of each element in the list
 *          joined by the separator.
 */
JCON_API QString variantListToString(const QVariantList& l,
                                     const QString& sep = ", ");

}

#endif
