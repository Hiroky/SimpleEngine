#pragma once

#include "se/Common.h"
#include "Atomic.h"


namespace se {

	static const uint32_t WAIT_INFINITE = INFINITE;

	/**
	 * クリティカルセクション
	 */
	class CriticalSection
	{
	private:
		CRITICAL_SECTION criticalSection_;

	public:
		CriticalSection()
		{
			InitializeCriticalSection(&criticalSection_);
			// スピンカウント設定(詳細はhttps://msdn.microsoft.com/ja-jp/library/cc429324.aspx)
			SetCriticalSectionSpinCount(&criticalSection_, 4000);
		}

		~CriticalSection()
		{
			DeleteCriticalSection(&criticalSection_);
		}


		void Lock()
		{
			// すでにスレッドがセクション保持している場合無駄なので一回Tryする
			if (TryEnterCriticalSection(&criticalSection_) == 0) {
				EnterCriticalSection(&criticalSection_);
			}
		}

		void Unlock()
		{
			LeaveCriticalSection(&criticalSection_);
		}
	};


	/**
	 * スピンロック
	 */
	class SpinLock
	{
	private:
		enum {
			UnLocked,
			Locked,
		};

		int32_t lock_;

	public:
		SpinLock()
			: lock_(0)
		{
		}

		void Lock()
		{
			while (Atomic::InterlockedCompareExchange(&lock_, UnLocked, Locked));
		}

		void Unlock()
		{
			Atomic::InterlockedExchange(&lock_, UnLocked);
		}
	};


	/**
	 * スコープロック
	 */
	template<class T>
	class ScopedLock
	{
	private:
		T* lock_;

	private:
		ScopedLock(const ScopedLock&) {}

	public:
		ScopedLock(T& lock)
			: lock_(&lock_)
		{
			lock_->Lock();
		}

		~ScopedLock()
		{
			lock_->Unlock();
		}
	};


	/**
	 * イベント
	 */
	class Event
	{
	private:
		HANDLE handle_;
		bool manualReset_;

	public:
		Event()
			: handle_(nullptr)
			, manualReset_(false)
		{
		}

		~Event()
		{
			if (handle_) {
				CloseHandle(handle_);
				handle_ = nullptr;
			}
		}

		bool IsManualReset() { return manualReset_; }

		void Create(bool isManualReset = false)
		{
			handle_ = CreateEvent(nullptr, isManualReset, 0, nullptr);
			manualReset_ = isManualReset;
		}

		// イベント発火
		void Trigger()
		{
			SetEvent(handle_);
		}

		void Reset()
		{
			ResetEvent(handle_);
		}

		// 正常にシグナルイベントで抜けたかを返す
		bool Wait(uint32_t waitTime = WAIT_INFINITE)
		{
			return WaitForSingleObject(handle_, waitTime) == WAIT_OBJECT_0;
		}
	};


	/**
	 * スコープによるイベント待ち
	 */
	class ScopedEvent
	{
	private:
		Event* event_;

	private:
		ScopedEvent(const ScopedEvent&) {}

	public:
		ScopedEvent(Event& event)
			: event_(&event)
		{
		}

		~ScopedEvent()
		{
			event_->Wait();
			event_ = nullptr;
		}
	};


	/**
	 * スレッド上で処理を走らせるオブジェクト
	 */
	class ThreadRunnable
	{
	public:
		virtual ~ThreadRunnable() {};

		virtual uint32_t Run() = 0;

		virtual bool Initialize() { return true; }
		virtual void Stop() {}
		virtual void Exit() {}
	};


	/**
	 * スレッド
	 */
	class Thread
	{
	private:
		HANDLE handle_;
		uint32_t id_;
		ThreadRunnable* runner_;

	private:
		// スレッドのエントリポイント
		static DWORD __stdcall ThreadProc(void* data)
		{
			return reinterpret_cast<Thread*>(data)->Run();
		}

	public:
		Thread()
			: handle_(nullptr)
			, id_(0)
			, runner_(nullptr)
		{
		}

		~Thread()
		{
			if (handle_) {
				Kill(true);
			}
		}


		void Create(ThreadRunnable* runner, const char* name = nullptr, uint32_t stackSize = 0)
		{
			runner_ = runner;
			handle_ = ::CreateThread(NULL, stackSize, ThreadProc, this, STACK_SIZE_PARAM_IS_A_RESERVATION, (LPDWORD)&id_);
		}

		void Wait()
		{
			WaitForSingleObject(handle_, WAIT_INFINITE);
		}

		void Suspend(bool pause = true)
		{
			if (pause) {
				::SuspendThread(handle_);
			}
			else {
				::ResumeThread(handle_);
			}
		}

		bool Kill(bool wait = false)
		{
			// 実行中のタスクを止める
			if (runner_) {
				runner_->Stop();
			}

			// 終了を待つ
			if (wait) {
				Wait();
			}

			// クローズ
			CloseHandle(handle_);
			handle_ = nullptr;
			return true;
		}

		uint32_t Run()
		{
			uint32_t exitCode = 0;

			if (runner_->Initialize()) {
				exitCode = runner_->Run();
				runner_->Exit();
			}

			return exitCode;
		}
	};

}