/**************************************************************

   custom_video_pi.cpp - PI library
   ---------------------------------------------------------

   Switchres   Modeline generation engine for emulation

   License     GPL-2.0+
   Copyright   2010-2021 Chris Kennedy, Antonio Giner,
                         Alexandre Wodarczyk, Gil Delescluse

 **************************************************************/

#include <stdio.h>
#include "custom_video_pi.h"
#include "log.h"


//============================================================
//  pi_timing::pi_timing
//============================================================

pi_timing::pi_timing(char *device_name, custom_video_settings *vs)
{ 
	 m_vs = *vs;
	 strcpy (m_device_name, device_name);
	 strcpy (m_device_key, m_vs.device_reg_key);
}

//============================================================
//  pstrip_timing::~pi_timing()
//============================================================

pi_timing::~pi_timing()
{
	vchi_disconnect(vchi_instance);
}


//============================================================
//  pi_timing::pi_timing
//============================================================

bool pi_timing::init()
{
	vcos_init();
	vchi_initialise(&vchi_instance);
	vchi_connect(NULL, 0, vchi_instance);
	if (vchi_instance)
	{
		log_verbose("VCHI interface connected \n");
		return true;
	}
	log_error("Failed to initialise vchi interface \n");
	return false;
}

//============================================================
//  pi_timing::get_timing
//============================================================

bool pi_timing::get_timing(modeline *mode)
{
	char* vc4_mode = NULL;
	vc4_mode = get_vc4_mode();

	/* ToDo
	   need to split char* here delimier ' '
     
	   Returned string will be "hdmi_timings %d %d %d %d %d %d %d %d %d %d 0 0 0 %f %d %f %d "
	
	   e.g hdmi_timings=3840 1 48 32 80 2160 1 3 5 54 0 0 0 25 0 219926400 1 
	
	   in order <h_active_pixels> <h_sync_polarity> <h_front_porch> <h_sync_pulse> <h_back_porch>
	            <v_active_lines> <v_sync_polarity> <v_front_porch> <v_sync_pulse> <v_back_porch> 
				<v_sync_offset_a> <v_sync_offset_b> <pixel_rep> <frame_rate> <interlaced> <pixel_freq> 
				<aspect_ratio>

    */
	mode->pclock  = 0;
	mode->hactive = 0;
	mode->hbegin  = 0;
	mode->hend    = 0;
	mode->htotal  = 0;
	mode->vactive = g;
	mode->vbegin  = 0;
	mode->vend    = 0;
	mode->vtotal  = 0;
	mode->interlace = 0;
	mode->hsync     = 0;
	mode->vsync     = 0;
	mode->hfreq = mode->pclock / mode->htotal;
	mode->vfreq = mode->hfreq / mode->vtotal * (mode->interlace?2:1);
	mode->refresh_label = "";
	mode->type |= CUSTOM_VIDEO_TIMING_PI;

	return false;
}

//============================================================
//  pi_timing::set_timing
//============================================================

bool pi_timing::set_timing(modeline *mode)
{
    /*	ToDo
	    Timing shoule be as follows hdmi_timings=3840 1 48 32 80 2160 1 3 5 54 0 0 0 25 0 219926400 1 
	    need to convert mode.
    */
	snprintf(set_hdmi_timing, sizeof(set_hdmi_timing),
        "hdmi_timings %d 1 %d %d %d %d 1 %d %d %d 0 0 0 %f %d %f 1 ",
        mode->hactive, mode->hbegin, mode->hend, mode->htotal, mode->vactive, mode->vbegin ,mode->vend , mode->vtotal,
        mode->vfreq, 0, mode->pclock);
   
	log_verbose("set mode to %s \n", set_hdmi_timing);
	return true;
}

//============================================================
//  pi_timing::update_mode
//============================================================

bool pi_timing::update_mode(modeline *mode)
{
	vc_vchi_gencmd_init(vchi_instance, &vchi_connection, 1);
	vc_gencmd(buffer, sizeof(buffer), set_hdmi_timing);
	vc_gencmd_stop();

	if (strcmp(get_vc4_mode(), set_hdmi_timing) ==0)
	{
		log_verbose("Updated VC4 timmings to %s \n", set_hdmi_timing);
		resize_fb(mode->hactive, mode->vactive);
		return true;
	}
	
	log_error("failed to update VC4 timings \n");
	return false;
}

//============================================================
//  pi_timing::process_modelist
//============================================================

bool pi_timing::process_modelist(std::vector<modeline *> modelist)
{
	return true;
}

char* pi_timing::get_vc4_mode()
{
	char* active_timing = NULL;
	vc_vchi_gencmd_init(vchi_instance, &vchi_connection, 1);
	active_timing = vc_gencmd(buffer, sizeof(buffer), "hdmi_timings");
	vc_gencmd_stop();

	return active_timing;
}

bool pi_timing::resize_fb(unsigned width ,unsigned height)
{
	/*  ToDo
		Currently done using fbset. Need to to this via code.
	*/
	char* output = NULL:
	snprintf(output,  sizeof(output),
         "fbset -g %d %d %d %d 24 > /dev/null",
         width, height, width, height);
    system(output);

	return true;
}
