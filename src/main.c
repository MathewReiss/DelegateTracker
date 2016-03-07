#include <pebble.h>

Window *splash_window, *window;	
	
GBitmap *splash_bitmap;
BitmapLayer *splash_layer;

MenuLayer *menu_layer;

int delegates[8];
char delegate_subtitle[8][32];

enum {
	HILLARY = 0,	
	BERNIE = 1,
	UNCOMMITTED_D = 2,
	TRUMP = 3,
	CRUZ = 4,
	RUBIO = 5,
	KASICH = 6,
	UNCOMMITTED_R = 7
};

AppTimer *splash_timer;
bool data = false;

static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *t = dict_read_first(received);
	while(t != NULL){
		delegates[t->key] = atoi(t->value->cstring);
		
		if(delegates[t->key] < 1000) snprintf(delegate_subtitle[t->key], sizeof(delegate_subtitle[t->key]), "Count: %d", delegates[t->key]);
		else snprintf(delegate_subtitle[t->key], sizeof(delegate_subtitle[t->key]), "Count: %d,%03d", (int)(delegates[t->key]/1000), (int)(delegates[t->key]%1000));
		
		t = dict_read_next(received);
	}
	
	menu_layer_reload_data(menu_layer);
	
	data = true;	
}

void delay(){
	if(!data) splash_timer = app_timer_register(1000, delay, NULL);
	else{
		splash_timer = NULL;
		window_stack_push(window, true);
	}
}

void up(ClickRecognizerRef ref, void *context){
	menu_layer_set_selected_next(menu_layer, true, MenuRowAlignCenter, true);
	
	#ifdef PBL_COLOR
		if(menu_layer_get_selected_index(menu_layer).section == 0) menu_layer_set_highlight_colors(menu_layer, GColorBlue, GColorWhite);
	#endif
}

void down(ClickRecognizerRef ref, void *context){
	menu_layer_set_selected_next(menu_layer, false, MenuRowAlignCenter, true);
	
	#ifdef PBL_COLOR
		if(menu_layer_get_selected_index(menu_layer).section == 1) menu_layer_set_highlight_colors(menu_layer, GColorRed, GColorWhite);
	#endif
}

void back(ClickRecognizerRef ref, void *context){
	window_stack_pop_all(false);
}

void click_config(void *context){
	window_single_click_subscribe(BUTTON_ID_UP, up);
	window_single_click_subscribe(BUTTON_ID_DOWN, down);
	window_single_click_subscribe(BUTTON_ID_BACK, back);
}

void menu_draw_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context){
	if(cell_index->section == 0){
		switch(cell_index->row){
			case HILLARY: menu_cell_basic_draw(ctx, cell_layer, "H. Clinton", delegate_subtitle[HILLARY], NULL); break;
			case BERNIE: menu_cell_basic_draw(ctx, cell_layer, "B. Sanders", delegate_subtitle[BERNIE], NULL); break;
			case UNCOMMITTED_D: menu_cell_basic_draw(ctx, cell_layer, "Uncommitted (D)", delegate_subtitle[UNCOMMITTED_D], NULL); break;
		}
	}
	else{
		switch(cell_index->row+3){
			case TRUMP: menu_cell_basic_draw(ctx, cell_layer, "D. Trump", delegate_subtitle[TRUMP], NULL); break;
			case CRUZ: menu_cell_basic_draw(ctx, cell_layer, "T. Cruz", delegate_subtitle[CRUZ], NULL); break;
			case RUBIO: menu_cell_basic_draw(ctx, cell_layer, "M. Rubio", delegate_subtitle[RUBIO], NULL); break;
			case KASICH: menu_cell_basic_draw(ctx, cell_layer, "J. Kasich", delegate_subtitle[KASICH], NULL); break;
			case UNCOMMITTED_R: menu_cell_basic_draw(ctx, cell_layer, "Uncommitted (R)", delegate_subtitle[UNCOMMITTED_R], NULL); break;
		}
	}
}

void menu_draw_header(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *context){
	if(section_index == 0) menu_cell_basic_header_draw(ctx, cell_layer, "Dem (2,383 Needed)");
	else menu_cell_basic_header_draw(ctx, cell_layer, "Rep (1,237 Needed)");
}

static uint16_t menu_get_num_rows(MenuLayer *layer, uint16_t section_index, void *context){
	if(section_index == 0) return 3;
	else return 5;
}

static uint16_t menu_get_num_sections(MenuLayer *layer, void *context){
	return 2;
}

static int16_t menu_get_header_height(MenuLayer *layer, uint16_t section_index, void *context){
	return PBL_IF_RECT_ELSE(16,0);
}

void init(void) {
	splash_window = window_create();
	splash_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SPLASH);
	splash_layer = bitmap_layer_create(PBL_IF_RECT_ELSE(GRect(0,0,144,168),GRect(0,2,180,178)));
	bitmap_layer_set_bitmap(splash_layer, splash_bitmap);
	layer_add_child(window_get_root_layer(splash_window), bitmap_layer_get_layer(splash_layer));
	window_stack_push(splash_window, true);
	
	window = window_create();
	window_set_click_config_provider(window, click_config);
	
	menu_layer = menu_layer_create(layer_get_bounds(window_get_root_layer(window)));
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
		.draw_row = menu_draw_row,
		.get_num_rows = menu_get_num_rows,
		.get_num_sections = menu_get_num_sections,
		.get_header_height = menu_get_header_height,
		.draw_header = menu_draw_header
	});
	
	#ifdef PBL_COLOR
		menu_layer_set_highlight_colors(menu_layer, GColorBlue, GColorWhite);
	#endif
	
	layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));
	
	app_message_register_inbox_received(in_received_handler); 		
	app_message_open(2048, 2048);
	
	splash_timer = app_timer_register(2000, delay, NULL);
}

int main( void ) {
	init();
	app_event_loop();
}