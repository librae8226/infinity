//
// Created by JinHai on 2022/9/11.
//

#include "binding.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

SharedPtr<Binding>
Binding::MakeBinding(BindingType binding_type,
                     const String& name,
                     u64 table_index,
                     const Vector<DataType>& column_types,
                     const Vector<String>& column_names) {
    SharedPtr<Binding> binding = MakeShared<Binding>();
    binding->binding_type_ = binding_type;
    binding->table_name_ = name;
    binding->table_index_ = table_index;

    size_t column_count = column_names.size();
    PlannerAssert(column_count == column_types.size(), "Make binding error: column size isn't valid.");
    binding->column_types_ = column_types;
    binding->column_names_ = column_names;

    for(size_t idx = 0; idx < column_count; ++ idx) {
        binding->name2index_[column_names[idx]] = static_cast<i64>(idx);
    }
    return binding;
}

SharedPtr<Binding>
Binding::MakeBinding(BindingType binding_type,
                     const String& binding_alias,
                     u64 table_index,
                     SharedPtr<Table> table_ptr,
                     const Vector<DataType>& column_types,
                     const Vector<String>& column_names) {
    auto binding = MakeBinding(binding_type, binding_alias, table_index, column_types, column_names);
    binding->table_ptr_ = std::move(table_ptr);
//    binding->logical_node_ptr_ = std::move(logical_node_ptr);
//    binding->logical_node_id_ = logical_node_id;
    return binding;
}

}