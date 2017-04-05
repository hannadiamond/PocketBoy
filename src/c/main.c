#include <pebble.h>
#include "main.h"
#include <pebble-events/pebble-events.h>


static Window *s_main_window;

//Pocketboy image
static GBitmap *s_terminal_bitmap;
static BitmapLayer *s_bitmap_layer;
//Bluetooth image
static GBitmap *bl_terminal_bitmap;
static BitmapLayer *bl_bitmap_layer;

static GFont s_font36;
static GFont s_font28;
static GFont s_font18;
static GFont bl_font18;

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_connection_layer;
static TextLayer *s_step_layer;

static char PersistDisconnectVibrate[] = "0";  //<================
static char PersistDate[] = "0";  //<================
static char PersistPocketBoyType[] = "0";  //<================


//settings
// A struct for our specific settings (see main.h)
ClaySettings settings;

// Initialize the default settings
static void prv_default_settings() {
  settings.BackgroundColor = GColorBlack;
  settings.TextColor = GColorWhite;
  strcpy(PersistDisconnectVibrate, "0");
  strcpy(PersistDisconnectVibrate, "0");
  strcpy(PersistPocketBoyType,  "0");              //<================
  
 
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
  if(persist_exists(MESSAGE_KEY_PocketBoyType)) {                        //<================
     persist_read_string(MESSAGE_KEY_PocketBoyType, PersistPocketBoyType, sizeof(PersistPocketBoyType));
      APP_LOG(APP_LOG_LEVEL_INFO, "In Load Settings : PersistPocketBoyType = %s", PersistPocketBoyType);
   }
   if(persist_exists(MESSAGE_KEY_DisconnectVibrate)) {                        //<================
     persist_read_string(MESSAGE_KEY_DisconnectVibrate, PersistDisconnectVibrate, sizeof(PersistDisconnectVibrate));
   }
  if(persist_exists(MESSAGE_KEY_Date)) {                        //<================
     persist_read_string(MESSAGE_KEY_Date, PersistDate, sizeof(PersistDate));
   }
}


// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  persist_write_string(MESSAGE_KEY_PocketBoyType, PersistPocketBoyType);  //<================
  persist_write_string(MESSAGE_KEY_DisconnectVibrate, PersistDisconnectVibrate);  //<================
  persist_write_string(MESSAGE_KEY_Date, PersistDate);  //<================
  // Update the display based on new settings
  prv_update_display();
}

static void background(){
  gbitmap_destroy(s_terminal_bitmap);
  if(strcmp(PersistPocketBoyType, "0")==0){
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GameBoyClassic_Image);
  } else if(strcmp(PersistPocketBoyType, "1") == 1) {
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GameBoyNES_Image);
  }else if(strcmp(PersistPocketBoyType, "2") == 2) {
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GameBoySNES_Image);
  }else if(strcmp(PersistPocketBoyType, "3") == 3) {
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GameBoyGameCube_Image);
  } else if(strcmp(PersistPocketBoyType, "4") == 4) {
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GameBoyWii_Image);
  }else if(strcmp(PersistPocketBoyType, "5") == 5) {
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GameBoySwitch_Image);
  }else if(strcmp(PersistPocketBoyType, "6") == 6) {
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GameBoyFamicom_Image);
  }else if(strcmp(PersistPocketBoyType, "7") == 7) {
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GameBoyXboxBlack_Image);
  }else if(strcmp(PersistPocketBoyType, "8") == 8) {
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GameBoyXboxWhite_Image);
  }else if(strcmp(PersistPocketBoyType, "9") == 9) {
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GameBoyPlaystation_Image);
  }
  bitmap_layer_set_bitmap(s_bitmap_layer, s_terminal_bitmap);
}

