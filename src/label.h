/*
 * EU-tire-label - label.h
 * Copyright (c) 2015-2021 Arkadiusz Bokowy
 *
 * This file is a part of EU-tire-label.
 *
 * This project is licensed under the terms of the MIT license.
 *
 */

enum tire_class {
	TC_ERROR = 0,
	TC_C1,
	TC_C2,
	TC_C3,
};

enum fuel_efficiency_class {
	FEC_NONE = 0,
	FEC_A,
	FEC_B,
	FEC_C,
	FEC_D,
	FEC_E,
	FEC_F,
	FEC_G,
};

enum wet_grip_class {
	WGC_NONE = 0,
	WGC_A,
	WGC_B,
	WGC_C,
	WGC_D,
	WGC_E,
	WGC_F,
	WGC_G,
};

enum rolling_noise_class {
	RNC_NONE = 0,
	RNC_1,
	RNC_2,
	RNC_3,
};

struct eu_tire_label {
	char trademark[32];
	char tire_type[32];
	char tire_size[32];
	enum tire_class tire_class;
	enum fuel_efficiency_class fuel_efficiency;
	enum wet_grip_class wet_grip;
	enum rolling_noise_class rolling_noise;
	unsigned int rolling_noise_db;
	unsigned int snow_grip;
	unsigned int ice_grip;
};

/* Create EU tire label in the SVG format according to the given tire label
 * data structure. Memory for the string is obtained with malloc(3), and can
 * be freed with free(3). */
char *create_label_EC_1222_2009(const struct eu_tire_label *data);
char *create_label_EU_2020_740(const struct eu_tire_label *data);

enum tire_class parse_tire_class(const char *str);
enum fuel_efficiency_class parse_fuel_efficiency_class(const char *str);
enum wet_grip_class parse_wet_grip_class(const char *str);
enum rolling_noise_class parse_rolling_noise_class(const char *str);
unsigned int parse_rolling_noise_db(const char *str);
