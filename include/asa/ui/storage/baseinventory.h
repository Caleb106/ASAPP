#pragma once
#include "asa/ui/asainterface.h"
#include "asa/ui/components/components.h"
#include "asa/ui/info/baseinfo.h"
#include "asa/game/window.h"

#include <array>

enum PopcornFlags_
{
    PopcornFlags_None = 0,
    PopcornFlags_NoSlotChecks = 1 << 1, // disables the slot empty check for every slot
    PopcornFlags_UseSingleRow = 1 << 2, // only use the top row to popcorn

    PopcornFlags_Default = PopcornFlags_None,
};

using PopcornFlags = int32_t;

namespace asa
{
    class base_inventory : public asainterface
    {
    public:
        explicit base_inventory(bool t_remote = true,
                                std::unique_ptr<base_info> t_info = nullptr);

        enum Tabs
        {
            INVENTORY,
        };

    public:
        /**
         * @brief Gets the info component of the inventory.
         */
        [[nodiscard]] virtual base_info* get_info() const
        {
            return info_.get();
        }

        /**
         * @brief Gets the full area of the inventory, differs between local and remote.
         * 
         * @return A rect containing x, y, width and height of the inventories area.
         */
        [[nodiscard]] cv::Rect get_area() const { return area_; }

        /**
         * @brief Checks whether this inventory is currently being received (loaded).
         *
         * @remarks Only a remote inventory has to be received, the local inventory
         * will always be available instantly.
         * 
         * @return True if the inventory is being received, false otherwise.
         */
        [[nodiscard]] bool is_receiving_remote_inventory() const;

        /**
         * @brief Checks whether the inventory is currently open.
         *
         * 
         * @return True if the inventory is open, false otherwise.
         */
        [[nodiscard]] bool is_open() const override;

        /**
         * @brief Waits to receive the remote inventory within a given duration.
         * 
         * @param timeout How long the action is allowed to take until an exception is thrown.
         * 
         * @throws ReceivingRemoteInventoryTimeoutError if the action took too long.
         */
        void receive_remote_inventory(std::chrono::seconds timeout) const;

        /**
         * @brief Checks whether a given item is located within the inventory.
         * 
         * @param item The item to check for.
         * @param search Whether to search for the item to determine whether it's there.
         * 
         * @return True if at least one occurence of the item is found, false otherwise.
         */
        bool has(const item& item, bool search = false);

        /**
         * @brief Counts how many stacks of a given item are located in an inventory.
         * 
         * @param item The item to count.
         * @param count_out Out parameter for the counted stacks.
         * @param search Whether to search for the item to count the stacks.
         *
         * @remarks Only up to 36 stacks can be counted (one inventory page).
         *
         * @return Whether the counted amount was smaller than the max inventory page.
         */
        bool count(const item& item, int& count_out, bool search = false);

        /**
         * @brief Finds the slot of a given item in the inventory.
         * 
         * @param is_searched Whether the item is currently searched for.
         * @param search_for Whether to search for the item first.
         * 
         * @return A pointer to the slow that the item is located in, or nullptr if not found.
         */
        const item_slot* find_item(const item&, bool is_searched = false,
                                   bool search_for = false);

        /**
         * @brief Popcorns a given amount of an item from the inventory.
         * 
         * @param item The item to popcorn.
         * @param stacks The amount of stacks to popcorn, default all (-1).
         * @param dropped Optional integer pointer for how many stacks were dropped.
         */
        [[maybe_unused]] base_inventory& popcorn(
            const item& item, int stacks = -1, int* dropped = nullptr);

        /**
         * @brief Popcorns n-slots from the inventory.
         * \param num_slots How many slots to popcorn.
         */
        [[maybe_unused]] base_inventory& popcorn(int num_slots);

        /**
         * @brief Popcorns from the provided item from inventory for the provided duration.
         *
         */
        [[maybe_unused]] base_inventory& popcorn(const std::string& term,
                                                 std::chrono::seconds duration = 5s);

        /**
         * @brief Popcorns al items from the inventory. 
         */
        [[maybe_unused]] base_inventory& popcorn_all(PopcornFlags = PopcornFlags_Default);

        /**
         * @brief Selects the given slot.
         * 
         * @param slot The slot to select.
         */
        [[maybe_unused]] base_inventory& select_slot(const item_slot& slot,
                                                     bool hovered_check = true,
                                                     bool tooltip_check = false);

        /**
          * @brief Selects the given slot by index.
          *
          * @index The index of the slot to select.
          */
        [[maybe_unused]] base_inventory& select_slot(
            const int index, const bool hovered_check = true,
            const bool tooltip_check = false)
        {
            return select_slot(slots[index], hovered_check, tooltip_check);
        }

        /**
         * @brief Closes the inventory.
         *
         * @remarks Uses the x button instead of the esc key in order to prevent
         * a double press in extreme cases (game froze, bad fps, bad timing...)
         */
        [[maybe_unused]] base_inventory& close();

        /**
         * @brief Drops all items frop the inventory.
         */
        [[maybe_unused]] base_inventory& drop_all();

        /**
         * @brief Drops all of the given item from the inventory.
         * @param item The item to drop all of.
         */
        [[maybe_unused]] base_inventory& drop_all(const item& item);

