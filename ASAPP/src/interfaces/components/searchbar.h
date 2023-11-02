#pragma once
#include "iasainterfacecomponent.h"
#include <string>

namespace asa
{
	struct ASASearchBar : IASAInterfaceComponent
	{
		using IASAInterfaceComponent::IASAInterfaceComponent;

		void Press() const;
		void SearchFor(std::string term);
		void DeleteSearch();

		void SetTextCleared() { this->isTextEntered = false; }
		bool GetLastSearchedTerm(std::string& termOut) const { return 1; };
		bool IsSearching() const { return this->isSearching; }
		bool TextIsEntered() const { return this->isTextEntered; }

	private:
		std::string lastSearchedTerm = "";
		bool isSearching{ false };
		bool isTextEntered{ false };
	};


}