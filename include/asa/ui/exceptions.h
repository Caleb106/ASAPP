#pragma once
#include "asainterface.h"
#include "asa/core/exceptions.h"

#include <format>

namespace asa
{
    class interface_error : public asapp_error
    {
        const asainterface* _interface;
    public:
        interface_error(const asainterface* t_interface, std::string t_info);

        explicit interface_error(const asainterface* t_interface);

        const char* what() const noexcept override;
    };

    class failed_to_open : public interface_error
    {
    public:
        explicit failed_to_open(const asainterface* t_interface);
    };

    class failed_to_close : public interface_error
    {
    public:
        explicit failed_to_close(const asainterface* t_interface);
    };

    class receiving_remote_inventory_timeout : public interface_error
    {
    public:
        explicit receiving_remote_inventory_timeout(const asainterface* t_interface);
    };

    /**
     * @brief Thrown when an action is attempted on an interface that requires
     * the it to be open, but it was not.
     */
    class no_interface_open : public interface_error
    {
    public:
        no_interface_open(std::string t_attempted_action,
                          const asainterface* t_interface);
    };

    class interface_interaction_timeout final : public interface_error
    {
    public:
        explicit interface_interaction_timeout(const std::string& t_what_action);
    };


}