        /**
         * @brief Searches a term and drops all of the matches.
         * @param term The term to filter and drop all for.
         */
        [[maybe_unused]] base_inventory& drop_all(const std::string& term);

        /**
         * @brief Transfers n-items into another inventory.
         *
         * @param item The item to transfer.
         * @param stacks The amount of stacks to transfer.
         * @param receiver The receiving inventory (default nullptr).
         * @param search Whether the item should be searched for (default true).
         *
         * @remarks The pointer to the receiving end may help to confirm transfers.
         */
        [[maybe_unused]] base_inventory& transfer(const item& item, int stacks = 1,
                                                  base_inventory* receiver = nullptr,
                                                  bool search = true);

        [[maybe_unused]] base_inventory& transfer(int32_t slot,
                                                  base_inventory* receiver = nullptr);

        [[maybe_unused]] base_inventory& transfer(const item_slot& slot,
                                                  base_inventory* recv = nullptr);

        /**
         * @brief Transfers all items to another inventory.
         *
         * @param receiver A pointer to the receiving inventory (optional).
         *
         * @remarks The pointer to the receiving end may help to confirm transfers.
         */
        [[maybe_unused]] base_inventory& transfer_all(base_inventory* receiver = nullptr);

        /**
         * @brief Transfers all of a given item into another inventory.
         *
         * @param item The item to transfer all of.
         * @param recv A pointer to the receiving inventory (optional).
         *
         * @remarks The pointer to the receiving end may help to confirm transfers.
         */
        [[maybe_unused]] base_inventory& transfer_all(
            const item& item, base_inventory* recv = nullptr);

        /**
         * @brief Transfers all of a given item into another inventory.
         *
         * @param term The term to search and transfer all of.
         * @param recv A pointer to the receiving inventory (optional).
         *
         * @remarks The pointer to the receiving end may help to confirm transfers.
         */
        [[maybe_unused]] base_inventory& transfer_all(const std::string& term,
                                                      base_inventory* recv = nullptr);

        /**
         * @brief Transfers a given amount of rows of a given item.
         *
         * @param item The item to transfer x rows of, will be searched beforehand
         * @param rows The estimated amounts of rows to transfer.
         *
         * @remark The amount of rows transferred is not checked but guessed.
         */
        [[maybe_unused]] base_inventory& transfer_rows(const item& item, int rows);

        /**
         * @brief Selects the info tab i.e the THEM / STRUCTURE tab for
         * remote inventories and the YOU tab for the local inventory.
         */
        [[maybe_unused]] base_inventory& select_info_tab();

        /**
         * @brief Transfers rows of a given item for a given duration.
         *
         * @param item The item to transfer x rows of, will be searched beforehand
         * @param duration The duration to transfer rows for in seconds.
         *
         * @remark The amount of rows transferred is not checked but guessed.
         */
        [[maybe_unused]] base_inventory& transfer_rows(const item& item,
                                                       std::chrono::seconds duration);

        /**
         * @brief Creates a new folder given a name.
         *
         * @param folder_name The name of the folder to create.
         *
         * @remarks Folder names must contain at least 2 characters.
         */
        [[maybe_unused]] base_inventory& make_new_folder(const std::string& folder_name);

        /**
         * @brief Auto stack all items in the inventory.
         */
        [[maybe_unused]] base_inventory& auto_stack();

        /**
         * @brief Toggles the inventory item tooltip on / off.
         */
        [[maybe_unused]] base_inventory& toggle_tooltips();

        /**
         * @brief Retrieves all items from the currently visible page.
         *
         * @param allowed_items Whitelist of allowed items, other items are not checked.
         * @param allowed_categories Whitelist of allowed item types, others are not checked.
         * @param num_threads The number of threads to use, default 5.
         *
         * @return A vector containing unique pointers to all items  in the current page.
         *
         * @remarks When an empty slot is encountered, the evaluation is stopped and the
         * result is returned. Otherwise the result is returned after the 36th slot.
         */
        [[nodiscard]] std::vector<std::unique_ptr<item> > get_current_page_items(
            std::vector<std::string>* allowed_items = nullptr,
            std::vector<item_data::ItemType>* allowed_categories = nullptr,
            int num_threads = 5) const;

        std::array<item_slot, 36> slots;
        search_bar search_bar;
        cv::Rect item_area;

    protected:
        struct management_button : button
        {
            management_button(const int t_x, const int t_y) : button(t_x, t_y, 45, 45) {}

            [[nodiscard]] bool is_toggled() const;

            [[nodiscard]] bool is_available() const;
        };

        struct inv_tab_button : button
        {
            using button::button;

            [[nodiscard]] bool is_selected() const;

            [[nodiscard]] bool exists() const;
        };

    protected:
        void assert_open(std::string for_action) const;

        inv_tab_button you_button_{756, 122, 117, 51};
        inv_tab_button them_button_{1043, 126, 121, 48};

        management_button transfer_all_button_;
        management_button drop_all_button_;
        management_button new_folder_button_;
        management_button auto_stack_button_;
        management_button folder_view_button_;

        bool is_remote_inventory_;
        cv::Rect area_;
        cv::Rect recv_remote_inv_area_{1340, 511, 295, 34};
        button close_button_{1781, 49, 36, 33};

        combo_box item_filter;


        std::unique_ptr<base_info> info_;

    private:
        void init_slots(const cv::Point& origin);
    };
}
