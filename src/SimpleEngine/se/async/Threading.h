#pragma once

#include "se/Common.h"
#include "Atomic.h"


namespace se {

	static const uint32_t WAIT_INFINITE = INFINITE;

	/**
	 * �N���e�B�J���Z�N�V����
	 */
	class CriticalSection
	{
	private:
		CRITICAL_SECTION criticalSection_;

	public:
		CriticalSection()
		{
			InitializeCriticalSection(&criticalSection_);
			// �X�s���J�E���g�ݒ�(�ڍׂ�https://msdn.microsoft.com/ja-jp/library/cc429324.aspx)
			SetCriticalSectionSpinCount(&criticalSection_, 4000);
		}

		~CriticalSection()
		{
			DeleteCriticalSection(&criticalSection_);
		}


		void Lock()
		{
			// ���łɃX���b�h���Z�N�V�����ێ����Ă���ꍇ���ʂȂ̂ň��Try����
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
	 * �X�s�����b�N
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
	 * �X�R�[�v���b�N
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
	 * �C�x���g
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

		// �C�x���g����
		void Trigger()
		{
			SetEvent(handle_);
		}

		void Reset()
		{
			ResetEvent(handle_);
		}

		// ����ɃV�O�i���C�x���g�Ŕ���������Ԃ�
		bool Wait(uint32_t waitTime = WAIT_INFINITE)
		{
			return WaitForSingleObject(handle_, waitTime) == WAIT_OBJECT_0;
		}
	};


	/**
	 * �X�R�[�v�ɂ��C�x���g�҂�
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
	 * �X���b�h��ŏ����𑖂点��I�u�W�F�N�g
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
	 * �X���b�h
	 */
	class Thread
	{
	private:
		HANDLE handle_;
		uint32_t id_;
		ThreadRunnable* runner_;

	private:
		// �X���b�h�̃G���g���|�C���g
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
			// ���s���̃^�X�N���~�߂�
			if (runner_) {
				runner_->Stop();
			}

			// �I����҂�
			if (wait) {
				Wait();
			}

			// �N���[�Y
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