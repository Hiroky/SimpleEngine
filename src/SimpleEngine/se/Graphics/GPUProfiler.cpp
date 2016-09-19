#include "se/Graphics/GPUProfiler.h"


namespace se
{

	GPUProfiler::GPUProfiler()
		: currentDisjointQuery_(0)
		, currentQuery_(0)
		, currentFrameFirstQuery_(0)
		, currentFrameProfilerTree_(nullptr)
	{
	}

	void GPUProfiler::Initialize()
	{
		hwQueries_.resize(MAX_HW_QUERIES);
		hwQueriesDescs_.resize(MAX_HW_QUERIES);
		correspondingQueryEnds_.resize(MAX_HW_QUERIES);
		for (int i = 0; i < MAX_HW_QUERIES; ++i) {
			hwQueries_[i].Create(Query::TIMESTAMP);
			hwQueriesDescs_[i] = "";
			correspondingQueryEnds_[i] = 0;
		}

		disjointQueries_.resize(MAX_HW_DISJOINT_QUERIES);
		for (int i = 0; i < MAX_HW_DISJOINT_QUERIES; ++i) {
			disjointQueries_[i].Create(Query::TIMESTAMP_DISJOINT);;
		}
	}

	void GPUProfiler::Finalize()
	{
		if (currentFrameProfilerTree_) {
			delete currentFrameProfilerTree_;
		}
	}

	void GPUProfiler::BeginProfilePoint(GraphicsContext& context, const char* profilePointName)
	{
		context.EndQuery(hwQueries_[currentQuery_]);
		hwQueriesDescs_[currentQuery_] = profilePointName;
		queriesStack_.push(currentQuery_);

		IncrementCurrentQuery();
	}

	void GPUProfiler::EndProfilePoint(GraphicsContext& context)
	{
		context.EndQuery(hwQueries_[currentQuery_]);
		int32_t beginQuery = queriesStack_.top();
		queriesStack_.pop();
		correspondingQueryEnds_[beginQuery] = currentQuery_;
		IncrementCurrentQuery();
	}

	void GPUProfiler::IncrementCurrentQuery()
	{
		correspondingQueryEnds_[currentQuery_] = INT_MAX;
		currentQuery_++;
		currentQuery_ %= MAX_HW_QUERIES;
	}

	void GPUProfiler::IncrementCurrentDisjointQuery()
	{
		currentDisjointQuery_++;
		currentDisjointQuery_ %= MAX_HW_DISJOINT_QUERIES;
	}

	void GPUProfiler::BeginFrameProfiling(GraphicsContext& context)
	{
		context.BeginQuery(disjointQueries_[currentDisjointQuery_]);
		currentFrameFirstQuery_ = currentQuery_;
		BeginProfilePoint(context, "WholeFrame");
	}

	void GPUProfiler::EndFrameProfiling(GraphicsContext& context)
	{
		PendingFrameQueries pendingFrame;
		pendingFrame.disjointQueryId_ = currentDisjointQuery_;
		pendingFrame.beginQuery_ = currentFrameFirstQuery_;
		pendingFrame.endQuery_ = currentQuery_;

		EndProfilePoint(context);

		if (queriesStack_.size() != 0) {
			throw "Wrong profile point count! Did you forget about EndProfilePoint?";
		}

		context.EndQuery(disjointQueries_[currentDisjointQuery_]);

		pendingFrames_.push(pendingFrame);

		IncrementCurrentDisjointQuery();

		if (currentFrameProfilerTree_) {
			delete currentFrameProfilerTree_;
		}

		// Time to fetch prev frames!
		if (pendingFrames_.size() > 4) {
			pendingFrame = pendingFrames_.front();
			pendingFrames_.pop();
			D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
			context.GetQueryData(disjointQueries_[pendingFrame.disjointQueryId_], &disjointData, sizeof(disjointData));
			ProfilerTreeMember* parent = new ProfilerTreeMember();
			ProfilerTreeMember* frameParent = parent;

			for (int queryIterator = pendingFrame.beginQuery_; queryIterator != (pendingFrame.endQuery_ + 1) % MAX_HW_QUERIES; queryIterator = (queryIterator + 1) % MAX_HW_QUERIES) {
				if (correspondingQueryEnds_[queryIterator] != INT_MAX) {
					auto* profilerObject = new ProfilerTreeMember();
					int correspondingEnd = correspondingQueryEnds_[queryIterator];
					uint64_t beginProfilePointData, endProfilePointData;
					context.GetQueryData(hwQueries_[queryIterator], &beginProfilePointData, sizeof(beginProfilePointData));
					context.GetQueryData(hwQueries_[correspondingEnd], &endProfilePointData, sizeof(endProfilePointData));

					profilerObject->time_ = (double)(endProfilePointData - beginProfilePointData) / (double)disjointData.Frequency * 1000.0;
					profilerObject->name_ = hwQueriesDescs_[queryIterator];
					profilerObject->parent_ = parent;

					parent->childMembers_.push_back(profilerObject);
					parent = profilerObject;
				} else {
					parent = parent->parent_;
					if (parent == nullptr) {
						throw "Error while constructing profiler tree";
					}
				}
			}
			if (frameParent->childMembers_.size() < 1) {
				throw "Error while constructing profiler tree";
			}

			currentFrameProfilerTree_ = frameParent->childMembers_[0];
		}
	}

}
