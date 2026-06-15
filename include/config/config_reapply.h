#ifndef CONFIG_REAPPLY_H
#define CONFIG_REAPPLY_H

#include "config_defaults.h"

void reset_blur_params(void) {
	if (config.blur) {
		Monitor *m = NULL;
		wl_list_for_each(m, &mons, link) {
			if (m->blur != NULL) {
				wlr_scene_node_destroy(&m->blur->node);
			}
			m->blur = wlr_scene_optimized_blur_create(&scene->tree, 0, 0);
			wlr_scene_node_reparent(&m->blur->node, layers[LyrBlur]);
			wlr_scene_optimized_blur_set_size(m->blur, m->m.width, m->m.height);
			wlr_scene_set_blur_data(
				scene, config.blur_params.num_passes, config.blur_params.radius,
				config.blur_params.noise, config.blur_params.brightness,
				config.blur_params.contrast, config.blur_params.saturation);
		}
	} else {
		Monitor *m = NULL;
		wl_list_for_each(m, &mons, link) {

			if (m->blur) {
				wlr_scene_node_destroy(&m->blur->node);
				m->blur = NULL;
			}
		}
	}
}

void reapply_monitor_rules(void) {
	ConfigMonitorRule *mr;
	Monitor *m = NULL;
	int32_t ji, vrr, custom;
	int32_t mx, my;
	struct wlr_output_state state;

	wl_list_for_each(m, &mons, link) {
		if (!m->wlr_output->enabled)
			continue;

		wlr_output_state_init(&state);

		for (ji = 0; ji < config.monitor_rules_count; ji++) {
			if (config.monitor_rules_count < 1)
				break;

			mr = &config.monitor_rules[ji];

			if (monitor_matches_rule(m, mr)) {
				mx = mr->x == INT32_MAX ? m->m.x : mr->x;
				my = mr->y == INT32_MAX ? m->m.y : mr->y;
				vrr = mr->vrr >= 0 ? mr->vrr : 0;
				custom = mr->custom >= 0 ? mr->custom : 0;

				(void)apply_rule_to_state(m, mr, &state, vrr, custom);
				wlr_output_layout_add(output_layout, m->wlr_output, mx, my);
				wlr_output_commit_state(m->wlr_output, &state);
				break;
			}
		}

		wlr_output_state_finish(&state);
	}
	updatemons(NULL, NULL);
}

void set_xcursor_env() {
	if (config.cursor_size > 0) {
		char size_str[16];
		snprintf(size_str, sizeof(size_str), "%d", config.cursor_size);
		setenv("XCURSOR_SIZE", size_str, 1);
	} else {
		setenv("XCURSOR_SIZE", "24", 1);
	}

	if (config.cursor_theme) {
		setenv("XCURSOR_THEME", config.cursor_theme, 1);
	}
}

void reapply_cursor_style(void) {
	if (hide_cursor_source) {
		wl_event_source_timer_update(hide_cursor_source, 0);
		wl_event_source_remove(hide_cursor_source);
		hide_cursor_source = NULL;
	}

	wlr_cursor_unset_image(cursor);

	wlr_cursor_set_surface(cursor, NULL, 0, 0);

	if (cursor_mgr) {
		wlr_xcursor_manager_destroy(cursor_mgr);
		cursor_mgr = NULL;
	}

	set_xcursor_env();

	cursor_mgr =
		wlr_xcursor_manager_create(config.cursor_theme, config.cursor_size);

	Monitor *m = NULL;
	wl_list_for_each(m, &mons, link) {
		wlr_xcursor_manager_load(cursor_mgr, m->wlr_output->scale);
	}

	wlr_cursor_set_xcursor(cursor, cursor_mgr, "left_ptr");

	hide_cursor_source = wl_event_loop_add_timer(wl_display_get_event_loop(dpy),
												 hidecursor, cursor);
	if (cursor_hidden) {
		wlr_cursor_unset_image(cursor);
	} else {
		wl_event_source_timer_update(hide_cursor_source,
									 config.cursor_hide_timeout * 1000);
	}
}

void reapply_rootbg(void) {
	wlr_scene_rect_set_color(root_bg, config.rootcolor);
}

void reapply_property(void) {
	Client *c = NULL;

	// reset border width when config change
	wl_list_for_each(c, &clients, link) {
		if (c && !c->iskilling) {
			if (!c->isnoborder && !c->isfullscreen) {
				c->bw = config.borderpx;
			}

			wlr_scene_rect_set_color(c->droparea, config.dropcolor);
		}
	}
}

