#pragma once

enum lcCommandId
{
	LC_FILE_NEW,
	LC_FILE_OPEN,
	LC_FILE_MERGE,
	LC_FILE_SAVE,
	LC_FILE_SAVEAS,
	LC_FILE_SAVE_IMAGE,
	LC_FILE_IMPORT_LDD,
	LC_FILE_IMPORT_INVENTORY,
	LC_FILE_EXPORT_3DS,
	LC_FILE_EXPORT_COLLADA,
	LC_FILE_EXPORT_HTML,
	LC_FILE_EXPORT_BRICKLINK,
	LC_FILE_EXPORT_CSV,
	LC_FILE_EXPORT_POVRAY,
	LC_FILE_EXPORT_WAVEFRONT,
	LC_FILE_RENDER,
	LC_FILE_PRINT,
	LC_FILE_PRINT_PREVIEW,
	LC_FILE_PRINT_BOM,
	LC_FILE_RECENT_FIRST,
	LC_FILE_RECENT1 = LC_FILE_RECENT_FIRST,
	LC_FILE_RECENT2,
	LC_FILE_RECENT3,
	LC_FILE_RECENT4,
	LC_FILE_RECENT_LAST = LC_FILE_RECENT4,
	LC_FILE_EXIT,
	LC_EDIT_UNDO,
	LC_EDIT_REDO,
	LC_EDIT_CUT,
	LC_EDIT_COPY,
	LC_EDIT_PASTE,
	LC_EDIT_FIND,
	LC_EDIT_FIND_NEXT,
	LC_EDIT_FIND_PREVIOUS,
	LC_EDIT_SELECT_ALL,
	LC_EDIT_SELECT_NONE,
	LC_EDIT_SELECT_INVERT,
	LC_EDIT_SELECT_BY_NAME,
	LC_EDIT_SELECT_BY_COLOR,
	LC_EDIT_SELECTION_MODE_FIRST,
	LC_EDIT_SELECTION_SINGLE = LC_EDIT_SELECTION_MODE_FIRST,
	LC_EDIT_SELECTION_PIECE,
	LC_EDIT_SELECTION_COLOR,
	LC_EDIT_SELECTION_PIECE_COLOR,
	LC_EDIT_SELECTION_MODE_LAST = LC_EDIT_SELECTION_PIECE_COLOR,
	LC_EDIT_TRANSFORM_RELATIVE,
	LC_EDIT_SNAP_MOVE_TOGGLE,
	LC_EDIT_SNAP_MOVE_XY0,
	LC_EDIT_SNAP_MOVE_XY1,
	LC_EDIT_SNAP_MOVE_XY2,
	LC_EDIT_SNAP_MOVE_XY3,
	LC_EDIT_SNAP_MOVE_XY4,
	LC_EDIT_SNAP_MOVE_XY5,
	LC_EDIT_SNAP_MOVE_XY6,
	LC_EDIT_SNAP_MOVE_XY7,
	LC_EDIT_SNAP_MOVE_XY8,
	LC_EDIT_SNAP_MOVE_XY9,
	LC_EDIT_SNAP_MOVE_Z0,
	LC_EDIT_SNAP_MOVE_Z1,
	LC_EDIT_SNAP_MOVE_Z2,
	LC_EDIT_SNAP_MOVE_Z3,
	LC_EDIT_SNAP_MOVE_Z4,
	LC_EDIT_SNAP_MOVE_Z5,
	LC_EDIT_SNAP_MOVE_Z6,
	LC_EDIT_SNAP_MOVE_Z7,
	LC_EDIT_SNAP_MOVE_Z8,
	LC_EDIT_SNAP_MOVE_Z9,
	LC_EDIT_SNAP_ANGLE_TOGGLE,
	LC_EDIT_SNAP_ANGLE0,
	LC_EDIT_SNAP_ANGLE1,
	LC_EDIT_SNAP_ANGLE2,
	LC_EDIT_SNAP_ANGLE3,
	LC_EDIT_SNAP_ANGLE4,
	LC_EDIT_SNAP_ANGLE5,
	LC_EDIT_SNAP_ANGLE6,
	LC_EDIT_SNAP_ANGLE7,
	LC_EDIT_SNAP_ANGLE8,
	LC_EDIT_SNAP_ANGLE9,
	LC_EDIT_TRANSFORM,
	LC_EDIT_TRANSFORM_ABSOLUTE_TRANSLATION,
	LC_EDIT_TRANSFORM_RELATIVE_TRANSLATION,
	LC_EDIT_TRANSFORM_ABSOLUTE_ROTATION,
	LC_EDIT_TRANSFORM_RELATIVE_ROTATION,
	LC_EDIT_ACTION_FIRST,
	LC_EDIT_ACTION_INSERT = LC_EDIT_ACTION_FIRST,
	LC_EDIT_ACTION_LIGHT,
	LC_EDIT_ACTION_SPOTLIGHT,
	LC_EDIT_ACTION_CAMERA,
	LC_EDIT_ACTION_SELECT,
	LC_EDIT_ACTION_MOVE,
	LC_EDIT_ACTION_ROTATE,
	LC_EDIT_ACTION_DELETE,
	LC_EDIT_ACTION_PAINT,
	LC_EDIT_ACTION_ZOOM,
	LC_EDIT_ACTION_PAN,
	LC_EDIT_ACTION_ROTATE_VIEW,
	LC_EDIT_ACTION_ROLL,
	LC_EDIT_ACTION_ZOOM_REGION,
	LC_EDIT_ACTION_LAST = LC_EDIT_ACTION_ZOOM_REGION,
	LC_EDIT_CANCEL,
	LC_VIEW_PREFERENCES,
	LC_VIEW_ZOOM_IN,
	LC_VIEW_ZOOM_OUT,
	LC_VIEW_ZOOM_EXTENTS,
	LC_VIEW_LOOK_AT,
	LC_VIEW_MOVE_FORWARD,
	LC_VIEW_MOVE_BACKWARD,
	LC_VIEW_MOVE_LEFT,
	LC_VIEW_MOVE_RIGHT,
	LC_VIEW_MOVE_UP,
	LC_VIEW_MOVE_DOWN,
	LC_VIEW_VIEWPOINT_FRONT,
	LC_VIEW_VIEWPOINT_BACK,
	LC_VIEW_VIEWPOINT_TOP,
	LC_VIEW_VIEWPOINT_BOTTOM,
	LC_VIEW_VIEWPOINT_LEFT,
	LC_VIEW_VIEWPOINT_RIGHT,
	LC_VIEW_VIEWPOINT_HOME,
	LC_VIEW_CAMERA_NONE,
	LC_VIEW_CAMERA_FIRST,
	LC_VIEW_CAMERA1 = LC_VIEW_CAMERA_FIRST,
	LC_VIEW_CAMERA2,
	LC_VIEW_CAMERA3,
	LC_VIEW_CAMERA4,
	LC_VIEW_CAMERA5,
	LC_VIEW_CAMERA6,
	LC_VIEW_CAMERA7,
	LC_VIEW_CAMERA8,
	LC_VIEW_CAMERA9,
	LC_VIEW_CAMERA10,
	LC_VIEW_CAMERA11,
	LC_VIEW_CAMERA12,
	LC_VIEW_CAMERA13,
	LC_VIEW_CAMERA14,
	LC_VIEW_CAMERA15,
	LC_VIEW_CAMERA16,
	LC_VIEW_CAMERA_LAST = LC_VIEW_CAMERA16,
	LC_VIEW_CAMERA_RESET,
	LC_VIEW_TIME_FIRST,
	LC_VIEW_TIME_PREVIOUS,
	LC_VIEW_TIME_NEXT,
	LC_VIEW_TIME_LAST,
	LC_VIEW_TIME_INSERT_BEFORE,
	LC_VIEW_TIME_INSERT_AFTER,
	LC_VIEW_TIME_DELETE,
	LC_VIEW_TIME_ADD_KEYS,
	LC_VIEW_SPLIT_HORIZONTAL,
	LC_VIEW_SPLIT_VERTICAL,
	LC_VIEW_REMOVE_VIEW,
	LC_VIEW_RESET_VIEWS,
	LC_VIEW_FULLSCREEN,
	LC_VIEW_CLOSE_CURRENT_TAB,
	LC_VIEW_SHADING_FIRST,
	LC_VIEW_SHADING_WIREFRAME = LC_VIEW_SHADING_FIRST,
	LC_VIEW_SHADING_FLAT,
	LC_VIEW_SHADING_DEFAULT_LIGHTS,
	LC_VIEW_SHADING_LAST = LC_VIEW_SHADING_DEFAULT_LIGHTS,
	LC_VIEW_PROJECTION_FIRST,
	LC_VIEW_PROJECTION_PERSPECTIVE = LC_VIEW_PROJECTION_FIRST,
	LC_VIEW_PROJECTION_ORTHO,
	LC_VIEW_PROJECTION_LAST = LC_VIEW_PROJECTION_ORTHO,
	LC_VIEW_TOGGLE_VIEW_SPHERE,
	LC_PIECE_INSERT,
	LC_PIECE_DELETE,
	LC_PIECE_DUPLICATE,
	LC_PIECE_RESET_PIVOT_POINT,
	LC_PIECE_REMOVE_KEY_FRAMES,
	LC_PIECE_CONTROL_POINT_INSERT,
	LC_PIECE_CONTROL_POINT_REMOVE,
	LC_PIECE_MOVE_PLUSX,
	LC_PIECE_MOVE_MINUSX,
	LC_PIECE_MOVE_PLUSY,
	LC_PIECE_MOVE_MINUSY,
	LC_PIECE_MOVE_PLUSZ,
	LC_PIECE_MOVE_MINUSZ,
	LC_PIECE_ROTATE_PLUSX,
	LC_PIECE_ROTATE_MINUSX,
	LC_PIECE_ROTATE_PLUSY,
	LC_PIECE_ROTATE_MINUSY,
	LC_PIECE_ROTATE_PLUSZ,
	LC_PIECE_ROTATE_MINUSZ,
	LC_PIECE_MINIFIG_WIZARD,
	LC_PIECE_ARRAY,
	LC_PIECE_VIEW_SELECTED_MODEL,
	LC_PIECE_MOVE_SELECTION_TO_MODEL,
	LC_PIECE_INLINE_SELECTED_MODELS,
	LC_PIECE_EDIT_SELECTED_SUBMODEL,
	LC_PIECE_EDIT_END_SUBMODEL,
	LC_PIECE_GROUP,
	LC_PIECE_UNGROUP,
	LC_PIECE_GROUP_ADD,
	LC_PIECE_GROUP_REMOVE,
	LC_PIECE_GROUP_EDIT,
	LC_PIECE_HIDE_SELECTED,
	LC_PIECE_HIDE_UNSELECTED,
	LC_PIECE_UNHIDE_SELECTED,
	LC_PIECE_UNHIDE_ALL,
	LC_PIECE_SHOW_EARLIER,
	LC_PIECE_SHOW_LATER,
	LC_MODEL_NEW,
	LC_MODEL_PROPERTIES,
	LC_MODEL_LIST,
	LC_MODEL_FIRST,
	LC_MODEL_01 = LC_MODEL_FIRST,
	LC_MODEL_02,
	LC_MODEL_03,
	LC_MODEL_04,
	LC_MODEL_05,
	LC_MODEL_06,
	LC_MODEL_07,
	LC_MODEL_08,
	LC_MODEL_09,
	LC_MODEL_10,
	LC_MODEL_11,
	LC_MODEL_12,
	LC_MODEL_13,
	LC_MODEL_14,
	LC_MODEL_15,
	LC_MODEL_16,
	LC_MODEL_17,
	LC_MODEL_18,
	LC_MODEL_19,
	LC_MODEL_20,
	LC_MODEL_21,
	LC_MODEL_22,
	LC_MODEL_23,
	LC_MODEL_24,
	LC_MODEL_LAST = LC_MODEL_24,
	LC_HELP_HOMEPAGE,
	LC_HELP_BUG_REPORT,
	LC_HELP_UPDATES,
	LC_HELP_ABOUT,
	LC_TIMELINE_INSERT_BEFORE,
	LC_TIMELINE_INSERT_AFTER,
	LC_TIMELINE_DELETE,
	LC_TIMELINE_MOVE_SELECTION,
	LC_TIMELINE_SET_CURRENT,
	LC_NUM_COMMANDS
};

struct lcCommand
{
	const char* ID;
	const char* MenuName;
	const char* StatusText;
	const char* DefaultShortcut;
};

extern lcCommand gCommands[LC_NUM_COMMANDS];

enum lcTool
{
	LC_TOOL_INSERT,
	LC_TOOL_LIGHT,
	LC_TOOL_SPOTLIGHT,
	LC_TOOL_CAMERA,
	LC_TOOL_SELECT,
	LC_TOOL_MOVE,
	LC_TOOL_ROTATE,
	LC_TOOL_ERASER,
	LC_TOOL_PAINT,
	LC_TOOL_ZOOM,
	LC_TOOL_PAN,
	LC_TOOL_ROTATE_VIEW,
	LC_TOOL_ROLL,
	LC_TOOL_ZOOM_REGION,
	LC_NUM_TOOLS
};

extern const char* gToolNames[LC_NUM_TOOLS];

