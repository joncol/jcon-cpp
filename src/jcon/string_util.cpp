#include "string_util.h"

#include <QString>

namespace jcon {

QString variantListToString(const QVariantList& l, const QString& sep)
{
    QStringList str_list;
    for (auto e : l) {
        if (e.type() == QVariant::List) {
            str_list.push_back(QString("list (%1 elements)")
                               .arg(e.toList().size()));
        } else if (e.type() == QVariant::Map) {
            str_list.push_back(QString("map (%1 elements)")
                               .arg(e.toMap().size()));
        } else if (e.canConvert<QString>()) {
            str_list.push_back(e.toString());
        } else {
            str_list.push_back("N/A");
        }
    }
    return str_list.join(sep);
}

}
