//
// File: raia_duktape.c
// About:
//   author - dolphilia
//   date - 2022/09/09
//

#include "raia/raia_duktape.h"

static duk_ret_t regist_global_buffer_test(duk_context *ctx) {
    unsigned char *ptr;
    duk_size_t len;
    //duk_size_t i;
    ptr = (unsigned char *) duk_require_buffer_data(ctx, 0 /*idx*/, &len);
    printf("%d %d\n", (int)ptr[0], (int)len);
    //duk_size_t size;
    //void * ptr = duk_to_buffer(ctx, 0, &size);
    //printf("coerced data at %p, size %lu\n", ptr, (unsigned long) size);
    //uint8_t * u8 = (uint8_t *)ptr;
    //printf("%d\n", (int)u8[0]);
    
    //printf("%s\n", duk_to_string(ctx, 0));
    //ptr = duk_require_buffer_data(ctx, -3, &sz);
    //duk_get_buffer_data(ctx, <#duk_idx_t idx#>, <#duk_size_t *out_size#>)
    return 0;  /* no return value (= undefined) */
}

static duk_ret_t regist_global_object_test(duk_context *ctx) {
   // void *ptr = duk_get_heapptr(ctx, 0);
    
    //printf("%s\n", duk_to_string(ctx, 0));
    //if (duk_is_object(ctx, 0)) {
    //    printf("true\n");
    //    /* ... */
    //}

    //duk_push_object(ctx);
    //printf("JSON encoded: %s\n", duk_json_encode(ctx, -1));
    
    return 1;  /* no return value (= undefined) */
}

static duk_ret_t regist_global_get_object_test(duk_context *ctx) {
    //printf("%s\n", duk_to_string(ctx, 0));
    // return {
    //     raia_ver: 1,
    //     raia_value: 102,
    //     raia_str: hello raia,
    //     raia_array: ["foo", "bar"],
    // }
    duk_push_object(ctx);
    duk_push_int(ctx, 1);
    duk_put_prop_string(ctx, -2, "raia_ver");
    duk_push_int(ctx, 102);
    duk_put_prop_string(ctx, -2, "raia_value");
    duk_push_string(ctx, "hello raia");
    duk_put_prop_string(ctx, -2, "raia_str");
    
    duk_idx_t arr_idx;
    arr_idx = duk_push_array(ctx);
    duk_push_string(ctx, "foo");
    duk_put_prop_index(ctx, arr_idx, 0);
    duk_push_string(ctx, "bar");
    duk_put_prop_index(ctx, arr_idx, 1);
    //duk_pop(ctx);
    duk_put_prop_string(ctx, -2, "raia_array");
    
    return 1;  /* no return value (= undefined) */
}


static duk_ret_t regist_global_print(duk_context *ctx) {
    printf("%s\n", duk_to_string(ctx, 0));
    return 0;  /* no return value (= undefined) */
}

static duk_ret_t regist_io_load_string_filename(duk_context *ctx) {
    char * ret = load_string_filename(duk_to_string(ctx, 0));
    duk_push_string(ctx, ret);
    return 1;  /* no return value (= undefined) */
}

// Function: regist_stdc_puts
// C?????????puts??????????????????RaiaAPI???????????????????????????
//
static duk_ret_t regist_stdc_puts(duk_context *ctx) {
    int ret = puts(duk_to_string(ctx, 0));
    duk_push_number(ctx, ret);
    return 1;  /* no return value (= undefined) */
}

static duk_ret_t regist_library_init(duk_context *ctx) {
    const char* dll_file = duk_to_string(ctx, 0);
    init_dlfcn(dll_file);
    return 0;  /* no return value (= undefined) */
}

static duk_ret_t regist_library_add(duk_context *ctx) {
    //void * handle = get_dlfcn_handle();
    const char* dll_func_name = duk_to_string(ctx, 0);
    const char* regist_func_name = duk_to_string(ctx, 1);
    int nargs = (int)duk_to_number(ctx, 2);
    duk_ret_t (*pfunc)(duk_context * ctx) = add_dlfcn_func(ctx, dll_func_name);
    //duk_push_number(ctx, ret);
    duk_push_c_function(ctx, pfunc, nargs);
    duk_put_global_string(ctx, regist_func_name);
    return 0;  /* no return value (= undefined) */
}

