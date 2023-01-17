//
// Created by JinHai on 2022/7/22.
//

#include "query_context.h"
#include "infinity.h"
#include "planner/logical_planner.h"
#include "planner/optimizer.h"
#include "executor/physical_planner.h"
#include "executor/physical_operator.h"
#include "common/utility/infinity_assert.h"
#include "scheduler/operator_pipeline.h"

#include "SQLParser.h"
#include "SQLParserResult.h"

#include <sstream>
#include <utility>

namespace infinity {

class Pipeline;

std::string
QueryResult::ToString() const {
    std::stringstream ss;

    switch (root_operator_type_) {
        case LogicalNodeType::kInsert: {
            return "INSERT 0 1";
        }
        case LogicalNodeType::kUpdate: {
            return "UPDATE 0 1";
        }
        case LogicalNodeType::kDelete: {
            return "DELETE 0 1";
        }
        default: {
            ss << std::endl;
        }
    }


    SizeT column_count = result_->ColumnCount();
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        String end;
        if(idx != column_count - 1) {
            end = " ";
        }
        ss << result_->GetColumnNameById(idx) << end;
    }
    ss << std::endl;

    // Get Block count
    SizeT block_count = result_->DataBlockCount();

    // Iterate all blocks
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        // Get current block
        SharedPtr<DataBlock> current_block = result_->GetDataBlockById(idx);

        ss << current_block->ToString();
    }

    return ss.str();
}

QueryContext::QueryContext(SharedPtr<Session> session_ptr, UniquePtr<TransactionContext>& transaction)
    : session_ptr_(std::move(session_ptr)), transaction_(transaction) {}

QueryResult
QueryContext::Query(const std::string &query) {
    hsql::SQLParserResult parse_result;

    // Parse sql
    hsql::SQLParser::parse(query, &parse_result);
    if(!parse_result.isValid()) {
        ParserError(parse_result.errorMsg())
    }

    SharedPtr<QueryContext> query_context = shared_from_this();

    LogicalPlanner logical_planner(query_context);
    Optimizer optimizer(query_context);
    PhysicalPlanner physical_planner(query_context);

    PlannerAssert(parse_result.getStatements().size() == 1, "Not support more statements");
    for (hsql::SQLStatement *statement : parse_result.getStatements()) {
        // Build unoptimized logical plan for each SQL statement.
        logical_planner.Build(*statement);
        SharedPtr<LogicalNode> unoptimized_plan = logical_planner.LogicalPlan();

        // Apply optimized rule to the logical plan
        SharedPtr<LogicalNode> optimized_plan = optimizer.optimize(unoptimized_plan);

        // Build physical plan
        SharedPtr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

        // Create execution pipeline
        SharedPtr<Pipeline> pipeline = OperatorPipeline::Create(physical_plan);

        // Schedule the query pipeline
        Infinity::instance().scheduler()->Schedule(query_context, pipeline);

        QueryResult query_result;
        query_result.result_ = pipeline->GetResult();
        query_result.root_operator_type_ = unoptimized_plan->operator_type();

        return query_result;
//        ResponseError(optimized_plan->ToString(0));
    }

    NetworkError("Can't reach here.")
//    return QueryResult();
}

}
