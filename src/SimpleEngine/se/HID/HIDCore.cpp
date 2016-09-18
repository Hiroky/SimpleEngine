#include "se/HID/Mouse.h"
#include "HIDCore.h"


namespace se 
{

	Mouse HIDCore::mouse_;

	void HIDCore::Initialize()
	{
	}

	void HIDCore::Update()
	{
		mouse_.Update();
	}

}