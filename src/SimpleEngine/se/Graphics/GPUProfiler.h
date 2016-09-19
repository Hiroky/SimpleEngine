#pragma once

#include "se/Common.h"
#include "se/Graphics/GraphicsContext.h"
#include "se/Graphics/GPUBuffer.h"
#include <vector>
#include <stack>
#include <queue>


// プロファイルスコープ
#define seGpuPerfScope(context, id, name)	se::GpuProfilePoint perf##id(context, name)


namespace se
{
	
	/**
	 * GPUプロファイラ
	 */
	class GPUProfiler
	{
	private:
		struct PendingFrameQueries
		{
			int disjointQueryId_;
			int beginQuery_;
			int endQuery_;
		};

	public:
		struct ProfilerTreeMember
		{
			std::vector<ProfilerTreeMember*> childMembers_;
			ProfilerTreeMember* parent_;
			const char* name_;
			double time_;

			ProfilerTreeMember()
			{
				parent_ = nullptr;
				name_ = "invalid";
				time_ = 0.0;
			}

			~ProfilerTreeMember()
			{
				for (auto* c : childMembers_) {
					delete c;
				}
			}
		};

	private:
		const int MAX_HW_QUERIES = 1024 * 64;
		const int MAX_HW_DISJOINT_QUERIES = 8;
		std::vector<Query> hwQueries_;
		std::vector<const char*> hwQueriesDescs_;
		std::vector<Query> disjointQueries_;
		std::vector<int> correspondingQueryEnds_;
		std::stack<int> queriesStack_;
		int currentDisjointQuery_;
		int currentQuery_;
		int currentFrameFirstQuery_;

		ProfilerTreeMember* currentFrameProfilerTree_;
		std::queue<PendingFrameQueries> pendingFrames_;

	private:
		void IncrementCurrentQuery();
		void IncrementCurrentDisjointQuery();

	public:
		static GPUProfiler& Get() {
			static GPUProfiler instance;
			return instance;
		}

	public:
		GPUProfiler();

		void Initialize();
		void Finalize();

		void BeginProfilePoint(GraphicsContext& context, const char* profilePointName);
		void EndProfilePoint(GraphicsContext& context);
		void BeginFrameProfiling(GraphicsContext& context);
		void EndFrameProfiling(GraphicsContext& context);

		const ProfilerTreeMember* GetProfilerTreeRootNode() const { return currentFrameProfilerTree_; }
	};


	/**
	 * プロファイルポイント
	 */
	class GpuProfilePoint
	{
	private:
		GraphicsContext& context_;

	public:
		GpuProfilePoint(GraphicsContext& context, const char* name)
			:context_(context)
		{
			GPUProfiler::Get().BeginProfilePoint(context_, name);
		}

		~GpuProfilePoint()
		{
			GPUProfiler::Get().EndProfilePoint(context_);
		}
	};

}
