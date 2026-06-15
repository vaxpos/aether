#ifndef CONFIG_EXEC_H
#define CONFIG_EXEC_H

#include "config_input.h"

void set_env() {
	for (int32_t i = 0; i < config.env_count; i++) {
		setenv(config.env[i]->type, config.env[i]->value, 1);
	}
}

void run_exec() {
	Arg arg;

	for (int32_t i = 0; i < config.exec_count; i++) {
		arg.v = config.exec[i];
		spawn_shell(&arg);
	}
}

void run_exec_once() {
	Arg arg;

	for (int32_t i = 0; i < config.exec_once_count; i++) {
		arg.v = config.exec_once[i];
		spawn_shell(&arg);
	}
}

#endif /* CONFIG_EXEC_H */