// Update the display elements
static void prv_update_display() {
  // Background color
  window_set_background_color(s_main_window, settings.BackgroundColor); 
  background();

  // Text Color
  text_layer_set_text_color(s_time_layer, settings.TextColor);
  text_layer_set_text_color(s_date_layer, settings.TextColor);
  text_layer_set_text_color(s_battery_layer, settings.TextColor);
  text_layer_set_text_color(s_connection_layer, settings.TextColor);
  text_layer_set_text_color(s_step_layer, settings.TextColor); 
  
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[10];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
 
  // Display this time on the TextLayer
  clock_copy_time_string(s_buffer,sizeof(s_buffer));
  text_layer_set_text(s_time_layer, s_buffer);
  
  // Show the date
  if(strcmp(PersistDate, "0") == 0) {
    static char date_buffer[40];
    strftime(date_buffer, sizeof(date_buffer), "%a %b %d", tick_time);
    text_layer_set_text(s_date_layer, date_buffer);
  } else if(strcmp(PersistDate, "1")== 1) {
    static char date_buffer[40];
    strftime(date_buffer, sizeof(date_buffer), "%b %d, %Y", tick_time );
    text_layer_set_text(s_date_layer, date_buffer);    
  } else if(strcmp(PersistDate, "2") == 2) {
    static char date_buffer[40];
    strftime(date_buffer, sizeof(date_buffer), "%D", tick_time );
    text_layer_set_text(s_date_layer, date_buffer);  
  } else if (strcmp(PersistDate, "3") == 3) {
    static char date_buffer[40];
    strftime(date_buffer, sizeof(date_buffer), "%a %D", tick_time );
    text_layer_set_text(s_date_layer, date_buffer);  
  } else if (strcmp(PersistDate, "4") == 4) {
    static char date_buffer[40];
    strftime(date_buffer, sizeof(date_buffer), "%a %D", tick_time );
    text_layer_set_text(s_date_layer, date_buffer);  
  }
}


// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  //PocketBoy Image Type
  Tuple *type_select_t = dict_find(iter, MESSAGE_KEY_PocketBoyType);
  	if(type_select_t) { 
     strcpy(PersistPocketBoyType, type_select_t->value->cstring); //<================
  } else {
     strcpy(PersistPocketBoyType, "0"); //<================
  }
    APP_LOG(APP_LOG_LEVEL_INFO, "In InboxReceived: PersistPocketBoy = %s", PersistPocketBoyType);

  // Background Color
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (bg_color_t) {
    settings.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
  }
  // Text Color
  Tuple *fg_color_t = dict_find(iter, MESSAGE_KEY_TextColor);
  if (fg_color_t) {
    settings.TextColor = GColorFromHEX(fg_color_t->value->int32);
  }
   // Disconnect Vibrations
  Tuple *disconnect_vibratate_t = dict_find(iter, MESSAGE_KEY_DisconnectVibrate);
  if (disconnect_vibratate_t) {
    strcpy(PersistDisconnectVibrate, disconnect_vibratate_t->value->cstring);
  }else {
     strcpy(PersistDisconnectVibrate, "0"); //<================
  }
  //Date
  Tuple *bp_select_t = dict_find(iter, MESSAGE_KEY_Date);
	if(bp_select_t) { 
    strcpy(PersistDate, bp_select_t->value->cstring); 
  }else {
     strcpy(PersistDate, "0"); //<================
  }
  // Save the new settings to persistent storage
  prv_save_settings();
}

static void get_step_count() {
static char s_current_steps_buffer[20];
static int s_step_count = 0;
s_step_count = (int)health_service_sum_today(HealthMetricStepCount);
int thousands = s_step_count/1000;;
int hundreds = s_step_count-(thousands*1000);
  if(thousands > 0) {
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),"%d,%d", thousands, hundreds);
  } else {
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),"%d", s_step_count);
  }
  text_layer_set_text(s_step_layer, s_current_steps_buffer);
}

// pebble health handler
static void health_handler(HealthEventType event, void *context) {
  if(event != HealthEventSleepUpdate) {
    get_step_count();
  }
}


//pebble time handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  prv_update_display();
}

