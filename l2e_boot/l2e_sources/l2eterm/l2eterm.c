// Credit: lvgl, https://github.com/lupyuen/lvglterm and me (Vulcan)

#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lvgl/lvgl.h"
#include <dirent.h>
#include <poll.h>
#include <pthread.h>
#include <spawn.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define DISP_BUF_SIZE (1920 * 256)

#define TIMER_PERIOD_MS 100
#define READ_PIPE 0
#define WRITE_PIPE 1
#define L2E_TASK "/l2e"

static int create_widgets(void);
static void timer_callback(lv_timer_t *timer);
static void input_callback(lv_event_t *e);
static bool has_input(int fd);
static void remove_escape_codes(char *buf, int len);

/* Pipes for L2E Shell: stdin, stdout, stderr */

static int l2e_stdin[2];
static int l2e_stdout[2];
static int l2e_stderr[2];

/* LVGL Column Container for L2E Widgets */

static lv_obj_t *g_col;

/* LVGL Text Area Widgets for L2E Input and Output */

static lv_obj_t *g_input;
static lv_obj_t *g_output;

/* LVGL Keyboard Widget for L2E Terminal */

static lv_obj_t *g_kb;

/* LVGL Font Style for L2E Input and Output */

static lv_style_t g_terminal_style;

/* LVGL Timer for polling L2E Output */

static lv_timer_t *g_timer;

/* Arguments for L2E Task */

static char *const l2e_argv[] = {L2E_TASK, NULL};

// Creates a Terminal
static int create_terminal(void) {
  int ret;
  pid_t pid;

  /* Create the pipes for L2E Shell: stdin, stdout and stderr */

  ret = pipe(l2e_stdin);
  if (ret < 0) {
    printf("stdin pipe failed: %d\n", ret);
    return -1;
  }

  ret = pipe(l2e_stdout);
  if (ret < 0) {
    printf("stdout pipe failed: %d\n", ret);
    return -1;
  }

  ret = pipe(l2e_stderr);
  if (ret < 0) {
    printf("stderr pipe failed: %d\n", ret);
    return -1;
  }

  /* Close default stdin, stdout and stderr */

  close(0);
  close(1);
  close(2);

  /* Assign the new pipes as stdin, stdout and stderr */

  dup2(l2e_stdin[READ_PIPE], 0);
  dup2(l2e_stdout[WRITE_PIPE], 1);
  dup2(l2e_stderr[WRITE_PIPE], 2);

  /* Start the L2E Shell and inherit stdin, stdout and stderr */

  ret = posix_spawn(&pid,     /* Returned Task ID */
                    L2E_TASK, /* L2E Path */
                    NULL,     /* Inherit stdin, stdout and stderr */
                    NULL,     /* Default spawn attributes */
                    l2e_argv, /* Arguments */
                    NULL);    /* No environment */
  if (ret < 0) {
    int errcode = ret;
    printf("posix_spawn failed: %d\n", errcode);
    return -errcode;
  }

  /* Create an LVGL Timer to poll for output from L2E Shell */

  g_timer = lv_timer_create(timer_callback,  /* Callback Function */
                            TIMER_PERIOD_MS, /* Timer Period (millisec) */
                            NULL);           /* Callback Argument */

  /* Create the LVGL Terminal Widgets */

  ret = create_widgets();
  if (ret < 0) {
    return ret;
  }

  return 0;
}

// Wallpaper

void set_wall(void) {
  // LV_IMG_DECLARE(img_lcars_png);
  lv_obj_t *img;

  img = lv_img_create(lv_scr_act());
  /* Assuming a File system is attached to letter 'A'
   * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
  lv_img_set_src(img, "A:/tmp/LAIRS.png");
  lv_obj_align(img, LV_ALIGN_TOP_LEFT, 0, 0);
}

// L.I.T.T Animation

static void anim_x_cb(void *var, int32_t v) { lv_obj_set_x(var, v); }

static void anim_size_cb(void *var, int32_t v) { lv_obj_set_size(var, v, v); }

/**
 * Create a playback animation
 */
void litt_anim(void) {

  lv_obj_t *obj = lv_obj_create(lv_scr_act());
  lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_RED), 0);
  lv_obj_set_size(obj, 60, 10);
  lv_obj_set_style_radius(obj, 0, 0);

  lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, obj);
  lv_anim_set_values(&a, 0, 0);
  lv_anim_set_time(&a, 2000);
  lv_anim_set_playback_delay(&a, 300);
  lv_anim_set_playback_time(&a, 1000);
  lv_anim_set_repeat_delay(&a, 300);
  lv_anim_set_repeat_count(&a, 3);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

  // lv_anim_set_exec_cb(&a, anim_size_cb);
  // lv_anim_start(&a);
  lv_anim_set_exec_cb(&a, anim_x_cb);
  lv_anim_set_values(&a, 0, 500);
  lv_anim_start(&a);
}
// End Animation

