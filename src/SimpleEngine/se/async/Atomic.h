#pragma once

#include "se/Common.h"

namespace se {

	/**
	 * アトミック操作
	 */
	class Atomic
	{
	public:

		static int32_t InterlockedIncrement(int32_t* value)
		{
			return (LONG)::InterlockedIncrement((LPLONG)value);
		}

		static int32_t InterlockedDecrement(int32_t* value)
		{
			return (LONG)::InterlockedDecrement((LPLONG)value);
		}

		static int32_t InterlockedAdd(int32_t* value, int32_t amount)
		{
			return (LONG)::InterlockedAdd((LPLONG)value, (LONG)amount);
		}

		static int32_t InterlockedExchange(int32_t* value, int32_t exchange)
		{
			return (LONG)::InterlockedExchange((LPLONG)value, (LONG)exchange);
		}

		static int32_t InterlockedCompareExchange(int32_t* value, int32_t compare, int32_t exchange)
		{
			return (LONG)::InterlockedCompareExchange((LPLONG)value, (LONG)exchange, (LONG)compare);
		}
	};

}