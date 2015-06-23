#include <pebble.h>

// globals
static Window *s_main_window;
static TextLayer *s_hours_layer, *s_debug_layer, *s_fuzzy_layer, *s_dates_layer;
static char *s_fuzzy_buf, *s_hours_buf, *s_dates_buf, *s_debug_buf;
static int s_buflen=32, s_fuzzy_idx, s_tmp, s_hour_offset = 0;
static GFont s_font_custom1,s_font_custom2;

// config
#define ColorBackground GColorFolly
#define ColorForeground GColorWhite
#define ColorDebug GColorYellow
#define FontCustom1 RESOURCE_ID_FONT_ALLER_BOLD_42
#define FontCustom2 RESOURCE_ID_FONT_ALLER_30
//#define FontHours fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD)
#define FontHours s_font_custom1
//#define FontFuzzy fonts_get_system_font(FONT_KEY_GOTHIC_28)
#define FontFuzzy s_font_custom2
#define FontDates fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD)


static char* s_hours_text[12] = {"Eins", "Zwei", "Drei", "Vier", "Fünf", "Sechs",
                                 "Sieben", "Acht", "Neun", "Zehn", "Elf", "Zwölf" };
static char* s_fuzzy_text[] = { "", "kurz nach", "viertel", "kurz vor halb",
                                "halb", "kurz nach halb", "dreiviertel", "kurz vor" };



static void time_handler(struct tm *tick_time, TimeUnits units_changed){
  // conserve some energy and only update once a minute
  tick_timer_service_subscribe(MINUTE_UNIT, time_handler);
  
  // Minutes
  strftime(s_fuzzy_buf, s_buflen, "%M", tick_time);
  s_tmp = atoi(s_fuzzy_buf);
  s_hour_offset = 1; // always add one to the hour (%I) per default
  if( s_tmp >= 0 && s_tmp <= 3 ){
    s_hour_offset = 0; // 11:01 == eleven
    s_fuzzy_idx = 0;
  }
  else if( s_tmp >=  4 && s_tmp <= 11 ){
    s_hour_offset = 0; // 11:04 == a bit past eleven
    s_fuzzy_idx = 1; 
  }
  else if( s_tmp >= 12 && s_tmp <= 18 )
    s_fuzzy_idx = 2; // BUT: 11:12 == quarter twelve (in german!)
  else if( s_tmp >= 19 && s_tmp <= 26 )
    s_fuzzy_idx = 3;
  else if( s_tmp >= 27 && s_tmp <= 33 )
    s_fuzzy_idx = 4; // 11:30 == half twelve
  else if( s_tmp >= 34 && s_tmp <= 41 )
    s_fuzzy_idx = 5;
  else if( s_tmp >= 42 && s_tmp <= 48 )
    s_fuzzy_idx = 6; // 11:45 == three quarter of twelve
  else if( s_tmp >= 49 && s_tmp <= 56 )
    s_fuzzy_idx = 7;
  else {
    s_fuzzy_idx = 0; // 11:59 == (full) twelve
  }
  text_layer_set_text(s_fuzzy_layer, s_fuzzy_text[s_fuzzy_idx]);
  
  // Hours
  strftime(s_hours_buf, s_buflen, "%I", tick_time);
  text_layer_set_text(s_hours_layer,
                      s_hours_text[ (atoi(s_hours_buf) + s_hour_offset - 1)%12 ]);
  
  // Date
  strftime(s_dates_buf, s_buflen, "%A  |  %e. %B", tick_time);
  text_layer_set_text(s_dates_layer, s_dates_buf);
}

static void main_window_load(Window *window){

  // custom resources
  s_font_custom1 = fonts_load_custom_font(resource_get_handle(FontCustom1));
  s_font_custom2 = fonts_load_custom_font(resource_get_handle(FontCustom2));
  
  // hour layer
  s_hours_layer = text_layer_create(GRect(0,68,130,50));
  text_layer_set_background_color(s_hours_layer, ColorBackground);
  text_layer_set_text_color(s_hours_layer, ColorForeground);
  text_layer_set_font(s_hours_layer, s_font_custom1);
  text_layer_set_text_alignment(s_hours_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hours_layer));
  
  // fuzzy layer
  s_fuzzy_layer = text_layer_create(GRect(10,38,144,32));
  text_layer_set_background_color(s_fuzzy_layer, ColorBackground);
  text_layer_set_text_color(s_fuzzy_layer, ColorForeground);
  text_layer_set_font(s_fuzzy_layer, s_font_custom2);
  text_layer_set_text_alignment(s_fuzzy_layer, GTextAlignmentLeft);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_fuzzy_layer));
  
  // dates layer
  s_dates_layer = text_layer_create(GRect(0,150,144,20));
  text_layer_set_background_color(s_dates_layer, ColorBackground);
  text_layer_set_text_color(s_dates_layer, ColorForeground);
  text_layer_set_font(s_dates_layer, FontDates);
  text_layer_set_text_alignment(s_dates_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_dates_layer));
  
  // debug text layer at the top
  s_debug_layer = text_layer_create(GRect(0,0,144,20));
  text_layer_set_background_color(s_debug_layer, ColorBackground);
  text_layer_set_text_color(s_debug_layer, ColorDebug);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_debug_layer));

  // register time handler
  tick_timer_service_subscribe(SECOND_UNIT, time_handler);
}


static void main_window_unload(Window *window){
  text_layer_destroy(s_hours_layer);
  text_layer_destroy(s_fuzzy_layer);
  text_layer_destroy(s_debug_layer);
  text_layer_destroy(s_dates_layer);
  tick_timer_service_unsubscribe();
  fonts_unload_custom_font(s_font_custom1);
  fonts_unload_custom_font(s_font_custom2);
}

static void init(){
  // text buffers
  s_debug_buf = malloc(s_buflen);
  s_fuzzy_buf = malloc(s_buflen);
  s_hours_buf = malloc(s_buflen);
  s_dates_buf = malloc(s_buflen);
  
  // main window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = main_window_load,
      .unload = main_window_unload
  });
  window_set_background_color(s_main_window, ColorBackground);
  window_stack_push(s_main_window, true);
  
  // global stuff
  setlocale(LC_TIME, "de_DE");  // not working...
}

static void deinit(){
  window_destroy(s_main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}