// Function: regist_glfw_pool_events
// GLFW???glfwPoolEvents??????????????????
//
static duk_ret_t regist_glfw_pool_events(duk_context *ctx) {
    glfwPollEvents();
    return 0;  /* no return value (= undefined) */
}

// ????????????glfw_window_should_close????????????????????????
static duk_ret_t regist_glfw_window_should_close(duk_context *ctx) {
    GLFWwindow* glfw_window = get_raia_window();
    int ret = glfwWindowShouldClose(glfw_window);
    duk_push_number(ctx, ret);
    return 1; /* one return value */
}

// ????????????glfw_redraw???????????????
static duk_ret_t regist_draw_redraw(duk_context *ctx) {
    glfw_redraw();
    return 0; /* no return value */
}

// ????????????set_pixel??????????????????????????????
static duk_ret_t regist_draw_set_pixel_rgb(duk_context *ctx) {
    int n = duk_get_top(ctx);  /* #args */
    if (n < 5) {
        fprintf(stderr, "??????????????????????????????\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    int point_x = (int)duk_to_number(ctx, 0);
    int point_y = (int)duk_to_number(ctx, 1);
    int color_r = (int)duk_to_number(ctx, 2);
    int color_g = (int)duk_to_number(ctx, 3);
    int color_b = (int)duk_to_number(ctx, 4);
    uint8_t* pixel_data = get_pixel_data();
    raia_header_t header = get_raia_header();
    set_pixel_rgb(pixel_data, point_x, point_y, color_r, color_g, color_b, header.window_width, header.window_height, header.samples_per_pixel);
    return 0;
}

// ????????????set_pixel??????????????????????????????
static duk_ret_t regist_draw_set_pixel(duk_context *ctx) {
    int n = duk_get_top(ctx);  /* #args */
    if (n < 2) {
        fprintf(stderr, "??????????????????????????????\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    int point_x = (int)duk_to_number(ctx, 0);
    int point_y = (int)duk_to_number(ctx, 1);
    uint8_t* pixel_data = get_pixel_data();
    raia_header_t header = get_raia_header();
    set_pixel_rgb(pixel_data, point_x, point_y, header.current_color.red, header.current_color.green, header.current_color.blue, header.window_width, header.window_height, header.samples_per_pixel);
    return 0;
}

// ????????????fill_rect_rgb??????????????????????????????
static duk_ret_t regist_draw_fill_rect_rgb(duk_context *ctx) {
    int n = duk_get_top(ctx);  /* #args */
    if (n < 7) {
        fprintf(stderr, "??????????????????????????????\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    int start_point_x = (int)duk_to_number(ctx, 0);
    int start_point_y = (int)duk_to_number(ctx, 1);
    int end_point_x = (int)duk_to_number(ctx, 2);
    int end_point_y = (int)duk_to_number(ctx, 3);
    int color_r = (int)duk_to_number(ctx, 4);
    int color_g = (int)duk_to_number(ctx, 5);
    int color_b = (int)duk_to_number(ctx, 6);
    uint8_t* pixel_data = get_pixel_data();
    raia_header_t header = get_raia_header();
    fill_rect_rgb(pixel_data, start_point_x, start_point_y, end_point_x, end_point_y, color_r, color_g, color_b, header.window_width, header.window_height, header.samples_per_pixel);
    return 0;
}

// ????????????fill_rect_rgb??????????????????????????????
static duk_ret_t regist_draw_fill_rect(duk_context *ctx) {
    int n = duk_get_top(ctx);  /* #args */
    if (n < 4) {
        fprintf(stderr, "??????????????????????????????\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    int start_point_x = (int)duk_to_number(ctx, 0);
    int start_point_y = (int)duk_to_number(ctx, 1);
    int end_point_x = (int)duk_to_number(ctx, 2);
    int end_point_y = (int)duk_to_number(ctx, 3);
    uint8_t* pixel_data = get_pixel_data();
    raia_header_t header = get_raia_header();
    fill_rect_rgb(pixel_data, start_point_x, start_point_y, end_point_x, end_point_y, header.current_color.red, header.current_color.green, header.current_color.blue, header.window_width, header.window_height, header.samples_per_pixel);
    return 0;
}

static duk_ret_t regist_draw_set_current_color(duk_context *ctx) {
    int r = duk_to_int(ctx, 0);
    int g = duk_to_int(ctx, 1);
    int b = duk_to_int(ctx, 2);
    set_current_color(r, g, b);
    return 0;  /* no return value (= undefined) */
}

static duk_ret_t regist_draw_set_current_point(duk_context *ctx) {
    int x = duk_to_int(ctx, 0);
    int y = duk_to_int(ctx, 1);
    set_current_point(x, y);
    return 0;  /* no return value (= undefined) */
}

static duk_ret_t regist_window_set_title(duk_context *ctx) {
    GLFWwindow* window = get_raia_window();
    const char* title = duk_to_string(ctx, 0);
    glfwSetWindowTitle(window, title);
    set_title((char*)title);
    return 0;  /* no return value (= undefined) */
}

static duk_ret_t regist_window_get_title(duk_context *ctx) {
    raia_header_t header = get_raia_header();
    duk_push_string(ctx, header.window_title);
    return 1;  /* no return value (= undefined) */
}

static duk_ret_t regist_window_get_width(duk_context *ctx) {
    raia_header_t header = get_raia_header();
    duk_push_int(ctx, header.window_width);
    return 1;  /* no return value (= undefined) */
}

static duk_ret_t regist_window_get_height(duk_context *ctx) {
    raia_header_t header = get_raia_header();
    duk_push_int(ctx, header.window_height);
    return 1;  /* no return value (= undefined) */
}

static duk_ret_t regist_image_load(duk_context *ctx) {
    //GLFWwindow* window = get_raia_window();
    const char* filename = duk_to_string(ctx, 0);
    uint8_t* pixel_data = get_pixel_data();
    raia_header_t header = get_raia_header();
    load_image(filename, pixel_data, header.current_point.x, header.current_point.y, header.window_width, header.window_height);
    //glfwSetWindowTitle(window, title);
    //set_title((char*)title);
    return 0;  /* no return value (= undefined) */
}

static duk_ret_t regist_callback(duk_context *ctx) {
    duk_dup(ctx, 0);
    duk_put_global_string(ctx, "_my_callback");
    return 0;
}

static duk_ret_t regist_call(duk_context *ctx) {
    duk_get_global_string(ctx, "_my_callback");
    duk_int_t rc = duk_pcall(ctx, 0);  /* no arguments in this example */
    if (rc != 0) {
        printf("Callback failed: %s\n", duk_safe_to_string(ctx, -1));
    } else {
        printf("Callback success\n");
    }
    duk_pop(ctx);  /* pop result */
    return 0;
}

static duk_ret_t regist_event_set_error_callback(duk_context *ctx) {
    duk_dup(ctx, 0);
    duk_put_global_string(ctx, "_glfw_error_callback");
    return 0;
}

static void event_error_callback(int error, const char *message) {
    fprintf(stderr, "GLFW: %s\n", message);
    
    duk_context* ctx = get_duk_ctx();
    duk_get_global_string(ctx, "_glfw_error_callback");
    duk_push_int(ctx, error);
    duk_push_string(ctx, message);
    duk_int_t rc = duk_pcall(ctx, 2);  /* no arguments in this example */
    if (rc != 0) {
        printf("Callback failed: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);  /* pop result */
}

static duk_ret_t regist_event_set_key_callback(duk_context *ctx) {
    duk_dup(ctx, 0);
    duk_put_global_string(ctx, "_glfw_key_callback");
    return 0;
}

static void event_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    //    glfwSetWindowShouldClose(window, GL_TRUE);
    //}
    duk_context* ctx = get_duk_ctx();
    duk_get_global_string(ctx, "_glfw_key_callback");
    duk_push_int(ctx, key);
    duk_push_int(ctx, scancode);
    duk_push_int(ctx, action);
    duk_push_int(ctx, mods);
    duk_int_t rc = duk_pcall(ctx, 4);  /* no arguments in this example */
    if (rc != 0) {
        printf("Callback failed: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);  /* pop result */
}

static duk_ret_t regist_event_set_update_callback(duk_context *ctx){
    duk_dup(ctx, 0);
    duk_put_global_string(ctx, "_event_update_callback");
    set_exist_update_callback(true);
    return 0;
}

static void event_update_callback(void) {
    duk_context* ctx = get_duk_ctx();
    duk_get_global_string(ctx, "_event_update_callback");
    duk_int_t rc = duk_pcall(ctx, 0);  /* no arguments in this example */
    if (rc != 0) {
        printf("Callback failed: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);  /* pop result */
}

static duk_ret_t regist_event_set_cursor_position_callback(duk_context *ctx) {
    duk_dup(ctx, 0);
    duk_put_global_string(ctx, "_glfw_cursor_position_callback");
    return 0;
}

static void event_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    duk_context* ctx = get_duk_ctx();
    duk_get_global_string(ctx, "_glfw_cursor_position_callback");
    duk_push_number(ctx, xpos);
    duk_push_number(ctx, ypos);
    duk_int_t rc = duk_pcall(ctx, 2);  /* no arguments in this example */
    //if (rc != 0) {
    //    printf("Callback failed: %s\n", duk_safe_to_string(ctx, -1));
    //}
    duk_pop(ctx);  /* pop result */
}

static duk_ret_t regist_event_set_mouse_button_callback(duk_context *ctx) {
    duk_dup(ctx, 0);
    duk_put_global_string(ctx, "_glfw_mouse_button_callback");
    return 0;
}

static void event_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    duk_context* ctx = get_duk_ctx();
    duk_get_global_string(ctx, "_glfw_mouse_button_callback");
    duk_push_int(ctx, button);
    duk_push_int(ctx, action);
    duk_push_int(ctx, mods);
    duk_int_t rc = duk_pcall(ctx, 3);  /* no arguments in this example */
    //if (rc != 0) {
    //    printf("Callback failed: %s\n", duk_safe_to_string(ctx, -1));
    //}
    duk_pop(ctx);  /* pop result */
}

// Function: regist_callbacks
// ????????????????????????????????????
//
static void regist_callbacks(duk_context *ctx) {
    glfwSetKeyCallback(get_raia_window(), event_key_callback); // ??????????????????????????????
    glfwSetErrorCallback(event_error_callback); // ???????????????????????????
    glfwSetCursorPosCallback(get_raia_window(), event_cursor_position_callback); // ???????????????
    glfwSetMouseButtonCallback(get_raia_window(), event_mouse_button_callback); // ??????????????????
}

// Function: regist_functions
// ????????????????????????
//
static void regist_functions(duk_context *ctx) {
    regist_func(ctx, regist_global_buffer_test, "buffer_test", 1);
    regist_func(ctx, regist_global_object_test, "object_test", 1);
    regist_func(ctx, regist_global_get_object_test, "get_object_test", 1);
    regist_func(ctx, regist_global_print, "print", 1);
    regist_func(ctx, regist_io_load_string_filename, "_io_load_string_filename", 1);
    regist_func(ctx, regist_stdc_puts, "_stdc_puts", 1);
    regist_func(ctx, regist_library_init, "_library_init", 1);
    regist_func(ctx, regist_library_add, "_library_add", 3);
    regist_func(ctx, regist_glfw_pool_events, "_glfw_pool_events", 0);
    regist_func(ctx, regist_glfw_window_should_close, "_glfw_window_should_close", 0);
    regist_func(ctx, regist_draw_redraw, "_draw_redraw", 0);
    regist_func(ctx, regist_draw_set_pixel_rgb, "_draw_set_pixel_rgb", 5);
    regist_func(ctx, regist_draw_set_pixel, "_draw_set_pixel", 2);
    regist_func(ctx, regist_draw_fill_rect_rgb, "_draw_fill_rect_rgb", 7);
    regist_func(ctx, regist_draw_fill_rect, "_draw_fill_rect", 4);
    regist_func(ctx, regist_draw_set_current_color, "_draw_set_current_color", 3);
    regist_func(ctx, regist_draw_set_current_point, "_draw_set_current_point", 2);
    regist_func(ctx, regist_window_set_title, "_window_set_title", 1);
    regist_func(ctx, regist_window_get_title, "_window_get_title", 0);
    regist_func(ctx, regist_window_get_width, "_window_get_width", 0);
    regist_func(ctx, regist_window_get_height, "_window_get_height", 0);
    regist_func(ctx, regist_image_load, "_image_load", 1);
    regist_func(ctx, regist_event_set_key_callback, "_event_set_key_callback", 1);
    regist_func(ctx, regist_event_set_error_callback, "_event_set_error_callback", 1);
    regist_func(ctx, regist_event_set_update_callback, "_event_set_update_callback", 1);
    regist_func(ctx, regist_event_set_cursor_position_callback, "_event_set_cursor_position_callback", 1);
    regist_func(ctx, regist_event_set_mouse_button_callback, "_event_set_mouse_button_callback", 1);
    regist_func(ctx, regist_callback, "callback", 1);
    regist_func(ctx, regist_call, "call", 0);
}

// Function: regist_objects
// ????????????????????????????????????
//
static void regist_objects(duk_context *ctx) {
    char* objects =
    "var STDC = {};"
    "var Library = {};"
    "var IO = {};"
    "var GLFW = {};"
    "var Draw = {};"
    "var Image = {};"
    "var Window = {};"
    "var Event = {};"
    ;
    duk_eval_string(ctx, objects);
}

// Function: regist_methods
// ??????????????????????????????
//
static void regist_methods(duk_context *ctx) {
    char* methods =
    "STDC = {"
    "    puts: function(a){_stdc_puts(a);},"
    "};"
    "Library = {"
    "    init: function(a){_library_init(a);},"
    "    add: function(a,b,c){_library_add(a,b,c);},"
    "};"
    "IO = {"
    "    loadStringFilename: function(a){return _io_load_string_filename(a);},"
    "};"
    "Window = {"
    "    setTitle: function(a){_window_set_title(a);},"
    "    getTitle: function(){return _window_get_title();},"
    "    getWidth: function(){return _window_get_width();},"
    "    getHeight: function(){return _window_get_height();},"
    "};"
    "Draw = {"
    "    redraw: function(){return _draw_redraw();},"
    "    setColor: function(a,b,c){_draw_set_current_color(a,b,c);},"
    "    setPoint: function(a,b){_draw_set_current_point(a,b);},"
    "    setPixel: function(a,b){_draw_set_pixel(a,b);},"
    "    fillRect: function(a,b,c,d){_draw_fill_rect(a,b,c,d);},"
    "    setPixelRGB: function(a,b,c,d,e){_draw_set_pixel_rgb(a,b,c,d,e);},"
    "    fillRectRGB: function(a,b,c,d,e,f,g){_draw_fill_rect_rgb(a,b,c,d,e,f,g);},"
    "};"
    "GLFW = {"
    "    poolEvents: function(){_glfw_pool_events();},"
    "    windowShouldClose: function(){return _glfw_window_should_close();},"
    "};"
    "Image = {"
    "    load: function(a){_image_load(a)},"
    "};"
    "Event = {"
    "    setKeyCallback: function(a,b,c,d){return _event_set_key_callback(a,b,c,d);},"
    "    setErrorCallback: function(a,b){return _event_set_error_callback(a,b);},"
    "    setUpdateCallback: function(a){return _event_set_update_callback(a);},"
    "    setCursorPositionCallback: function(a,b,c,d){return _event_set_cursor_position_callback(a,b,c,d);},"
    "    setMouseButtonCallback: function(a,b,c,d){return _event_set_mouse_button_callback(a,b,c,d);},"
    "};"
    ;
    duk_eval_string(ctx, methods);
}

// Functions: regist_properties
// ????????????????????????
//
static void regist_constants(duk_context *ctx) {
    char* constants =
    /*
    "GLFW = {"
    "    FOCUSED: 0x00020001,"
    "    ICONIFIED: 0x00020002,"
    "    RESIZABLE: 0x00020003,"
    "    VISIBLE: 0x00020004,"
    "    DECORATED: 0x00020005,"
    "    AUTO_ICONIFY: 0x00020006,"
    "    FLOATING: 0x00020007,"
    "    MAXIMIZED: 0x00020008,"
    "    CENTER_CURSOR: 0x00020009,"
    "    TRANSPARENT_FRAMEBUFFER: 0x0002000A,"
    "    HOVERED: 0x0002000B,"
    "    FOCUS_ON_SHOW: 0x0002000C,"
    "    RED_BITS: 0x00021001,"
    "    GREEN_BITS: 0x00021002,"
    "    BLUE_BITS: 0x00021003,"
    "    ALPHA_BITS: 0x00021004,"
    "    DEPTH_BITS: 0x00021005,"
    "    STENCIL_BITS: 0x00021006,"
    "    ACCUM_RED_BITS: 0x00021007,"
    "    ACCUM_GREEN_BITS: 0x00021008,"
    "    ACCUM_BLUE_BITS: 0x00021009,"
    "    ACCUM_ALPHA_BITS: 0x0002100A,"
    "    AUX_BUFFERS: 0x0002100B,"
    "    STEREO: 0x0002100C,"
    "    SAMPLES: 0x0002100D,"
    "    SRGB_CAPABLE: 0x0002100E,"
    "    REFRESH_RATE: 0x0002100F,"
    "    DOUBLEBUFFER: 0x00021010,"
    "    CLIENT_API: 0x00022001,"
    "    CONTEXT_VERSION_MAJOR: 0x00022002,"
    "    CONTEXT_VERSION_MINOR: 0x00022003,"
    "    CONTEXT_REVISION: 0x00022004,"
    "    CONTEXT_ROBUSTNESS: 0x00022005,"
    "    OPENGL_FORWARD_COMPAT: 0x00022006,"
    "    OPENGL_DEBUG_CONTEXT: 0x00022007,"
    "    OPENGL_PROFILE: 0x00022008,"
    "    CONTEXT_RELEASE_BEHAVIOR: 0x00022009,"
    "    CONTEXT_NO_ERROR: 0x0002200A,"
    "    CONTEXT_CREATION_API: 0x0002200B,"
    "    SCALE_TO_MONITOR: 0x0002200C,"
    "    COCOA_RETINA_FRAMEBUFFER: 0x00023001,"
    "    COCOA_FRAME_NAME: 0x00023002,"
    "    COCOA_GRAPHICS_SWITCHING: 0x00023003,"
    "    X11_CLASS_NAME: 0x00024001,"
    "    X11_INSTANCE_NAME: 0x00024002,"
    "};"
    */
    "var OS = {"
#ifdef __WINDOWS__
    "    PLATFORM: 'Windows',"
#endif
#ifdef __MACOS__
    "    PLATFORM: 'macOS',"
#endif
#ifdef __LINUX__
    "    PLATFORM: 'Linux',"
#endif
    "};";
    duk_eval_string(ctx, constants);
/*
    duk_eval_string(ctx, "var OS = new Object();");
#ifdef __WINDOWS__
    duk_eval_string(ctx, "OS.PLATFORM = 'windows';");
#endif
#ifdef __MACOS__
    duk_eval_string(ctx, "OS.PLATFORM = 'macos';");
#endif
*/
}

static void regist_elements(duk_context *ctx) {
    regist_callbacks(ctx); // ????????????????????????????????????
    regist_functions(ctx); // ????????????????????????
    regist_objects(ctx); // ????????????????????????????????????
    regist_methods(ctx); // ??????????????????????????????
    regist_constants(ctx); // ????????????????????????
}

static void enable_module(duk_context *ctx) {
    // ????????????????????????????????????????????????????????????????????????.
    char * code =
    "Duktape.modSearch = function (id, require, exports, module) {"
    "    var filename = 'modules/' + id + '.js';"
    "    var src = IO.loadStringFilename(filename);"
    "    if (typeof src === 'string') {"
    "        print('loaded ECMAScript:', filename);"
    "    } else {"
    "        throw new Error('module not found: ' + id);"
    "    }"
    "    return src;"
    "};";
    duk_eval_string(ctx, code);
}

static void wait_time_with_fps(double now, double prev, int fps) {
    while(!glfwWindowShouldClose(get_raia_window())) { // ?????? 60fps ????????????????????????
        now = (double)clock() / CLOCKS_PER_SEC;
        if ((now - prev) > 1.0 / (double)fps) {
            break;
        }
        glfwPollEvents();
    }
}

static void loop_update(void) {
    double now = (double)clock() / CLOCKS_PER_SEC;
    double prev = (double)clock() / CLOCKS_PER_SEC;
    while(!glfwWindowShouldClose(get_raia_window())) {
        now = (double)clock() / CLOCKS_PER_SEC;
        event_update_callback();
        glfw_redraw();
        prev = now;
        wait_time_with_fps(now, prev, 60);
        glfwPollEvents();
    }
}

static void run_update(void) {
    raia_header_t header = get_raia_header();
    if(header.exist_update_callback == true) {
        loop_update();
    }
}

// Duktape?????????
//
void duktape_start(void) {
    duk_context* ctx = get_duk_ctx();
    regist_elements(ctx);
    duk_module_duktape_init(ctx);
    enable_module(ctx);
    load_script_filename(ctx, "startup.js"); // ??????????????????????????????????????????????????????????????????????????????
    duk_eval(ctx); // ????????????
    run_update();
    duk_destroy_heap(ctx); // ????????????????????????
}
