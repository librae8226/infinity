//
// Created by JinHai on 2022/8/12.
//

#include "common/utility/infinity_assert.h"
#include "insert_binder.h"

namespace infinity {

SharedPtr<BaseExpression>
InsertBinder::BuildExpression(const hsql::Expr &expr, const SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);
    return result;
}

//SharedPtr<BaseExpression>
//InsertBinder::BuildColRefExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) {
//    PlannerError("HavingBinder::BuildColRefExpr");
//}

}