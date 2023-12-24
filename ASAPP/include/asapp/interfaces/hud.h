#pragma once
#include "asapp/items/item.h"
#include "asapp/game/window.h"
#include "asapp/interfaces/iinterface.h"

namespace asa::interfaces
{
    class HUD : public IInterface
    {
    public:
        bool is_open() const override { return true; }

        [[nodiscard]] bool is_mount_overweight() { return false; };
        [[nodiscard]] bool is_mount_low() { return false; };
        [[nodiscard]] bool is_mount_out_of_food() { return false; };
        [[nodiscard]] bool is_mount_out_of_stamina() { return false; };
        [[nodiscard]] bool is_mount_sprinting() { return false; };

        [[nodiscard]] bool is_player_overweight() const;
        [[nodiscard]] bool is_player_broken_bones() const;
        [[nodiscard]] bool is_player_out_of_water() const;
        [[nodiscard]] bool is_player_out_of_food() const;
        [[nodiscard]] bool is_player_sprinting() const;

        [[nodiscard]] bool can_default_teleport() const;
        [[nodiscard]] bool can_fast_travel() const;
        [[nodiscard]] bool can_access_inventory() const;

        [[nodiscard]] bool extended_information_is_toggled() const;

        /**
         * @brief Checks whether an item was removed in the given area
         * 
         * @param area The area to check whether an item was removed.
         * 
         * @return True if an item was removed, false otherwise.  
         */
        [[nodiscard]] static bool item_removed(const window::Rect& area);

        /**
         * @brief Checks whether an item was added in the given area
         * 
         * @param area The area to check whether an item was added.
         * 
         * @return True if an item was added, false otherwise.  
         */
        [[nodiscard]] static bool item_added(const window::Rect& area);
        [[nodiscard]] bool item_added(items::Item&, window::Rect* roi_out) const;
        [[nodiscard]] bool item_removed(items::Item&, window::Rect* roi_out) const;
        [[nodiscard]] bool transferred_item_into_dedicated_storage() const;

        bool count_items_added(items::Item&, int& amount_out) const;
        bool count_items_removed(items::Item&, int& amount_out) const;

        float get_water_amount() const;
        float get_food_amount() const;
        float get_weight_amount() const;

    private:
        window::Color blink_red_state_{109, 54, 52};
        window::Color blink_red_state_weight_{255, 45, 45};

        window::Rect water_icon_{1868, 806, 34, 46};
        window::Rect food_icon_{1861, 858, 46, 41};
        window::Rect stamina_icon_{1872, 900, 29, 51};
        window::Rect health_icon_{1861, 953, 48, 45};
        window::Rect weight_icon_{1860, 751, 51, 56};
        window::Rect default_teleport_{866, 911, 78, 27};

        window::Rect item_icon_removed_or_added_area{7, 12, 41, 1067};
        window::Rect item_removed_area{43, 20, 110, 1054};
        window::Rect item_added_area{40, 15, 85, 1063};
        window::Rect status_update_area{844, 12, 224, 30};
    };

    inline HUD* hud = new HUD();
}