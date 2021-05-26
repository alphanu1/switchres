/**************************************************************

    custom_video_pi.h - VC4 library header

    ---------------------------------------------------------

    Switchres   Modeline generation engine for emulation

    License     GPL-2.0+
    Copyright   2010-2021 Chris Kennedy, Antonio Giner,
                          Alexandre Wodarczyk, Gil Delescluse


 **************************************************************/
#ifndef __CUSTOM_VIDEO_PI__
#define __CUSTOM_VIDEO_PI__

#include "/opt/vc/include/userland/interface/vmcs_host/vc_vchi_gencmd.h"
#include "custom_video.h"


class pi_timing : public custom_video
{
	public:
		pi_timing(char *device_name, custom_video_settings *vs);
		~pi_timing() {};
		const char *api_name() { return "VC4"; }
		bool init();
		int caps() { return CUSTOM_VIDEO_CAPS_UPDATE | CUSTOM_VIDEO_CAPS_SCAN_EDITABLE; }

		bool update_mode(modeline *mode);

		bool get_timing(modeline *mode);
		bool set_timing(modeline *mode);

		bool process_modelist(std::vector<modeline *>);

	private:
		char* buffer;
		char* set_hdmi_timing;
		VCHI_INSTANCE_T vchi_instance;
		VCHI_CONNECTION_T *vchi_connection  = NULL;
		char* pi_timing::get_vc4_mode();
		bool pi_timing::resize_fb();

		char m_display_name[32];
		char m_device_key[128];

};
