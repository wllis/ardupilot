#pragma once
#include <GCS_MAVLink/GCS.h>

/*
 *  GCS backend used for many examples and tools
 */
class GCS_MAVLINK_HerePro : public GCS_MAVLINK
{
public:

    using GCS_MAVLINK::GCS_MAVLINK;

private:

    uint32_t telem_delay() const override { return 0; }
    void handleMessage(const mavlink_message_t &msg) override { handle_common_message(msg); }
    bool handle_guided_request(AP_Mission::Mission_Command &cmd) override { return true; }
    void handle_change_alt_request(AP_Mission::Mission_Command &cmd) override {}

protected:

    uint8_t sysid_my_gcs() const override;

    // HerePro information:
    MAV_MODE base_mode() const override { return (MAV_MODE)MAV_MODE_FLAG_CUSTOM_MODE_ENABLED; }
    MAV_STATE vehicle_system_status() const override { return MAV_STATE_CALIBRATING; }

    bool set_home_to_current_location(bool _lock) override { return false; }
    bool set_home(const Location& loc, bool _lock) override { return false; }

    void send_nav_controller_output() const override {};
    void send_pid_tuning() override {};
};

/*
 * a GCS singleton used for many example sketches and tools
 */

extern const AP_HAL::HAL& hal;

class GCS_HerePro : public GCS
{
public:

    using GCS::GCS;

protected:

    uint8_t sysid_this_mav() const override { return 1; }

    GCS_MAVLINK_HerePro *new_gcs_mavlink_backend(GCS_MAVLINK_Parameters &params,
                                               AP_HAL::UARTDriver &uart) override {
        return new GCS_MAVLINK_HerePro(params, uart);
    }

private:
    GCS_MAVLINK_HerePro *chan(const uint8_t ofs) override {
        if (ofs > _num_gcs) {
            INTERNAL_ERROR(AP_InternalError::error_t::gcs_offset);
            return nullptr;
        }
        return (GCS_MAVLINK_HerePro *)_chan[ofs];
    };
    const GCS_MAVLINK_HerePro *chan(const uint8_t ofs) const override {
        if (ofs > _num_gcs) {
            INTERNAL_ERROR(AP_InternalError::error_t::gcs_offset);
            return nullptr;
        }
        return (GCS_MAVLINK_HerePro *)_chan[ofs];
    };

    void send_textv(MAV_SEVERITY severity, const char *fmt, va_list arg_list, uint8_t dest_bitmask) override {
        hal.console->printf("TOGCS: ");
        hal.console->vprintf(fmt, arg_list);
        hal.console->printf("\n");
    }

    MAV_TYPE frame_type() const override { return MAV_TYPE_GENERIC; }
    uint32_t custom_mode() const override { return 3; } // magic number
};
