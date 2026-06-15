/*
 * parse_config.h — Master config include
 *
 * This file is intentionally kept thin. All implementation lives in
 * the sub-headers below, ordered by dependency.
 *
 * Dependency chain (each file includes the one above it):
 *
 *   config_types.h    — structs, enums, macros, global declarations
 *   config_utils.h    — trim_whitespace, parse_color, sanitize_string, …
 *   config_input.h    — parse_mod, parse_key, parse_func_name, …
 *   config_exec.h     — set_env, run_exec, run_exec_once
 *   config_parse.h    — parse_option (large), parse_config_line/file
 *   config_free.h     — free_config, free_baked_points, free_circle_layout
 *   config_defaults.h — override_config, set_value_default, parse_config
 *   config_reapply.h  — reapply_*, reset_option, reload_config
 */

#ifndef PARSE_CONFIG_H
#define PARSE_CONFIG_H

#include "config_types.h"
#include "config_utils.h"
#include "config_input.h"
#include "config_exec.h"
#include "config_parse.h"
#include "config_free.h"
#include "config_defaults.h"
#include "config_reapply.h"

#endif /* PARSE_CONFIG_H */
