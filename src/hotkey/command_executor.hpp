/*
	Copyright (C) 2003 - 2025
	by David White <dave@whitevine.net>
	Part of the Battle for Wesnoth Project https://www.wesnoth.org/

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY.

	See the COPYING file for more details.
*/

#pragma once

#include "hotkey_command.hpp"

#include <SDL2/SDL_events.h>

class display;

namespace hotkey {

enum ACTION_STATE { ACTION_STATELESS, ACTION_ON, ACTION_OFF, ACTION_SELECTED, ACTION_DESELECTED };

/// Used as the main parameter for can_execute_command/do_execute_command
/// These functions are used to execute hotkeys but also to execute menu items,
/// (Most menu items point to the same action as a hotkey but not all)
struct ui_command
{
	/// The hotkey::HOTKEY_COMMAND associated with this action, HOTKEY_NULL for actions that don't allow hotkey binding.
	/// different actions of the ame type might have the same HOTKEY_COMMAND (like different wml menu items that allow
	/// hotkey bindings.). This is preferred to be used for comparision over id (for example being an enum makes it
	/// impossible to make typos in the id and its faster, plus c++ unfortunately doesn't allow switch statements with
	/// strings)
	hotkey::HOTKEY_COMMAND hotkey_command;
	/// The string command, never empty, describes the action uniquely. when the action is the result of a menu click
	/// this matches the id element of the clicked item (the id parameter of show_menu)
	std::string id;
	/// When this action was the result of a menu click, this is the index of the clicked item in the menu.
	int index;
	ui_command(hotkey::HOTKEY_COMMAND hotkey_command, std::string_view id, int index = -1)
		: hotkey_command(hotkey_command)
		, id(id)
		, index(index)
	{ }
	explicit ui_command(const hotkey::hotkey_command& cmd, int index = -1)
		: ui_command(cmd.command, cmd.id, index)
	{ }
	// the string @param id references must live longer than this object.
	explicit ui_command(std::string_view id, int index = -1)
		: ui_command(hotkey::HOTKEY_NULL, id, index)
	{
		hotkey_command = hotkey::get_hotkey_command(std::string(id)).command;
	}
};

// Abstract base class for objects that implement the ability
// to execute hotkey commands.
class command_executor
{

protected:
	virtual ~command_executor() {}

public:
	virtual void cycle_units() {}
	virtual void cycle_back_units() {}
	virtual void end_turn() {}
	virtual void goto_leader() {}
	virtual void unit_hold_position() {}
	virtual void end_unit_turn() {}
	virtual void undo() {}
	virtual void redo() {}
	virtual void terrain_description() {}
	virtual void unit_description() {}
	virtual void rename_unit() {}
	virtual void save_game() {}
	virtual void save_replay() {}
	virtual void save_map() {}
	virtual void load_game() {}
	virtual void toggle_ellipses() {}
	virtual void toggle_grid() {}
	virtual void status_table() {}
	virtual void recall() {}
	virtual void recruit() {}
	virtual void repeat_recruit() {}
	virtual void speak() {}
	virtual void whisper() {}
	virtual void shout() {}
	virtual void create_unit() {}
	virtual void change_side() {}
	virtual void kill_unit() {}
	virtual void preferences() {}
	virtual void objectives() {}
	virtual void unit_list() {}
	virtual void show_statistics() {}
	virtual void stop_network() {}
	virtual void start_network() {}
	virtual void label_terrain(bool /*team_only*/) {}
	virtual void clear_labels() {}
	virtual void label_settings() {}
	virtual void show_enemy_moves(bool /*ignore_units*/) {}
	virtual void toggle_shroud_updates() {}
	virtual void update_shroud_now() {}
	virtual void continue_move() {}
	virtual void search() {}
	virtual void show_help() {}
	virtual void show_chat_log() {}
	virtual void user_command() {}
	virtual void custom_command() {}
	virtual void ai_formula() {}
	virtual void clear_messages() {}
	virtual void change_language() {}
	virtual void play_replay() {  }
	virtual void reset_replay() {}
	virtual void stop_replay() {}
	virtual void replay_next_turn() {  }
	virtual void replay_next_side() {  }
	virtual void replay_next_move() {  }
	virtual void replay_show_everything() {}
	virtual void replay_show_each() {}
	virtual void replay_show_team1() {}
	virtual void replay_skip_animation() {}
	virtual void replay_exit() {}
	virtual void whiteboard_toggle() {}
	virtual void whiteboard_execute_action() {}
	virtual void whiteboard_execute_all_actions() {}
	virtual void whiteboard_delete_action() {}
	virtual void whiteboard_bump_up_action() {}
	virtual void whiteboard_bump_down_action() {}
	virtual void whiteboard_suppose_dead() {}
	virtual void select_hex() {}
	virtual void deselect_hex() {}
	virtual void move_action() {}
	virtual void select_and_action() {}
	virtual void select_teleport() {}
	virtual void touch_hex() {}
	virtual void toggle_accelerated_speed() {}
	virtual void scroll_up(bool /*on*/) {}
	virtual void scroll_down(bool /*on*/) {}
	virtual void scroll_left(bool /*on*/) {}
	virtual void scroll_right(bool /*on*/) {}
	virtual void lua_console();
	virtual void zoom_in() {}
	virtual void zoom_out() {}
	virtual void zoom_default() {}
	virtual void map_screenshot() {}
	virtual void surrender_quit_game() {}