void reapply_keyboard(void) {
	InputDevice *id;
	wl_list_for_each(id, &inputdevices, link) {
		if (id->wlr_device->type != WLR_INPUT_DEVICE_KEYBOARD) {
			continue;
		}
		wlr_keyboard_set_repeat_info((struct wlr_keyboard *)id->device_data,
									 config.repeat_rate, config.repeat_delay);
	}
}

void reapply_pointer(void) {
	InputDevice *id;
	struct libinput_device *device;
	wl_list_for_each(id, &inputdevices, link) {

		if (id->wlr_device->type != WLR_INPUT_DEVICE_POINTER) {
			continue;
		}

		device = id->libinput_device;
		if (wlr_input_device_is_libinput(id->wlr_device) && device) {
			configure_pointer(device);
		}
	}
}

void reapply_master(void) {

	int32_t i;
	Monitor *m = NULL;
	for (i = 0; i <= LENGTH(tags); i++) {
		wl_list_for_each(m, &mons, link) {
			if (!m->wlr_output->enabled) {
				continue;
			}
			m->pertag->nmasters[i] = config.default_nmaster;
			m->pertag->mfacts[i] = config.default_mfact;
			m->gappih = config.gappih;
			m->gappiv = config.gappiv;
			m->gappoh = config.gappoh;
			m->gappov = config.gappov;
		}
	}
}

void parse_tagrule(Monitor *m) {
	int32_t i, jk;
	ConfigTagRule tr;
	Client *c = NULL;
	bool match_rule = false;

	for (i = 0; i <= LENGTH(tags); i++) {
		m->pertag->nmasters[i] = config.default_nmaster;
		m->pertag->mfacts[i] = config.default_mfact;
	}

	for (i = 0; i < config.tag_rules_count; i++) {

		tr = config.tag_rules[i];

		match_rule = true;

		if (tr.monitor_name != NULL) {
			if (!regex_match(tr.monitor_name, m->wlr_output->name)) {
				match_rule = false;
			}
		}

		if (tr.monitor_make != NULL) {
			if (m->wlr_output->make == NULL ||
				strcmp(tr.monitor_make, m->wlr_output->make) != 0) {
				match_rule = false;
			}
		}

		if (tr.monitor_model != NULL) {
			if (m->wlr_output->model == NULL ||
				strcmp(tr.monitor_model, m->wlr_output->model) != 0) {
				match_rule = false;
			}
		}

		if (tr.monitor_serial != NULL) {
			if (m->wlr_output->serial == NULL ||
				strcmp(tr.monitor_serial, m->wlr_output->serial) != 0) {
				match_rule = false;
			}
		}

		if (config.tag_rules_count > 0 && match_rule) {

			for (jk = 0; jk < LENGTH(layouts); jk++) {
				if (tr.layout_name &&
					strcmp(layouts[jk].name, tr.layout_name) == 0) {
					m->pertag->ltidxs[tr.id] = &layouts[jk];
				}
			}

			if (tr.no_hide >= 0)
				m->pertag->no_hide[tr.id] = tr.no_hide;
			if (tr.nmaster >= 1)
				m->pertag->nmasters[tr.id] = tr.nmaster;
			if (tr.mfact > 0.0f)
				m->pertag->mfacts[tr.id] = tr.mfact;
			if (tr.no_render_border >= 0)
				m->pertag->no_render_border[tr.id] = tr.no_render_border;
			if (tr.open_as_floating >= 0)
				m->pertag->open_as_floating[tr.id] = tr.open_as_floating;
		}
	}

	for (i = 1; i <= LENGTH(tags); i++) {
		wl_list_for_each(c, &clients, link) {
			if ((c->tags & (1 << (i - 1)) & TAGMASK) && ISTILED(c)) {
				if (m->pertag->mfacts[i] > 0.0f)
					c->master_mfact_per = m->pertag->mfacts[i];
			}
		}
	}
}

void reapply_tagrule(void) {
	Monitor *m = NULL;
	wl_list_for_each(m, &mons, link) {
		if (!m->wlr_output->enabled) {
			continue;
		}
		parse_tagrule(m);
	}
}

void reset_option(void) {
	init_baked_points();
	handlecursoractivity();
	reset_keyboard_layout();
	reset_blur_params();
	set_env();
	run_exec();

	reapply_cursor_style();
	reapply_property();
	reapply_rootbg();
	reapply_keyboard();
	reapply_pointer();
	reapply_master();

	reapply_tagrule();
	reapply_monitor_rules();

	arrange(selmon, false, false);
}

int32_t reload_config(const Arg *arg) {
	parse_config();
	reset_option();
	printstatus();
	return 1;
}

#endif /* CONFIG_REAPPLY_H */
