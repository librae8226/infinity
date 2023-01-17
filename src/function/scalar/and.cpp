//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"

namespace infinity {

struct AndFunction {
    template<typename TA, typename TB, typename TC>
    static inline void
    Run(TA left, TB right, TC& result) {
        result = left and right;
    }
};

static void
GenerateAndFunction(SharedPtr<ScalarFunctionSet>& function_set_ptr) {
    String func_name = "AND";
    ScalarFunction and_function(
            func_name,
            { DataType(LogicalType::kBoolean), DataType(LogicalType::kBoolean) },
            { DataType(LogicalType::kBoolean) },
            &ScalarFunction::BinaryFunction<BooleanT, BooleanT, BooleanT, AndFunction>);
    function_set_ptr->AddFunction(and_function);
}

void
RegisterAndFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "AND";
    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateAndFunction(function_set_ptr);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}