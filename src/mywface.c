#include <pebble.h>

static Window* s_main_window;
static TextLayer* s_time_layer;
static TextLayer* s_date_layer;
static TextLayer* s_step_layer;

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
	//step_count = 1042;
	/* Write the current hours and minutes into a buffer */
	snprintf(s_step_buffer, sizeof(s_step_buffer), "%d", step_count);

	/* Display this time on the TextLayer */
	text_layer_set_text(s_step_layer, s_step_buffer);
}

/* Function called every minute by TickTimerService */
static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
	update_time();
	update_date();
}


static void main_window_load(Window* window) {
	/* Get information about the Window */
	Layer* window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	/* Create the TextLayer with specific bounds */
	s_time_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(58,50), bounds.size.w, 50));
	s_date_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(90,100), bounds.size.w, 30));
	s_step_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(90,130), bounds.size.w, 30));

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
	text_layer_set_text_alignment(s_step_layer, GTextAlignmentCenter);


	/* Add it as a child layer to the Window's root layer */
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_step_layer));

}

static void main_window_unload(Window* window) {
	/* Destroy text layers */
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_step_layer);

	/* Destroy text font */
	fonts_unload_custom_font(s_time_font);
}

static void init() {
	/* Register with TickTimerService */
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

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
}

static void deinit() {
	window_destroy(s_main_window);
}


int main(void) {
	init();
	app_event_loop();
	deinit();
}
