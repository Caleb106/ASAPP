#include "searchbar.h"
#include "../../game/controls.h"
#include <Windows.h>

void asa::ASASearchBar::Press() const
{
	window::Point loc = this->area.GetRandLocation(2);

	window::SetMousePos(loc.x, loc.y);
	Sleep(30);
	controls::MousePress(controls::LEFT);
	Sleep(30);
}

void asa::ASASearchBar::SearchFor(std::string term)
{
	this->isSearching = true;
	if (!OpenClipboard(nullptr)) {
		std::cout << "[!] Failed to open clipboard." << std::endl;
		return;
	}
	EmptyClipboard();

	size_t size = term.length() + 1;
	HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, size);

	if (!hClipboardData) {
		std::cout << "[!] Clipboard memory allocation failed!" << std::endl;
		return;
	}

	memcpy(GlobalLock(hClipboardData), term.c_str(), size);
	GlobalUnlock(hClipboardData);
	SetClipboardData(CF_TEXT, hClipboardData);
	CloseClipboard();
	GlobalFree(hClipboardData);

	controls::KeyCombinationPress("Ctrl", "v");
	controls::KeyPress("Esc");

	this->isSearching = false;
	this->lastSearchedTerm = term;
	this->isTextEntered = true;
}

void asa::ASASearchBar::DeleteSearch()
{
	this->Press();

	controls::KeyCombinationPress("Ctrl", "a");
	controls::KeyPress("Delete");
	Sleep(100);
	controls::KeyPress("Esc");

	this->SetTextCleared();
}