// Creates widgets
static int create_widgets(void) {
  /* Set the Font Style for L2E Input and Output to a Monospaced Font */

  lv_style_init(&g_terminal_style);
  lv_style_set_text_font(&g_terminal_style, &lv_font_unscii_16);

  /* Create an LVGL Container with Column Flex Direction */

  // add wallpaper
  set_wall();

  // LITT Anim

  litt_anim();

  g_col = lv_obj_create(lv_scr_act());

  lv_obj_set_size(g_col, 1425, 525);
  lv_obj_align(g_col, LV_ALIGN_TOP_LEFT, 353, 432);

  lv_obj_set_flex_flow(g_col, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(g_col, 0, 0); /* No padding */

  /* Create an LVGL Text Area Widget for L2E Output */

  g_output = lv_textarea_create(g_col);

  lv_obj_add_style(g_output, &g_terminal_style, 0);
  lv_obj_set_width(g_output, LV_PCT(100));
  lv_obj_set_flex_grow(g_output, 1); /* Fill the column */

  /* Create an LVGL Text Area Widget for L2E Input */

  g_input = lv_textarea_create(g_col);

  lv_obj_add_style(g_input, &g_terminal_style, 0);
  lv_obj_set_size(g_input, LV_PCT(100), LV_SIZE_CONTENT);

  /* Create an LVGL Keyboard Widget */

  g_kb = lv_keyboard_create(g_col);

  lv_obj_set_style_pad_all(g_kb, 0, 0); /* No padding */

  /* Register the Callback Function for L2E Input */

  lv_obj_add_event_cb(g_input, input_callback, LV_EVENT_ALL, NULL);

  /* Set the Keyboard to populate the L2E Input Text Area */

  lv_keyboard_set_textarea(g_kb, g_input);

  return 0;
}

// Time Callback makes sure pipes are in sync
static void timer_callback(lv_timer_t *timer) {
  int ret;
  static char buf[64];

  /* Poll L2E stdout to check if there's output to be processed */

  if (has_input(l2e_stdout[READ_PIPE])) {
    /* Read the output from L2E stdout */

    ret = read(l2e_stdout[READ_PIPE], buf, sizeof(buf) - 1);
    if (ret > 0) {
      /* Add to L2E Output Text Area */

      buf[ret] = 0;
      remove_escape_codes(buf, ret);
      lv_textarea_add_text(g_output, buf);
    }
  }

  /* Poll L2E stderr to check if there's output to be processed */

  if (has_input(l2e_stderr[READ_PIPE])) {
    /* Read the output from L2E stderr */

    ret = read(l2e_stderr[READ_PIPE], buf, sizeof(buf) - 1);
    if (ret > 0) {
      /* Add to L2E Output Text Area */

      buf[ret] = 0;
      remove_escape_codes(buf, ret);
      lv_textarea_add_text(g_output, buf);
    }
  }
}

// If Enter Key was pressed, send the L2E Input Command to L2E stdin.

static void input_callback(lv_event_t *e) {
  int ret;

  /* Decode the LVGL Event */

  const lv_event_code_t code = lv_event_get_code(e);

  // Auto hide keyboard

  if (code == LV_EVENT_FOCUSED) {
    if (lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD) {
      lv_keyboard_set_textarea(g_kb, g_input);
      lv_obj_update_layout(g_col); /*Be sure the sizes are recalculated*/
      lv_obj_clear_flag(g_kb, LV_OBJ_FLAG_HIDDEN);
      lv_obj_scroll_to_view_recursive(g_input, LV_ANIM_OFF);
      lv_indev_wait_release(lv_event_get_param(e));
    }
  } else if (code == LV_EVENT_DEFOCUSED) {
    lv_keyboard_set_textarea(g_kb, NULL);
    lv_obj_add_flag(g_kb, LV_OBJ_FLAG_HIDDEN);
    lv_indev_reset(NULL, g_input);
    litt_anim();
  }

  /* If L2E Input Text Area has changed, get the Key Pressed */

  if (code == LV_EVENT_VALUE_CHANGED) {
    /* Get the Button Index of the Keyboard Button Pressed */

    const uint16_t id = lv_keyboard_get_selected_btn(g_kb);

    /* Get the Text of the Keyboard Button */

    const char *key = lv_keyboard_get_btn_text(g_kb, id);
    if (key == NULL) {
      return;
    }

    /* If Key Pressed is Enter, send the Command to L2E stdin */

    if (code == LV_EVENT_VALUE_CHANGED) {
      /* Read the L2E Input */

      const char *cmd;

      cmd = lv_textarea_get_text(g_input);
      if (cmd == NULL || cmd[0] == 0) {
        return;
      }

      /* Send the Command to L2E stdin */
      if (strchr(cmd, '\n')) {
        ret = write(l2e_stdin[WRITE_PIPE], cmd, strlen(cmd));
        /* Erase the L2E Input */
        lv_textarea_set_text(g_input, "");
      }
    }
  }
}

// Return true if the File Descriptor has data to be read.
static bool has_input(int fd) {
  int ret;

  /* Poll the File Descriptor for input */

  struct pollfd fdp;
  fdp.fd = fd;
  fdp.events = POLLIN;
  ret = poll(&fdp, /* File Descriptors */
             1,    /* Number of File Descriptors */
             0);   /* Poll Timeout (Milliseconds) */

  if (ret > 0) {
    /* If poll is OK and there is input */

    if ((fdp.revents & POLLIN) != 0) {
      /* Report that there is input */

      return true;
    }

    /* Else report no input */

    return false;
  } else if (ret == 0) {
    /* If timeout, report no input */

    return false;
  } else if (ret < 0) {
    /* Handle error */

    printf("poll failed: %d, fd=%d\n", ret, fd);
    return false;
  }

  /* Never comes here */

  return false;
}

// Remove ANSI Escape Codes from the string. Assumes that buf[len] is 0.
static void remove_escape_codes(char *buf, int len) {
  int i;
  int j;

  for (i = 0; i < len; i++) {
    /* Escape Code looks like 0x1b 0x5b 0x4b */

    if (buf[i] == 0x1b) {
      /* Remove 3 bytes */

      for (j = i; j + 2 < len; j++) {
        buf[j] = buf[j + 3];
      }
    }
  }
}

// Hide cursor
static void hidecursor(bool setting) {
  // hide "\033[?25l" / show "\033[?25h"
  system(setting ? "echo -e \"\033[?25l\"" : "echo -e \"\033[?25h\"");
}

// Make terminal silent
// Credit: // https://stackoverflow.com/a/73204172
void termprep() {
  struct termios tc;
  tcgetattr(0, &tc);
  tc.c_lflag &= ~(ICANON | ECHO);
  tc.c_cc[VMIN] = 0;
  tc.c_cc[VTIME] = 0;
  tcsetattr(0, TCSANOW, &tc);
}

int main(void) {
  int ret;

  hidecursor(true);

  /* Prepare terminal */

  static struct termios oterm;
  // Get current terminal parameters
  tcgetattr(0, &oterm);
  termprep();

  /* LVGL initialization */

  lv_init();

  /* LVGL port initialization */

  /*Linux frame buffer device init*/
  fbdev_init();

  /*A small buffer for LittlevGL to draw the screen's content*/
  static lv_color_t buf[DISP_BUF_SIZE];

  /*Initialize a descriptor for the buffer*/
  static lv_disp_draw_buf_t disp_buf;
  lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

  /*Initialize and register a display driver*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf;
  disp_drv.flush_cb = fbdev_flush;
  disp_drv.hor_res = 1920;
  disp_drv.ver_res = 1080;
  lv_disp_drv_register(&disp_drv);

  evdev_init();
  static lv_indev_drv_t indev_drv_1;
  lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
  indev_drv_1.type = LV_INDEV_TYPE_POINTER;

  /*This function will be called periodically (by the library) to get the mouse
   * position and state*/
  indev_drv_1.read_cb = evdev_read;
  lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

  /*Set a cursor for the mouse*/
  LV_IMG_DECLARE(mouse_cursor_icon)
  lv_obj_t *cursor_obj =
      lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
  lv_img_set_src(cursor_obj, &mouse_cursor_icon); /*Set the image source*/
  lv_indev_set_cursor(mouse_indev,
                      cursor_obj); /*Connect the image  object to the driver*/

  /* Create the LVGL Widgets */

  ret = create_terminal();
  if (ret < 0) {
    return EXIT_FAILURE;
  }

  /*Handle LitlevGL tasks (tickless mode)*/
  while (1) {
    lv_timer_handler();
    usleep(5000);
  }

  // Restore terminal
  tcsetattr(STDIN_FILENO, TCSANOW, &oterm);

  return EXIT_SUCCESS;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void) {
  static uint64_t start_ms = 0;
  if (start_ms == 0) {
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);
    start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
  }

  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  uint64_t now_ms;
  now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;
  uint32_t time_ms = now_ms - start_ms;
  return time_ms;
}
