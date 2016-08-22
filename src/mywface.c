#include <pebble.h>

#define MARGIN 3
#define COLOR_FRONT GColorWhite
#define COLOR_BACK  GColorBlack
#define COLOR_DEBUG GColorLightGray

/* Leave for production code. Comment BOTH for debugging */
#undef APP_LOG
#define APP_LOG(...) 

static Window* s_main_window;
static TextLayer* s_hour_layer;
static TextLayer* s_minute_layer;
static TextLayer* s_date_layer;
static TextLayer* s_step_layer;
static TextLayer* s_battery_layer;
static TextLayer* s_bt_layer;
static GFont s_time_font;


/* Function to update the time layer with the current time.
 * TODO: Implement update_hour and update_minute to save battery */
static void update_time() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering update_time");
	/* Get a tm structure */
	time_t temp = time(NULL);
	struct tm* tick_time = localtime(&temp);

	/* Write the current hours and minutes into a buffer */
	static char s_hour[8];
	static char s_minute[8];
	strftime(s_hour, sizeof(s_hour), clock_is_24h_style() ? "%H" : "%I", tick_time);
	strftime(s_minute, sizeof(s_minute), "%M", tick_time);

	/* Display this time on the TextLayer */
	text_layer_set_text(s_hour_layer, s_hour);
	text_layer_set_text(s_minute_layer, s_minute);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting update_time");
}

/* Function to update the date layer with the current date */
static void update_date() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering update_date");
	/* Get a tm structure */
	time_t temp = time(NULL);
	struct tm* tick_time = localtime(&temp);

	/* Write the current date into a buffer */
	static char s_date_buffer[16];
	strftime(s_date_buffer, sizeof(s_date_buffer), "%a, %d %b", tick_time);

	/* Display date on the TextLayer */
	text_layer_set_text(s_date_layer, s_date_buffer);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting update_date");
}

/* Function to update the step layer with the current steps from Pebble Health.
 * TODO: Check if Pebble Health is enabled */
static void update_step() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering update_step");
	int step_count;
	static char s_step_buffer[16];

	step_count = (int)health_service_sum_today(HealthMetricStepCount);

	/* Write the current steps into a buffer */
	snprintf(s_step_buffer, sizeof(s_step_buffer), "%d", step_count);

	/* Display steps on the TextLayer */
	text_layer_set_text(s_step_layer, s_step_buffer);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting update_step");
}

/* Function called every minute by TickTimerService */
static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
	if(units_changed & MINUTE_UNIT){
		update_time();
		update_step();
	}
	if(units_changed & DAY_UNIT){
		update_date();
	}
}

/* Function called when battery state changes */
static void battery_handler() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering battery_handler");
	BatteryChargeState battery_info;
	static char s_battery[8];

	battery_info = battery_state_service_peek();

	if(battery_info.is_charging) {
		snprintf(s_battery, sizeof(s_battery), "CHR");
	}
	else {
		snprintf(s_battery, sizeof(s_battery), "%d%%", battery_info.charge_percent);
	}

	/* Display battery status on the TextLayer */
	text_layer_set_text(s_battery_layer, s_battery);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting battery_handler");
}

/* Function called when bluetooth connection status changes */
static void bt_handler(bool connected) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering bt_handler");
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

	/* Display connection status on the TextLayer */
	text_layer_set_text(s_bt_layer, s_bt);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting bt_handler");
}

static void main_window_load(Window* window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering main_window_load");
	/* Get information about the Window */
	Layer* window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	/* Create the TextLayer with specific bounds. Only valid for Pebble/Pebble Time */
	s_hour_layer = text_layer_create(
		GRect(0, 22, bounds.size.w, 64));
	s_minute_layer = text_layer_create(
		GRect(0, 74, bounds.size.w, 64));
	s_date_layer = text_layer_create(
		GRect(0, 144, bounds.size.w, 24));
	s_step_layer = text_layer_create(
		GRect(MARGIN, 0, bounds.size.w*0.33-MARGIN, 30));
	s_bt_layer = text_layer_create(
		GRect(bounds.size.w*0.33+2, 0, bounds.size.w*0.33-MARGIN, 30));
	s_battery_layer = text_layer_create(
		GRect(bounds.size.w*0.67, 0, bounds.size.w*0.33-MARGIN, 30));

	/* Create GFont */
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SM_62));

	/* Main window */
	window_set_background_color(window, COLOR_BACK);

	/* Layout for hour layer */
	text_layer_set_background_color(s_hour_layer, COLOR_BACK);
	text_layer_set_text_color(s_hour_layer, COLOR_FRONT);
	text_layer_set_font(s_hour_layer, s_time_font);
	text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);

	/* Layout for minute layer */
	text_layer_set_background_color(s_minute_layer, COLOR_BACK);
	text_layer_set_text_color(s_minute_layer, COLOR_FRONT);
	text_layer_set_font(s_minute_layer, s_time_font);
	text_layer_set_text_alignment(s_minute_layer, GTextAlignmentCenter);

	/* Layout for date layer */
	text_layer_set_background_color(s_date_layer, COLOR_BACK);
	text_layer_set_text_color(s_date_layer, COLOR_FRONT);
	text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

	/* Layout for step layer */
	text_layer_set_background_color(s_step_layer, COLOR_BACK);
	text_layer_set_text_color(s_step_layer, COLOR_FRONT);
	text_layer_set_font(s_step_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_step_layer, GTextAlignmentLeft);

	/* Layout for battery layer */
	text_layer_set_background_color(s_battery_layer, COLOR_BACK);
	text_layer_set_text_color(s_battery_layer, COLOR_FRONT);
	text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);

	/* Layout for bluetooth connection layer */
	text_layer_set_background_color(s_bt_layer, COLOR_BACK);
	text_layer_set_text_color(s_bt_layer, COLOR_FRONT);
	text_layer_set_font(s_bt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_bt_layer, GTextAlignmentCenter);

	/* Add the layers as a child layers to the Window's root layer. 
	 * The order here is important to control clipping */
	layer_add_child(window_layer, text_layer_get_layer(s_minute_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_hour_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_step_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_bt_layer));

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting main_window_load");
}

static void main_window_unload(Window* window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering main_window_unload");
	/* Destroy text layers on unload */
	text_layer_destroy(s_hour_layer);
	text_layer_destroy(s_minute_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_step_layer);
	text_layer_destroy(s_battery_layer);
	text_layer_destroy(s_bt_layer);

	/* Destroy text font */
	fonts_unload_custom_font(s_time_font);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting main_window_unload");
}

static void init() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering init");
	/* Register with TickTimerService once a minute to change time. 
	 * IMPORTANT: Apparently ONLY ONE TickTimerService can be active
	 * at a time. Newer calls to this function will overwrite this
	 * listener! */
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

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

	/* Show the current data from the beggining */
	update_time();
	update_date();
	update_step();
	battery_handler();
	bt_handler(connection_service_peek_pebble_app_connection());
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting init");
}

static void deinit() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering deinit");
	window_destroy(s_main_window);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting deinit");
}


int main(void) {
	init();
	app_event_loop();
	deinit();
}