//battery 
static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";
  if (charge_state.is_charging) {
    text_layer_set_text(s_battery_layer,"5");
    text_layer_set_font(s_battery_layer, bl_font18);
  } else {
    snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
    text_layer_set_text(s_battery_layer, battery_text);
    text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  }
  
}

//bluetooth
static void handle_bluetooth(bool connected) {
  text_layer_set_text(s_connection_layer, connected ? "B": "");
  text_layer_set_font(s_connection_layer, bl_font18);
 
}

static void bluetooth_callback(bool connected) {
	if(!connected) {
			if(strcmp(PersistDisconnectVibrate, "0") == 0) {
      }								// No vibration 
			else if(strcmp(PersistDisconnectVibrate, "1") == 1) {
        vibes_short_pulse(); 
      }		// Short vibration
			else if(strcmp(PersistDisconnectVibrate, "2") == 2) {
        vibes_long_pulse(); 
      }		// Long vibration
			else if(strcmp(PersistDisconnectVibrate, "3") == 3) { 
        vibes_double_pulse(); 
      }	// Double vibration					
	} 
}


static void prv_main_window_load(Window *window) {

  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_font36 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Pixel_Font_36));
  s_font28 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Pixel_Font_28)); 
  s_font18 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Pixel_Font_18));
  
  bl_font18 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Pyrite_Font_20));
  
  window_set_background_color(s_main_window, settings.BackgroundColor);
   
  //Image
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GameBoyClassic_Image);
  s_bitmap_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_terminal_bitmap);
  
  //bluetooth vibration
  bluetooth_callback(connection_service_peek_pebble_app_connection()); 
  
  //time
  s_time_layer = text_layer_create( GRect(0, 35, bounds.size.w, 45));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  //date
  s_date_layer = text_layer_create(GRect(0, 80, bounds.size.w, 25));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text(s_date_layer, "Wednesday September /1111");

  //battery 
  s_battery_layer = text_layer_create(GRect(15, 12, bounds.size.w, 20));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentLeft);
  battery_state_service_subscribe(handle_battery);
  handle_battery(battery_state_service_peek());
  
  //bluetooth  
  s_connection_layer = text_layer_create(GRect(-15, 12, bounds.size.w, 20));
  text_layer_set_background_color(s_connection_layer, GColorClear);
  text_layer_set_text_alignment(s_connection_layer, GTextAlignmentRight);
  handle_bluetooth(connection_service_peek_pebble_app_connection());
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = handle_bluetooth
  });

 
  // Subscribe to health events if we can
  health_service_events_subscribe(health_handler, NULL);
  //steps
  s_step_layer = text_layer_create(GRect(0, 12, bounds.size.w, 35));
  text_layer_set_background_color(s_step_layer, GColorClear);
  text_layer_set_text_alignment(s_step_layer, GTextAlignmentCenter);
  text_layer_set_text(s_step_layer, "0"); 

    //Fonts
    if (clock_is_24h_style()){
       text_layer_set_font(s_time_layer, s_font36);
    } else {
      text_layer_set_font(s_time_layer, s_font28);
    };
   
  text_layer_set_font(s_date_layer, s_font18);
  text_layer_set_font(s_step_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    
  //child layers
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_connection_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_step_layer));
  
  prv_update_display();
}

static void prv_main_window_unload(Window *window) {
  
  //Destroy Images
  gbitmap_destroy(s_terminal_bitmap);
  bitmap_layer_destroy(s_bitmap_layer);
  gbitmap_destroy(bl_terminal_bitmap);
  bitmap_layer_destroy(bl_bitmap_layer);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_connection_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_step_layer);
  
  //unsubscribe
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
}

static void prv_init() {

  events_app_message_open();
  
  prv_load_settings();

  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = prv_main_window_load,
    .unload = prv_main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
 
  // Register with TickTimerService
tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  
  // Declare a file-scope variable
  prv_update_display();
  

}

static void prv_deinit() {
  // Destroy Window
  window_destroy(s_main_window);

}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
