#pragma once

namespace asa::structures
{
	class BaseStructure
	{
	public:
		BaseStructure(std::string name) : name(name){};

		const std::string name;
	};

}