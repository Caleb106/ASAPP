#include "asa/ui/exceptions.h"
#include "asa/ui/storage/baseinventory.h"

#include <iostream>

#include "asa/utility.h"
#include "asa/core/logging.h"
#include "asa/core/state.h"

namespace asa
{
    namespace
    {
        constexpr int MAX_ITEMS_PER_PAGE = 36;
    }

    base_inventory::base_inventory(const bool t_remote, std::unique_ptr<base_info> t_info)
        : search_bar(t_remote ? 1207 : 177, 176, 133, 44),
          item_area(t_remote ? 1205 : 178, 239, 552, 588),
          transfer_all_button_(t_remote ? 1415 : 393, 176),
          drop_all_button_(t_remote ? 1463 : 440, 176),
          new_folder_button_(t_remote ? 1558 : 536, 176),
          auto_stack_button_(t_remote ? 1606 : 584, 176),
          folder_view_button_(t_remote ? 1654 : 632, 176),
          is_remote_inventory_(t_remote),
          area_(t_remote ? 1179 : 149, 94, 591, 827),
          item_filter(t_remote ? 1205 : 175, 841, 552, 42),
          info_(t_info ? std::move(t_info) : std::make_unique<base_info>())
    {
        init_slots({t_remote ? 1205 : 178, 239});
    };

    bool base_inventory::management_button::is_toggled() const
    {
        static cv::Vec3b toggled_color{128, 231, 255};
        return utility::count_matches(area, toggled_color, 10) > 30;
    }

    bool base_inventory::management_button::is_available() const
    {
        static cv::Vec3b base_color{0, 140, 171};
        return utility::count_matches(area, base_color, 10) > 20;
    }

    bool base_inventory::inv_tab_button::is_selected() const
    {
        static cv::Vec3b selected_color{188, 244, 255};
        return utility::count_matches(area, selected_color, 10) > 100;
    }

    bool base_inventory::inv_tab_button::exists() const
    {
        static cv::Vec3b inactive_color{80, 141, 155};
        return utility::count_matches(area, inactive_color, 10) > 100 || is_selected();
    }

    bool base_inventory::is_receiving_remote_inventory() const
    {
        assert_open(__func__);
        if (!is_remote_inventory_) { return false; }

        static cv::Vec3b text_color{191, 243, 255};
        return utility::count_matches(recv_remote_inv_area_, text_color, 25) > 100;
    }

    void base_inventory::receive_remote_inventory(
        const std::chrono::seconds timeout) const
    {
        assert_open(__func__);

        if (!utility::await([this] { return !is_receiving_remote_inventory(); },
                            timeout)) {
            throw receiving_remote_inventory_timeout(this);
        }
    }

    bool base_inventory::is_open() const
    {
        return match(embedded::interfaces::cb_arrowdown, item_filter.area, 0.8f);
    }

    bool base_inventory::has(const item& item, const bool search)
    {
        assert_open(__func__);

        if (search) {
            search_bar.search_for(item.get_name());
            Sleep(100);
        }

        // if an items query isnt ambigious, i.e when we enter the item name
        // ONLY the item can show up, just check the first slot for efficiency.
        if (search && !item.get_data().has_ambiguous_query) { return slots[0].has(item); }

        return match(item.get_inventory_icon(), item_area, 0.7f, false,
                     item.get_inventory_icon_mask());
    }

    bool base_inventory::count_stacks(const item& item, int& count_out, const bool search)
    {
        assert_open(__func__);

        const auto matches = locate_all(
            item.get_inventory_icon(), item_area, 0.9f, false,
            item.get_inventory_icon_mask());

        if (matches.empty()) {
            count_out = 0;
            return true;
        }

        count_out = static_cast<int>(matches.size());
        return count_out != MAX_ITEMS_PER_PAGE;
    }

    const item_slot* base_inventory::find_item(const item& item, const bool is_searched,
                                               const bool search_for)
    {
        assert_open(__func__);

        if (!item.get_data().has_ambiguous_query && (is_searched || search_for)) {
            if (search_for) { search_bar.search_for(item.get_name()); }
            return slots[0].has(item) ? slots.data() : nullptr;
        }

        if (search_for) { search_bar.search_for(item.get_name()); }
        for (const item_slot& slot: slots) {
            if (slot.has(item)) { return &slot; }
            if (slot.is_empty()) { return nullptr; }
        }
        return nullptr;
    }

