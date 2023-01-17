//
// Created by JinHai on 2022/9/12.
//

#pragma once

namespace infinity {

enum class TableRefType {
    kInvalid,
    kBaseTable,
    kSubquery,
    kCrossProduct,
    kJoin,
    kDummy,
};

class TableRef {
public:
    explicit
    TableRef(TableRefType type, String alias): type_(type), alias_(std::move(alias)) {}

    TableRefType type_{TableRefType::kInvalid};
    String alias_{};
};

}
