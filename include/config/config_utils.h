#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

#include "config_types.h"

// Helper function to trim whitespace from start and end of a string
void trim_whitespace(char *str) {
	if (str == NULL || *str == '\0')
		return;

	// Trim leading space
	char *start = str;
	while (isspace((unsigned char)*start)) {
		start++;
	}

	// Trim trailing space
	char *end = str + strlen(str) - 1;
	while (end > start && isspace((unsigned char)*end)) {
		end--;
	}

	// Null-terminate the trimmed string
	*(end + 1) = '\0';

	// Move the trimmed part to the beginning if needed
	if (start != str) {
		memmove(str, start, end - start + 2); // +2 to include null terminator
	}
}

int32_t parse_double_array(const char *input, double *output,
						   int32_t max_count) {
	char *dup = strdup(input);
	char *token;
	int32_t count = 0;

	// 先清空整个数组
	memset(output, 0, max_count * sizeof(double));

	token = strtok(dup, ",");
	while (token != NULL && count < max_count) {
		trim_whitespace(token);
		char *endptr;
		double val = strtod(token, &endptr);
		if (endptr == token || *endptr != '\0') {
			fprintf(
				stderr,
				"\033[1m\033[31m[ERROR]:\033[33m Invalid number in array: %s\n",
				token);
			free(dup);
			return -1;
		}
		output[count] = val; // 赋值到当前count位置
		count++;			 // 然后才自增
		token = strtok(NULL, ",");
	}

	free(dup);
	return count;
}

// 清理字符串中的不可见字符（包括 \r, \n, 空格等）
char *sanitize_string(char *str) {
	// 去除首部不可见字符
	while (*str != '\0' && !isprint((unsigned char)*str))
		str++;
	// 去除尾部不可见字符
	char *end = str + strlen(str) - 1;
	while (end > str && !isprint((unsigned char)*end))
		end--;
	*(end + 1) = '\0';
	return str;
}

// 解析bind组合字符串
void parse_bind_flags(const char *str, KeyBinding *kb) {

	// 检查是否以"bind"开头
	if (strncmp(str, "bind", 4) != 0) {
		return;
	}

	const char *suffix = str + 4; // 跳过"bind"

	// 遍历后缀字符
	for (int32_t i = 0; suffix[i] != '\0'; i++) {
		switch (suffix[i]) {
		case 's':
			kb->keysymcode.type = KEY_TYPE_SYM;
			break;
		case 'l':
			kb->islockapply = true;
			break;
		case 'r':
			kb->isreleaseapply = true;
			break;
		case 'p':
			kb->ispassapply = true;
			break;
		default:
			fprintf(stderr,
					"\033[1m\033[31m[ERROR]:\033[33m Unknown bind flag: %c\n",
					suffix[i]);
			break;
		}
	}
}

int32_t parse_circle_direction(const char *str) {
	// 将输入字符串转换为小写
	char lowerStr[10];
	int32_t i = 0;
	while (str[i] && i < 9) {
		lowerStr[i] = tolower(str[i]);
		i++;
	}
	lowerStr[i] = '\0';

	// 根据转换后的小写字符串返回对应的枚举值
	if (strcmp(lowerStr, "next") == 0) {
		return NEXT;
	} else {
		return PREV;
	}
}

int32_t parse_direction(const char *str) {
	// 将输入字符串转换为小写
	char lowerStr[10];
	int32_t i = 0;
	while (str[i] && i < 9) {
		lowerStr[i] = tolower(str[i]);
		i++;
	}
	lowerStr[i] = '\0';

	// 根据转换后的小写字符串返回对应的枚举值
	if (strcmp(lowerStr, "up") == 0) {
		return UP;
	} else if (strcmp(lowerStr, "down") == 0) {
		return DOWN;
	} else if (strcmp(lowerStr, "left") == 0) {
		return LEFT;
	} else if (strcmp(lowerStr, "right") == 0) {
		return RIGHT;
	} else {
		return UNDIR;
	}
}

int32_t parse_fold_state(const char *str) {
	// 将输入字符串转换为小写
	char lowerStr[10];
	int32_t i = 0;
	while (str[i] && i < 9) {
		lowerStr[i] = tolower(str[i]);
		i++;
	}
	lowerStr[i] = '\0';

	// 根据转换后的小写字符串返回对应的枚举值
	if (strcmp(lowerStr, "fold") == 0) {
		return FOLD;
	} else if (strcmp(lowerStr, "unfold") == 0) {
		return UNFOLD;
	} else {
		return INVALIDFOLD;
	}
}
int64_t parse_color(const char *hex_str) {
	char *endptr;
	int64_t hex_num = strtol(hex_str, &endptr, 16);
	if (*endptr != '\0') {
		return -1;
	}
	return hex_num;
}

// 辅助函数：检查字符串是否以指定的前缀开头（忽略大小写）
static bool starts_with_ignore_case(const char *str, const char *prefix) {
	while (*prefix) {
		if (tolower(*str) != tolower(*prefix)) {
			return false;
		}
		str++;
		prefix++;
	}
	return true;
}

static char *combine_args_until_empty(char *values[], int count) {
	// find the first empty string
	int first_empty = count;
	for (int i = 0; i < count; i++) {
		// check if it's empty: empty string or only contains "0" (initialized)
		if (values[i][0] == '\0' ||
			(strlen(values[i]) == 1 && values[i][0] == '0')) {
			first_empty = i;
			break;
		}
	}

	// 	if there are no valid parameters, return an empty string
	if (first_empty == 0) {
		return strdup("");
	}

	// 	calculate the total length
	size_t total_len = 0;
	for (int i = 0; i < first_empty; i++) {
		total_len += strlen(values[i]);
	}
	// 	plus the number of commas (first_empty-1 commas)
	total_len += (first_empty - 1);

	// 	allocate memory and concatenate
	char *combined = malloc(total_len + 1);
	if (combined == NULL) {
		return strdup("");
	}

	combined[0] = '\0';
	for (int i = 0; i < first_empty; i++) {
		if (i > 0) {
			strcat(combined, ",");
		}
		strcat(combined, values[i]);
	}

	return combined;
}

#endif /* CONFIG_UTILS_H */
