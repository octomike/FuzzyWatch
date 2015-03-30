#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_hours_layer, *s_debug_layer, *s_fuzzy_layer, *s_dates_layer;
static char *s_fuzzy_buf, *s_hours_buf, *s_dates_buf, *s_debug_buf;
static int s_buflen=32, s_fuzzy_idx, s_tmp, s_hour_offset = 0;

static char* s_hours_text[12] = {"Eins", "Zwei", "Drei", "Vier", "Fünf", "Sechs",
                                 "Sieben", "Acht", "Neun", "Zehn", "Elf", "Zwölf" };
static char* s_fuzzy_text[] = { "", "kurz nach", "viertel", "kurz vor halb",
                                "halb", "kurz nach halb", "dreiviertel", "kurz vor" };

static void time_handler(struct tm *tick_time, TimeUnits units_changed){
  // conserve some energy and only update once a minute
  tick_timer_service_subscribe(MINUTE_UNIT, time_handler);
  
  // Minutes
  strftime(s_fuzzy_buf, s_buflen, "%M", tick_time);
  s_tmp = atoi(s_fuzzy_buf) + 3;
  s_hour_offset = 0;
  if( s_tmp >= 0 && s_tmp <= 6 ){
    s_fuzzy_idx = 0;
    s_hour_offset = 1;
  }
  else if( s_tmp >=  7 && s_tmp <= 14 ){
    s_fuzzy_idx = 1;
    s_hour_offset = 1;
  }
  else if( s_tmp >= 15 && s_tmp <= 21 )
    s_fuzzy_idx = 2;
  else if( s_tmp >= 22 && s_tmp <= 29 )
    s_fuzzy_idx = 3;
  else if( s_tmp >= 30 && s_tmp <= 36 )
    s_fuzzy_idx = 4;
  else if( s_tmp >= 37 && s_tmp <= 44 )
    s_fuzzy_idx = 5;
  else if( s_tmp >= 45 && s_tmp <= 51 )
    s_fuzzy_idx = 6;
  else
    s_fuzzy_idx = 7;
  text_layer_set_text(s_fuzzy_layer, s_fuzzy_text[s_fuzzy_idx]);
  
  // Hours
  strftime(s_hours_buf, s_buflen, "%I", tick_time);
  text_layer_set_text(s_hours_layer,
                      s_hours_text[ (atoi(s_hours_buf) + 12 - s_hour_offset)%12 ]);
  
  // Date
  strftime(s_dates_buf, s_buflen, "%A  |  %e. %B", tick_time);
  text_layer_set_text(s_dates_layer, s_dates_buf);
}

static void main_window_load(Window *window){
  // hour layer
  s_hours_layer = text_layer_create(GRect(0,58,130,50));
  text_layer_set_background_color(s_hours_layer, GColorBlack);
  text_layer_set_text_color(s_hours_layer, GColorJazzberryJam);
  text_layer_set_font(s_hours_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(s_hours_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hours_layer));
  
  // fuzzy layer
  s_fuzzy_layer = text_layer_create(GRect(10,38,144,28));
  text_layer_set_background_color(s_fuzzy_layer, GColorBlack);
  text_layer_set_text_color(s_fuzzy_layer, GColorJazzberryJam);
  text_layer_set_font(s_fuzzy_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_fuzzy_layer, GTextAlignmentLeft);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_fuzzy_layer));
  
  // dates layer
  s_dates_layer = text_layer_create(GRect(0,150,144,16));
  text_layer_set_background_color(s_dates_layer, GColorBlack);
  text_layer_set_text_color(s_dates_layer, GColorJazzberryJam);
  text_layer_set_font(s_dates_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_dates_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_dates_layer));
  
  // debug text layer at the top
  s_debug_layer = text_layer_create(GRect(0,0,144,20));
  text_layer_set_background_color(s_debug_layer, GColorBlack);
  text_layer_set_text_color(s_debug_layer, GColorYellow);
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
  window_set_background_color(s_main_window, GColorBlack);
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