#include <pebble.h>

#define MARGIN 3

static Window* s_main_window;
static TextLayer* s_time_layer;
static TextLayer* s_date_layer;
static TextLayer* s_step_layer;
static TextLayer* s_battery_layer;
static TextLayer* s_bt_layer;
static GFont s_time_font;


/* Function to update the time layer with the current time */
static void update_time() {
	/* Get a tm structure */
	time_t temp = time(NULL);
	struct tm* tick_time = localtime(&temp);

	/* Write the current hours and minutes into a buffer */
	static char s_buffer[8];
	strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

	/* Display this time on the TextLayer */
	text_layer_set_text(s_time_layer, s_buffer);
}

/* Function to update the date layer with the current date */
static void update_date() {
	/* Get a tm structure */
	time_t temp = time(NULL);
	struct tm* tick_time = localtime(&temp);

	/* Write the current hours and minutes into a buffer */
	static char s_date_buffer[16];
	strftime(s_date_buffer, sizeof(s_date_buffer), "%a, %d %b", tick_time);

	/* Display this time on the TextLayer */
	text_layer_set_text(s_date_layer, s_date_buffer);
}

/* Function to update the step layer with the current steps from Pebble Health */
static void update_step() {
	int step_count;
	static char s_step_buffer[16];

	step_count = (int)health_service_sum_today(HealthMetricStepCount);

	/* Write the current hours and minutes into a buffer */
	snprintf(s_step_buffer, sizeof(s_step_buffer), "%d", step_count);

	/* Display this time on the TextLayer */
	text_layer_set_text(s_step_layer, s_step_buffer);
}

/* Function called every minute by TickTimerService */
static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
	update_time();
}

/* Function called once a day to update the date */
static void date_handler(struct tm* tick_date, TimeUnits units_changed) {
	update_date();
}

/* Function called when battery state changes */
static void battery_handler() {
	BatteryChargeState battery_info;
	static char s_battery[8];

	battery_info = battery_state_service_peek();

	if(battery_info.is_charging) {
		snprintf(s_battery, sizeof(s_battery), "CHR");
	}
	else {
		snprintf(s_battery, sizeof(s_battery), "%d%%", battery_info.charge_percent);
	}

	/* Display this time on the TextLayer */
	text_layer_set_text(s_battery_layer, s_battery);
}

/* Function called when bluetooth connection statud changes */
static void bt_handler(bool connected) {
	bool bt_connected;
	static char s_bt[8];

	bt_connected = connected;

	if(!bt_connected) {
		snprintf(s_bt, sizeof(s_bt), "BT");
		vibes_short_pulse();
	}
	else {
		snprintf(s_bt, sizeof(s_bt), "  ");
	}

	text_layer_set_text(s_bt_layer, s_bt);
}

static void main_window_load(Window* window) {
	/* Get information about the Window */
	Layer* window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	/* Create the TextLayer with specific bounds */
	s_time_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(58,50), bounds.size.w, 50));
	s_date_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(90,144), bounds.size.w, 24));
	s_step_layer = text_layer_create(
		GRect(MARGIN, PBL_IF_ROUND_ELSE(90,0), bounds.size.w*0.33-MARGIN-1, 30));
	s_bt_layer = text_layer_create(
		GRect(bounds.size.w*0.33+2, PBL_IF_ROUND_ELSE(90,0), bounds.size.w*0.33-MARGIN, 30));
	s_battery_layer = text_layer_create(
		GRect(bounds.size.w*0.67, PBL_IF_ROUND_ELSE(90,0), bounds.size.w*0.33-MARGIN, 30));

	/* Create GFont */
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DGB_48));

	/* Layout for text layer */
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorBlack);
	text_layer_set_font(s_time_layer, s_time_font);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	/* Layout for date layer */
	text_layer_set_background_color(s_date_layer, GColorLightGray);
	text_layer_set_text_color(s_date_layer, GColorBlack);
	text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

	/* Layout for step layer */
	text_layer_set_background_color(s_step_layer, GColorLightGray);
	text_layer_set_text_color(s_step_layer, GColorBlack);
	text_layer_set_font(s_step_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_step_layer, GTextAlignmentLeft);

	/* Layout for battery layer */
	text_layer_set_background_color(s_battery_layer, GColorLightGray);
	text_layer_set_text_color(s_battery_layer, GColorBlack);
	text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);

	/* Layout for bluetooth connection layer */
	text_layer_set_background_color(s_bt_layer, GColorLightGray);
	text_layer_set_text_color(s_bt_layer, GColorBlack);
	text_layer_set_font(s_bt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_bt_layer, GTextAlignmentCenter);

	/* Add the layers as a child layers to the Window's root layer */
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_step_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_bt_layer));

}

static void main_window_unload(Window* window) {
	/* Destroy text layers */
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_step_layer);
	text_layer_destroy(s_battery_layer);
	text_layer_destroy(s_bt_layer);

	/* Destroy text font */
	fonts_unload_custom_font(s_time_font);
}

static void init() {
	/* Register with TickTimerService once a minute to change time */
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	/* Register with TickTimerService once a day to change date */
	tick_timer_service_subscribe(DAY_UNIT, date_handler);

	/* Register with BatteryStateService */
	battery_state_service_subscribe(battery_handler); 

	/* Register with ConnectionService */
	connection_service_subscribe((ConnectionHandlers) {
		.pebble_app_connection_handler = bt_handler
	});

	/* Create main window element and assign to pointer */
	s_main_window = window_create();

	/* Set handlers to manage the elements inside the window */
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	/* Show the Window on the watch, with animated=true */
	window_stack_push(s_main_window, true);

	/* Show the current time from the beggining */
	update_time();
	update_date();
	update_step();
	battery_handler();
	bt_handler(true);
}

static void deinit() {
	window_destroy(s_main_window);
}


int main(void) {
	init();
	app_event_loop();
	deinit();
}
