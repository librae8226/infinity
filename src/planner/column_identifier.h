//
// Created by JinHai on 2022/9/23.
//

#pragma once

#include "sql/Expr.h"
#include "main/query_context.h"
#include "parser/expression/parsed_column_expression.h"

#include <string>

namespace infinity {

struct ColumnIdentifier {
public:
    static ColumnIdentifier
    MakeColumnIdentifier(SharedPtr<QueryContext>& query_context, const hsql::Expr &expr);

    static ColumnIdentifier
    MakeColumnIdentifier(SharedPtr<QueryContext>& query_context,
                         const SharedPtr<ParsedColumnExpression>& parsed_expr);

public:
    explicit
    ColumnIdentifier(SharedPtr<String> db_name,
                     SharedPtr<String> schema_name,
                     SharedPtr<String> table_name,
                     SharedPtr<String> column_name,
                     SharedPtr<String> alias_name);

    [[nodiscard]] String
    ToString() const;

    [[nodiscard]] bool
    operator==(const ColumnIdentifier& other) const;

    SharedPtr<String> db_name_ptr_;
    SharedPtr<String> schema_name_ptr_;
    SharedPtr<String> column_name_ptr_;
    SharedPtr<String> table_name_ptr_;
    SharedPtr<String> alias_name_ptr_;
};

}

