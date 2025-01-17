#pragma once
#include <string>
#include <format>
#include <opencv2/core/mat.hpp>

namespace asa
{
    struct tribelog_message
    {
    public:
        enum EventType : int
        {
            UNKNOWN,             // Could not be determined or has not yet been.
            DEMOLISHED,          // Structure was demolished (yellow)
            CLAIMED,             // Dino was unclaimed (purple)
            UNCLAIMED,           // Dino was unclaimed (yellow)
            UPLOADED,            // Dino was uploaded (light red ish?)
            DOWNLOADED,          // Dino was downloaded (light green)
            DINO_TAMED,          // Dino was tamed (green)
            PLAYER_ADDED,        // Player was added to the tribe (cyan)
            PLAYER_REMOVED,      // Player was removed from the tribe (yellow)
            DINO_STARVED,        // Dino has starved to death (white)
            DINO_CRYOD,          // Dino was cryopoded (white)
            DEMOTED,             // Tribemember was demoted (orange)
            PROMOTED,            // Tribemember was promoted (blue)
            TRIBE_GROUP_UPDATED, // Group of a tribemember was updated (white)
            TRIBE_DESTROYED,     // Structure of our own tribe was destroyed (red)
            TRIBE_PLAYER_KILLED, // Player of our own tribe was killed (red)
            TRIBE_DINO_KILLED,   // Dino of our own tribe was killed (red)
            ENEMY_DESTROYED,     // Structure of an enemy tribe was destroyed (beige?)
            ENEMY_DINO_KILLED,   // Dino of an enemy tribe was killed (purple)
            ENEMY_PLAYER_KILLED, // Player of an enemy tribe was killed (purple)
        };

        struct timestamp
        {
            int32_t day, hour, minute, second;

            bool operator>(const timestamp& other) const { return sum() > other.sum(); }

            bool operator<(const timestamp& other) const { return sum() < other.sum(); }

            bool operator==(const timestamp& other) const { return sum() == other.sum(); }

            /**
             * @Brief Gets the sum of seconds passed between the creation of the server
             * and the log message.
             */
            [[nodiscard]] int64_t sum() const;

            /**
             * @brief Returns a string representation of the timestamp just like in-game.
             */
            [[nodiscard]] std::string to_string() const;

            /**
             * @brief Constructs the timestamp from a raw string using a regex pattern.
             *
             * @param raw The string to extract the timestamp from.
             */
            static timestamp parse(const std::string& raw);
        };

    public:
        timestamp time;
        EventType type = UNKNOWN;

        std::string content;
        std::string raw_text;

        cv::Mat raw_image;

        [[nodiscard]] std::string to_string() const
        {
            return std::format("{}: {}", time.to_string(), content);
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const tribelog_message& msg)
    {
        return os << msg.to_string();
    }
}