	virtual void set_button_state() {}
	virtual void recalculate_minimap() {}

	// execute_command's parameter is changed to "hotkey_command& command" and this not maybe that is too inconsistent.
	// Gets the action's image (if any). Displayed left of the action text in menus.
	virtual std::string get_action_image(const hotkey::ui_command&) const { return ""; }
	// Does the action control a toggle switch? If so, return the state of the action (on or off).
	virtual ACTION_STATE get_action_state(const hotkey::ui_command&) const { return ACTION_STATELESS; }
	// Returns the appropriate menu image. Checkable items will get a checked/unchecked image.
	std::string get_menu_image(const std::string& command, int index=-1) const;
	// Returns a vector of images for a given menu.
	void get_menu_images(std::vector<config>& items);
	void surrender_game();
	// @a items_arg the items in the menus to be shows, each item can have the folliwng attributes:
	//   'id':    The id describing the action, will be passed to do_execute_commnd and can_execute_commnd,
	//            If 'id' specifies a known hotkey command or theme item the other attributes can be generated from it.
	//   'label': The label for this menu entry.
	//   'icon':  The icon for this menu entry.
	virtual void show_menu(const std::vector<config>& items_arg, int xloc, int yloc, bool context_menu);
	// @a items_arg the actions to be exceuted, exceutes all of the actions, it looks like the idea is to associate
	//  multiple actions with a single menu button, not sure whether it is actually used.
	void execute_action(const std::vector<std::string>& items_arg);

	virtual bool can_execute_command(const hotkey::ui_command& command) const = 0;
	void queue_command(const SDL_Event& event, int index = -1);
	bool run_queued_commands();
	void execute_quit_command()
	{
		const hotkey_command& quit_hotkey = hotkey_command::get_command_by_command(hotkey::HOTKEY_QUIT_GAME);
		do_execute_command(ui_command(quit_hotkey));
	}

	void handle_keyup()
	{
		press_event_sent_ = false;
	}

protected:
	virtual bool do_execute_command(const hotkey::ui_command& command, bool press=true, bool release=false);

private:
	struct queued_command
	{
		queued_command(const hotkey_command& command_, int index_, bool press_, bool release_)
			: command(&command_), index(index_), press(press_), release(release_)
		{}

		const hotkey_command* command;
		int index;
		bool press;
		bool release;
	};

	void execute_command_wrap(const queued_command& command);
	std::vector<queued_command> filter_command_queue();

	bool press_event_sent_ = false;
	std::vector<queued_command> command_queue_;
};
class command_executor_default : public command_executor
{
protected:
	virtual display& get_display() = 0;
public:
	void set_button_state();
	void recalculate_minimap();
	void lua_console();
	void zoom_in();
	void zoom_out();
	void zoom_default();
	void map_screenshot();
};
/* Functions to be called every time a event is intercepted.
 * Will call the relevant function in executor if the event is not nullptr.
 * Also handles some events in the function itself,
 * and so is still meaningful to call with executor=nullptr
 */
void jbutton_event(const SDL_Event& event, command_executor* executor);
void jhat_event(const SDL_Event& event, command_executor* executor);
void key_event(const SDL_Event& event, command_executor* executor);
void keyup_event(const SDL_Event& event, command_executor* executor);
void mbutton_event(const SDL_Event& event, command_executor* executor);
// Function to call to process the events.
void run_events(command_executor* executor);

}
