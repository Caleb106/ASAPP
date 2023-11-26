#pragma once
#include "components/button.h"
#include "iinterface.h"
#include <array>
#include <string>

namespace asa::interfaces
{
	class BaseTravelMap : public IInterface
	{
	protected:
		std::array<components::Button, 12> results;
		window::Rect dayTime{ 1570, 77, 272, 54 };
		components::Button confirmButton{ 1461, 931, 367, 60, 10 };

	public:
		BaseTravelMap()
		{
			for (int i = 0; i < results.max_size(); i++) {
				results[i] = components::Button(
					89, 195 + (i * 55), 459, 55, 10);
			}
		}

		virtual bool IsOpen();
		virtual bool CanConfirmTarget();
		virtual bool HasResult();
		virtual bool IsResultSelected(int index);

		virtual int CountResults();

		virtual void SelectResult(int index = 0);
		virtual void GoTo(const std::string& destination) = 0;
	};
}