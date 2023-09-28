//
// Created by jinhai on 23-9-27.
//

#pragma once

#include "storage/indexstore/knn_distance.h"

#include "storage/indexstore/knn_flat/alias.h"

namespace infinity {

template<typename DistType>
class KnnFlatIPBlas final : public KnnDistance<DistType> {

    using HeapResultHandler = NewHeapResultHandler<faiss::CMin<DistType, CompoundID>>;

public:
    explicit
    KnnFlatIPBlas(const DistType* queries,
              i64 query_count,
              i64 topk,
              i64 dimension,
              EmbeddingDataType elem_data_type)
            : KnnDistance<DistType>(KnnDistanceAlgoType::kKnnFlatIpBlas, elem_data_type),
              queries_(queries),
              query_count_(query_count),
              dimension_(dimension),
              top_k_(topk) {
        id_array_ = MakeUnique<Vector<CompoundID>>(topk * query_count_, CompoundID(-1, -1));
        distance_array_ = MakeUnique<DistType[]>(sizeof(DistType) * topk * query_count_);

        heap_result_handler_ = MakeUnique<HeapResultHandler>(query_count, distance_array_.get(), id_array_->data(), topk);
    }

    void
    Begin() final;

    void
    Search(const DistType* base,
           i64 base_count,
           i32 segment_id) final;

    void
    End() final;

    [[nodiscard]] inline DistType*
    GetDistances() const final {
        return heap_result_handler_->heap_dis_tab;
    }

    [[nodiscard]] inline CompoundID*
    GetIDs() const final {
        return heap_result_handler_->heap_ids_tab;
    }

    [[nodiscard]] inline DistType*
    GetDistanceByIdx(i64 idx) const final {
        if(idx >= query_count_) {
            ExecutorError("Query index exceeds the limit")
        }
        return heap_result_handler_->heap_dis_tab + idx * top_k_;
    }

    [[nodiscard]] inline CompoundID*
    GetIDByIdx(i64 idx) const final {
        if(idx >= query_count_) {
            ExecutorError("Query index exceeds the limit")
        }
        return heap_result_handler_->heap_ids_tab + idx * top_k_;
    }

private:
    UniquePtr<Vector<CompoundID>> id_array_{};
    UniquePtr<DistType[]> distance_array_{};

    UniquePtr<HeapResultHandler> heap_result_handler_{};
    const DistType* queries_{};
    i64 query_count_{};
    i64 dimension_{};
    i64 top_k_{};
    bool begin_{false};
};

template
class KnnFlatIPBlas<f32>;

}