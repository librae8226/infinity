//
// Created by JinHai on 2022/12/16.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "function/cast/bool_cast.h"

class BoolCastTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(BoolCastTest, bool_cast0) {
    using namespace infinity;

    // BooleanT to BooleanT, throw exception
    {
        BooleanT source = true;
        BooleanT target;
        EXPECT_THROW(TryCastBoolean::Run(source, target), FunctionException);
    }
    // BooleanT to VarcharT
    {
        BooleanT source = true;
        VarcharT target;
        EXPECT_TRUE(TryCastBoolean::Run(source, target));
        String result = String(target.prefix, 4);
        EXPECT_STREQ("true", result.c_str());

        source = false;
        EXPECT_TRUE(TryCastBoolean::Run(source, target));
        result = String(target.prefix, 5);
        EXPECT_STREQ("false", result.c_str());
    }
}

TEST_F(BoolCastTest, bool_cast1) {
    using namespace infinity;

    SharedPtr<DataType> bool_type = MakeShared<DataType>(LogicalType::kBoolean);

    SharedPtr<ColumnVector> col_bool = MakeShared<ColumnVector>(bool_type);
    col_bool->Initialize();

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeBool(i % 2 == 0);
        col_bool->AppendValue(v);
        Value vx = col_bool->GetValue(i);
    }
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_bool->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, i % 2 == 0);
    }

    // cast bool column vector to varchar column vector
    {
        SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto bool2varchar_ptr = BindBoolCast(*bool_type, *varchar_type);
        EXPECT_NE(bool2varchar_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(varchar_type);
        col_varchar_ptr->Initialize();

        CastParameters cast_parameters;
        bool result = bool2varchar_ptr.function(col_bool, col_varchar_ptr, DEFAULT_VECTOR_SIZE, cast_parameters);
        EXPECT_TRUE(result);
        for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
            Value vx = col_varchar_ptr->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            String res = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            if(i % 2 == 0) {
                EXPECT_STREQ(res.c_str(), "true");
            } else {
                EXPECT_STREQ(res.c_str(), "false");
            }
        }
    }

    {
        DataType source(LogicalType::kBoolean);
        DataType target(LogicalType::kVarchar);
        auto func_ptr = BindBoolCast(source, target);
        EXPECT_NE(func_ptr.function, nullptr);
    }

    {
        DataType source(LogicalType::kBoolean);
        DataType target(LogicalType::kBoolean);
        EXPECT_THROW(BindBoolCast(source, target), TypeException);
    }

}