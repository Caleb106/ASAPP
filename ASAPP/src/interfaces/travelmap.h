#pragma once
#include "basetravelmap.h"
#include "components/searchbar.h"

namespace asa::interfaces
{
	class TravelMap : public BaseTravelMap
	{
		using BaseTravelMap::BaseTravelMap;

	private:
		components::SearchBar searchbar{ 301, 940, 245, 51 };

	public:
		void Close();
		virtual void GoTo(std::string destination);
	};
}