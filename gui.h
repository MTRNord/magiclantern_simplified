#ifndef _dryos_gui_h_
#define _dryos_gui_h_

/** \file
 * DryOS GUI structures and functions.
 */

/** Display types.
 *
 * 0 == 720p LCD
 * 3 == 960 HDMI
 * 6 == 960 HDMI
 * All others unknown.
 */
extern int
gui_get_display_type( void );

extern void
color_palette_push(
	int			palette_id
);


/** Event types */
typedef enum {
	GOT_TOP_OF_CONTROL		= 0x800,
	LOST_TOP_OF_CONTROL		= 0x801,
	INITIALIZE_CONTROLLER		= 0x802,
	TERMINATE_WINSYS		= 0x804,
	DELETE_DIALOG_REQUEST		= 0x805,
	PRESS_RIGHT_BUTTON		= 0x807,
	PRESS_LEFT_BUTTON		= 0x809,
	PRESS_UP_BUTTON			= 0x80B,
	PRESS_DOWN_BUTTON		= 0x80D,
	PRESS_MENU_BUTTON		= 0x80F,
	UNPRESS_SET_BUTTON		= 0x810,
	PRESS_SET_BUTTON		= 0x812,
	PRESS_PICSTYLE_BUTTON		= 0x81C,
	PRESS_INFO_BUTTON		= 0x829,
	ELECTRONIC_SUB_DIAL_RIGHT	= 0x82B,
	ELECTRONIC_SUB_DIAL_LEFT	= 0x82C,
	PRESS_DISP_BUTTON		= 0x10000000,
	PRESS_DIRECT_PRINT_BUTTON	= 0x10000005,
	PRESS_FUNC_BUTTON		= 0x10000007,
	PRESS_PICTURE_STYLE_BUTTON	= 0x10000009,
	GUICMD_OPEN_SLOT_COVER		= 0x1000000B,
	GUICMD_CLOSE_SLOT_COVER		= 0x1000000C,
	GUICMD_MADE_QR			= 0x10000037,
	GUICMD_MADE_FILE		= 0x10000038,
	START_SHOOT_MOVIE		= 0x1000008A,
	RESIZE_MAYBE			= 0x10000085,
	GUICMD_UI_OK			= 0x100000A1,
	GUICMD_START_AS_CHECK		= 0x100000A2,
	GUICMD_LOCK_OFF			= 0x100000A3,
	GUICMD_LOCK_ON			= 0x100000A4,
} gui_event_t;


/** Create a GUI event handler.
 * Does this always take a dialog pointer?
 *
 * The handler must return 0 if it has handled the event or 1 if
 * it did not handle it and the event should be propagated.
 *
 * Event types are defined below.
 */

typedef int (*gui_event_handler)(
	void *			priv,
	gui_event_t		event,
	int			arg2,
	int			arg3
);


/** GUI task.
 * Not sure about the next/prev fields.
 * See gui_task_call_events() at 0xFFA53B8C
 */
struct gui_task
{
	gui_event_handler	handler;	// off_0x00;
	void *			priv;		// off_0x04;
	struct gui_task *	next;		// off_0x08;
	struct gui_task *	prev;		// off_0x0c;
};

SIZE_CHECK_STRUCT( gui_task, 0x10 );

extern struct gui_task *
gui_task_create(
	gui_event_handler	handler,
	void *			priv
);


/** Internal structure used by the gui code */
struct event
{
	uint32_t		type;
	uint32_t		param;
	void *			obj;
	uint32_t		arg; // unknown meaning
};

/** Magic Lantern gui event log */
#define MAX_GUI_EVENTS		16
extern struct event gui_events[ MAX_GUI_EVENTS ];
extern int gui_events_index;

extern void gui_init_end( void );
extern void msg_queue_receive( void *, struct event **, uint32_t );
extern void gui_massive_event_loop( uint32_t, void *, uint32_t );
extern void gui_local_post( uint32_t, void *, uint32_t );
extern void gui_other_post( uint32_t, void *, uint32_t );
extern void gui_post_10000085( uint32_t, void *, uint32_t );
extern void gui_init_event( void * obj );
extern void gui_change_shoot_type_post( uint32_t event );
extern void gui_change_lcd_state_post( uint32_t event );
extern void gui_timer_something( void *, uint32_t );
extern void gui_change_mode( uint32_t param );




struct gui_struct
{
	uint32_t		off_0x00;
	uint32_t		off_0x04;
	uint32_t		off_0x08;
	uint32_t		off_0x0c;
	uint32_t		off_0x10;
	uint32_t		off_0x14;
	uint32_t		off_0x18;
	uint32_t		off_0x1c;
	uint32_t		off_0x20;
	uint32_t		off_0x24;
	uint32_t		off_0x28;
	uint32_t		off_0x2c;
	uint32_t		off_0x30;
	uint32_t		off_0x34;
	uint32_t		off_0x38;
	uint32_t		off_0x3c;
	uint32_t		off_0x40;
	uint32_t		off_0x44;
	uint32_t		off_0x48;
	uint32_t		off_0x4c;
	uint32_t		off_0x50;
	uint32_t		off_0x54;
	uint32_t		off_0x58;
	uint32_t		off_0x5c;
	uint32_t		off_0x60;
	uint32_t		off_0x64;
	uint32_t		off_0x68;
	uint32_t		off_0x6c;
	uint32_t		off_0x70;
	uint32_t		off_0x74;
	uint32_t		off_0x78;
	uint32_t		off_0x7c;
	uint32_t		off_0x80;
	uint32_t		off_0x84;
	uint32_t		off_0x88;
	uint32_t		off_0x8c;
	uint32_t		off_0x90;
	uint32_t		off_0x94;
	uint32_t		off_0x98;
	uint32_t		off_0x9c;
	uint32_t		off_0xa0;
	uint32_t		off_0xa4;
	uint32_t		off_0xa8;
	uint32_t		off_0xac;
	uint32_t		off_0xb0;
	uint32_t		off_0xb4;
	uint32_t		off_0xb8;
	uint32_t		off_0xbc;

	/**
	 * 0 == no,
	 * 1 == starting,
	 * 2 == started,
	 * 3 == ending
	*/
	uint32_t		movie_is_recording;		// off_0xc0;

	uint32_t		off_0xc4;
	uint32_t		off_0xc8;
	uint32_t		off_0xcc;
	uint32_t		off_0xd0;
	uint32_t		off_0xd4;
	uint32_t		off_0xd8;

	/**
	 * 0 - 100%
	 */
	uint32_t		movie_record_buffer;		// off_0xdc;

	uint32_t		off_0xe0;
	uint32_t		off_0xe4;
	uint32_t		off_0xe8;
	uint32_t		off_0xec;
	uint32_t		off_0xf0;
	uint32_t		off_0xf4;
	uint32_t		off_0xf8;
	uint32_t		off_0xfc;
};

extern struct gui_struct gui_struct;

/** Magic Lantern GUI */
extern volatile int gui_show_menu;

#endif