    void base_inventory::init_slots(const cv::Point& origin)
    {
        // we got 6 rows where each row has 6 columns
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                // the slots are offset by 93 on the x and y axis
                const item_slot slot(i, origin.x + (j * 93), origin.y + (i * 93));

                // the current row is (row index * 6) + col index, image row = 1 and
                // col = 4: (1 * 6) + 4 = 10, so we are at slot index 10 :)
                slots[(i * 6) + j] = slot;
            }
        }
    }

    base_inventory& base_inventory::popcorn(const item& item, const int stacks,
                                            int* stacks_dropped)
    {
        assert_open(__func__);
        int dropped = 0;
        bool searched = false;
        if (!search_bar.has_text_entered()) {
            search_bar.search_for(item.get_name());
            searched = true;
        }

        while (slots[0].has(item) && (dropped < stacks || stacks == -1)) {
            for (int i = 0; i < 4; i++) {
                select_slot(slots[i]);
                post_press(get_action_mapping("DropItem"));
                dropped++;
            }
        }

        if (stacks_dropped) { *stacks_dropped = dropped; }
        if (searched) { search_bar.delete_search(); }
        return *this;
    }

    base_inventory& base_inventory::popcorn(const int num_slots)
    {
        assert_open(__func__);

        for (int slot = num_slots - 1; slot >= 0; slot--) {
            select_slot(slot);
            post_press(get_action_mapping("DropItem"));
            checked_sleep(100ms);
        }

        return *this;
    }

    base_inventory& base_inventory::popcorn_all(const PopcornFlags flags)
    {
        while (!slots[0].is_empty() && is_open()) {
            // for performance reason only check if the slot is empty if the
            // last slot in the inventory is & respect the NoSlotChecks flag.
            const bool check_empty = !(flags & PopcornFlags_NoSlotChecks) && slots[slots.
                                         size() - 1].is_empty();

            post_down(get_action_mapping("DropItem"));
            for (int i = 0; i < MAX_ITEMS_PER_PAGE; i++) {
                const bool reached_max = i > 5 && flags & PopcornFlags_UseSingleRow;
                if (reached_max || (check_empty && slots[i].is_empty())) { break; }

                set_mouse_pos(utility::center_of(slots[i].area));
                post_down(get_action_mapping("DropItem"));
            }
        }
        post_up(get_action_mapping("DropItem"));
        return *this;
    }

    base_inventory& base_inventory::close()
    {
        const auto start = std::chrono::system_clock::now();
        while (is_open()) {
            close_button_.press();
            if (utility::await([this] { return !is_open(); }, 5s)) {
                break;
            }

            // Increased timeout to 60 seconds
            if (utility::timedout(start, 60s)) { throw failed_to_close(this); }
        }
        return *this;
    }

    base_inventory& base_inventory::select_slot(const item_slot& slot,
                                                const bool hovered_check,
                                                const bool tooltip_check)
    {
        assert_open(__func__);

        const utility::stopwatch sw;
        get_logger()->debug("Selecting slot {}..", slot.index);

        // If hover check is requested, we need to move the mouse onto the item until it
        // has the white borders appear around it.
        if (hovered_check) {
            do {
                if (sw.timedout(15s)) {
                    throw interface_interaction_timeout(
                        std::format("Failed to hover slot {}", slot.index)
                    );
                }
                const cv::Point location = utility::center_of(slot.area);
                set_mouse_pos(location);
            } while (!utility::await([slot]() -> bool { return slot.is_hovered(); }, 2s));
        } else { set_mouse_pos(utility::center_of(slot.area)); }

        while (tooltip_check && !slot.get_tooltip()) {
            if (sw.timedout(15s)) {
                throw interface_interaction_timeout(
                    std::format("Failed to get tooltip of slot {}", slot.index)
                );
            }
            if (get_user_setting<bool>("bEnableInventoryItemTooltips")) {
                toggle_tooltips();
            }
            toggle_tooltips();
            checked_sleep(100ms);
        }
        get_logger()->debug("Slot {} hovered ({} elapsed).", slot.index, sw.elapsed());
        return *this;
    }

    base_inventory& base_inventory::drop_all()
    {
        assert_open(__func__);

        drop_all_button_.press();
        search_bar.set_text_cleared();
        checked_sleep(200ms);
        return *this;
    }

    base_inventory& base_inventory::drop_all(const item& item)
    {
        search_bar.search_for(item.get_name());
        drop_all();
        return *this;
    }

    base_inventory& base_inventory::drop_all(const std::string& term)
    {
        search_bar.search_for(term);
        drop_all();
        return *this;
    }

    base_inventory& base_inventory::transfer_all(base_inventory* receiver)
    {
        const utility::stopwatch sw;
        std::function<bool()> validated = nullptr;

        if (search_bar.has_text_entered(true)) {
            // Check for the text entered state, if there is text entered then we can use
            // that to validate if the transfer all has gone through (text will be gone)
            get_logger()->debug("Transferring all with search bar validation.");
            validated = [this] { return !search_bar.has_text_entered(); };
        } else {
            get_logger()->debug("Transferring all (no validation available)..");
        }

        do {
            transfer_all_button_.press();
        } while (!utility::await([validated] { return !validated || validated(); }, 3s));
        search_bar.set_text_cleared();

        get_logger()->debug("Transfer complete ({} elapsed).", sw.elapsed());
        return *this;
    }

    base_inventory& base_inventory::transfer_all(const item& item, base_inventory* recv)
    {
        search_bar.search_for(item.get_name(), false);
        transfer_all(recv);
        return *this;
    }

    base_inventory& base_inventory::transfer_all(const std::string& term, base_inventory* recv)
    {
        search_bar.search_for(term, false);
        transfer_all(recv);
        return *this;
    }

    base_inventory& base_inventory::transfer(const item& item, const int stacks,
                                             base_inventory* recv,
                                             const bool search)
    {
        assert_open(__func__);

        if (search) {
            search_bar.search_for(item.get_name());
            checked_sleep(50ms);
        }

        int i = 0;
        while (const auto slot = find_item(item, search)) {
            if (i++ > stacks && stacks != 0) { break; }
            transfer(*slot);
        }
        return *this;
    }

    base_inventory& base_inventory::transfer(const int32_t slot, base_inventory* receiver)
    {
        return transfer(slots[slot], receiver);
    }

    base_inventory& base_inventory::transfer(const item_slot& slot, base_inventory* recv)
    {
        assert_open(__func__);
        get_logger()->debug("Transferring slot {}..", slot.index);
        const utility::stopwatch sw;

        // Hover the slot if it isnt already hovered, this will let us know whether we
        // are good to transfer it and whether it has been transferred.
        if (!slot.is_hovered()) { select_slot(slot); }

        // Press T until the slot is no longer hovered, another item may move to that slot
        // but it wont be "hovered" until the mouse is moved.
        do {
            if (sw.timedout(10s)) {
                throw interface_interaction_timeout(
                    std::format("Transferring slot {}", slot.index)
                );
            }
            post_press(get_action_mapping("TransferItem"));
        } while (!utility::await([&slot]() -> bool { return !slot.is_hovered(); }, 5s));

        get_logger()->debug("Transfer complete ({} elapsed).", sw.elapsed());
        return *this;
    }

    base_inventory& base_inventory::transfer_rows(const item& item, const int rows)
    {
        search_bar.search_for(item.get_name());

        post_press(MouseButton::LEFT, utility::center_of(slots[0].area));

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < 6; j++) {
                set_mouse_pos(utility::center_of(slots[j].area));
                post_press(get_action_mapping("TransferItem"));
                checked_sleep(250ms);
            }
        }
        search_bar.delete_search();
        return *this;
    }

    base_inventory& base_inventory::transfer_rows(const item& item,
                                                  const std::chrono::seconds duration)
    {
        search_bar.search_for(item.get_name());

        post_press(MouseButton::LEFT, utility::center_of(slots[0].area));
        const auto start = std::chrono::system_clock::now();

        while (!utility::timedout(start, duration)) {
            for (int j = 0; j < 6; j++) {
                set_mouse_pos(utility::center_of(slots[j].area));
                post_press(get_action_mapping("TransferItem"));
                checked_sleep(std::chrono::milliseconds(250));
            }
        }
        search_bar.delete_search();
        return *this;
    }

    base_inventory& base_inventory::select_info_tab()
    {
        assert_open(__func__);
        if (is_remote_inventory_) { them_button_.press(); } else { you_button_.press(); }
        return *this;
    }

    base_inventory& base_inventory::auto_stack()
    {
        assert_open(__func__);
        auto_stack_button_.press();
        return *this;
    }

    base_inventory& base_inventory::toggle_tooltips()
    {
        post_press(get_action_mapping("TransferItem"));
        return *this;
    }

    base_inventory& base_inventory::make_new_folder(const std::string& folder_name)
    {
        new_folder_button_.press();
        asa::checked_sleep(std::chrono::milliseconds(500));

        post_press(MouseButton::LEFT, cv::Point{895, 499});
        utility::set_clipboard(folder_name);
        post_combination("ctrl", "v");
        post_press("enter");
        return *this;
    }

    void base_inventory::assert_open(std::string for_action) const
    {
        if (!utility::await([this] { return is_open(); }, 5s)) {
            throw no_interface_open(std::move(for_action), this);
        }
    }

    std::vector<std::unique_ptr<item> > base_inventory::get_current_page_items(
        std::vector<std::string>* allowed_items,
        std::vector<item_data::ItemType>* allowed_categories,
        const int num_threads) const
    {
        assert_open(__func__);

        const auto start = std::chrono::system_clock::now();
        std::cout << "[+] Getting all items of the current page...\n";
        // get the amount of slots that we need to fill, this makes multithreading the
        // determination process easier as we can assign one thread per slot from the start
        int num_slots_filled = 0;
        int folder_offset = 0;
        for (const auto& slot: slots) {
            if (slot.is_folder()) {
                folder_offset++;
                continue;
            }
            if (slot.is_empty()) { break; }
            num_slots_filled++;
        }

        std::cout << "\t[-] " << num_slots_filled << " slots to be determined...\n";
        std::vector<std::unique_ptr<item> > ret(num_slots_filled);
        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        for (int i = 0; i < num_threads; i++) {
            threads.emplace_back(
                [this, i, &ret, num_threads, num_slots_filled, folder_offset]() -> void {
                    for (int j = i; j < num_slots_filled; j += num_threads) {
                        ret[j] = slots[j + folder_offset].get_item();
                    }
                });
        }

        for (auto& thread: threads) { thread.join(); }
        return ret;
    }
}
