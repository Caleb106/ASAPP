#pragma once
#include "tooltip.h"
#include "interface_component.h"
#include "asa/items/item.h"

namespace asa
{
    struct item_slot : interface_component
    {
    public:
        item_slot() : item_slot(0, 0, 0) {}

        item_slot(const int32_t t_index, const int t_x, const int t_y)
            : interface_component(t_x, t_y, 86, 87), index(t_index) {}

        /**
         * @brief Computes the rect of the stack size value for this slot.
         * 
         * @return A rectangle containing the x, y, width and height of the stack size area.
         */
        [[nodiscard]] cv::Rect get_stack_size_area() const;

        /**
         * @brief Computes the rect of the armor / damage icon for this slot.
         * 
         * @return A rectangle containing the x, y, width and height of the icon area.
         */
        [[nodiscard]] cv::Rect get_armor_or_damage_icon_area() const;

        /**
         * @brief Computes the rect of the spoil / dura bar for this slot.
         * 
         * @return A rectangle containing the x, y, width and height of the bar.
         */
        [[nodiscard]] cv::Rect get_spoil_or_durability_bar_area() const;

        /**
         * @brief Computes the rect of the folder name for this slot.
         * 
         * @return A rectangle containing the x, y, width and height of the folder name.
         */
        [[nodiscard]] cv::Rect get_folder_name_area() const;

        /**
         * @brief Computes the rect where the weight value is displayed for this slot.
         * 
         * @return A rectangle containing the x, y, width and height of the weight value.
         */
        [[nodiscard]] cv::Rect get_weight_area() const;

        /**
         * @brief Computes the rect where the weight value is displayed for this slot.
         * 
         * @return A rectangle containing the x, y, width and height of the weight value.
         */
        [[nodiscard]] cv::Rect get_hovered_area() const;

        /**
         * @brief Gets the tooltip of a hovered item.
         *
         * @remarks If the slot is empty, a null pointer is returned immediately.
         * @remarks The slot must be hovered and toggle tooltips on for this to succeed.
         * 
         * @return A pointer to the constructed tooltip or null pointer upon failure. 
         */
        [[nodiscard]] std::unique_ptr<item_tooltip> get_tooltip() const;

        /**
         * @brief Determines whether the slot is empty or has an item in it.
         * 
         * @return True if the slot is empty, false otherwise.
         */
        [[nodiscard]] bool is_empty() const;

        /**
         * @brief Determines whether the slot is a folder.
         * 
         * @return True if the slot is a folder, false otherwise.
         */
        [[nodiscard]] bool is_folder() const;

        /**
         * @brief Computes the rect of the stack size value for this slot.
         * 
         * @return A rectangle containing the x, y, width and height of the stack size area.
         */
        [[nodiscard]] bool is_hovered() const;

        /**
         * @brief Determines whether a given item is located inside this slot.
         * 
         * @param item The item to check for.
         * @param accuracy_out An optional pointer to a float to store the match accuracy.
         * @param cache_img If true, the last taken image of the slot will be reused.
         *
         * @return True if the given item is located in this slot, false otherwise.
         */
        [[nodiscard]] bool has(const item& item, float* accuracy_out = nullptr,
                               bool cache_img = false) const;

        /**
         * @brief Determines the item located in the slot.
         *
         * Analyzes the item to narrow down possibilities based on certain characteristics.
         * Once the pool optimization is performed, it checks through every possible option
         * until a match is found.
         *
         * @remarks The quality of the item and whether it is a blueprint are also determined.
         *
         * @return A unique pointer to the determined item object.
         */
        [[nodiscard]] std::unique_ptr<item> get_item() const;

        /**
         * @brief Gets the durability of the item located in the slot.
         *
         * @param durability_out The float to store the durability in.
         *
         * @return Whether the slot is not empty and has a durability value.
         */
        bool get_item_durability(float& durability_out) const;

        int32_t index;

    private:
        /**
         * @brief Contains the information that we have about an item in the slot without
         * knowing the actual item.
         * 
         * @note Designed to help the item determination process to narrow down the options.
         */
        struct predetermination_result final
        {
        public:
            bool has_armor_modifier{false};
            bool has_damage_modifier{false};
            bool is_stack{false};
            bool has_spoil_bar{false};
            bool has_durability_bar{false};

            [[nodiscard]] bool matches(const item_data& data) const;

            [[nodiscard]] bool matches(item_data::ItemType type) const;
        };

        friend std::ostream& operator<<(std::ostream& os,
                                        const predetermination_result& d);

    private:
        /**
         * @brief Checks whether the item in this slot has an armor modifier.
         *
         * @remarks Shields are of the equippable type but no not have an armor modifier.
         * 
         * @return True if the item has an armor modifier, false otherwise.
         */
        [[nodiscard]] bool has_armor_value() const;

        /**
         * @brief Checks whether the item in this slot has a damage modifier.
         *
         * @remarks Not all weapons have a damage modifier (C4 detonator, spyglass...).
         * 
         * @return True if the item has a damage modifier, false otherwise.
         */
        [[nodiscard]] bool has_damage_value() const;

        /**
         * @brief Checks whether the item in this slot has a spoil timer.
         *
         * Useful to narrow down what items could possibly be located in the slot
         * and fiter out all items that do not have a spoil timer.
         *
         * @remark The spoil and durability bar are distinguishable.
         * 
         * @returns True if the item has a spoil timer, false otherwise.
         */
        [[nodiscard]] bool has_spoil_timer() const;

        /**
         * @brief Checks whether the item in this slot has a spoil timer.
         *
         * Useful to narrow down what items could possibly be located in the slot
         * and fiter out all items that do not have a spoil timer.
         *
         * @remark The spoil and durability bar are distinguishable.
         * 
         * @returns True if the item has a spoil timer, false otherwise.
         */
        [[nodiscard]] bool has_durability() const;

        /**
         * @brief Checks whether the item in this slot is part of a stack of items.
         *
         * Useful to narrow down what items could possibly be located in the slot and
         * filter out all items that are limited to a stack size of 1.
         *
         * @remarks An item not being a stack doesnt mean it can't be a stacked.
         * 
         * @return True if the item is a stack, false otherwise. 
         */
        [[nodiscard]] bool is_stack() const;

        /**
         * @brief Determines whether the item in the slot is a blueprint, given that
         * the base of the item has already been identified and the data is available.
         * 
         * @param data The data of the base item located in the slot.
         * 
         * @return True if the item is a blueprint, false otherwise.
         */
        [[nodiscard]] bool is_blueprint(const item_data& data) const;

        /**
         * @brief Determines the quality of the item in the slot.
         *
         * @remarks Works for blueprints and precrafted items.
         * @remarks Items without quality (consumables, ammo...) are considered primitive.
         * 
         * @return An enum value representing the quality of the item.
         */
        [[nodiscard]] item_data::ItemQuality get_quality() const;

        /**
         * @brief Collects data about the item without knowing the item itself.
         * 
         * @return The result containing the collected data.
         */
        [[nodiscard]] predetermination_result predetermine() const;

        mutable cv::Mat last_img_;
    };
}
