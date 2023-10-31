/*
 *  arduino-menusystem - A flexible and extensible menu system for Arduino.
 *
 *  arduino-menusystem is licensed under the MIT license.
 *  See LICENSE file for details.
 */

#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include <Arduino.h>

class Menu;
class MenuComponentRenderer;
class MenuSystem;

// Abstract base class representing a component in the menu structure.
class MenuComponent {
	friend class MenuSystem;
	friend class Menu;

public:
	// Callback for when the MenuComponent is selected.
	using SelectFnPtr = void (*)(MenuComponent *menu_component);

public:
	// Construct a MenuComponent.
	MenuComponent(const char *name, const char *icon, SelectFnPtr select_fn);

	// Set the component's name.
	void set_name(const char *name);

	// Get the component's name.
	const char *get_name() const;

	// Get the component's icon.
	const char *get_icon() const;

	// Render the component using the given MenuComponentRenderer.
	virtual void render(MenuComponentRenderer const &renderer) const = 0;

	// Returns true if this component has focus; false otherwise.
	bool has_focus() const;

	// Set the focus state of the component
	void set_focus(bool hasFocus);

	// Returns true if this is the current component; false otherwise.
	bool is_current() const;

	// Sets the function to call when the MenuItem is selected.
	void set_select_function(SelectFnPtr select_fn);

protected:
	// Processes the next action.
	virtual bool next(bool loop = false) = 0;

	// Processes the prev action.
	virtual bool prev(bool loop = false) = 0;

	// Resets the component to its initial state.
	virtual void reset() = 0;

	// Processes the select action.
	virtual Menu *select();

	// Set the current state of the component.
	void set_current(bool is_current = true);

protected:
	SelectFnPtr _select_fn;
	const char *_name;
	const char *_icon;
	bool _has_focus;
	bool _is_current;
};

// A MenuComponent that calls a callback function when selected.
class MenuItem : public MenuComponent {
public:
	// Construct a MenuItem.
	MenuItem(const char *name, const char *icon, SelectFnPtr select_fn);

	// Render the MenuItem.
	virtual void render(MenuComponentRenderer const &renderer) const;

protected:
	// This method does nothing in MenuItem.
	virtual bool next(bool loop = false);

	// This method does nothing in MenuItem.
	virtual bool prev(bool loop = false);

	// This method does nothing in MenuItem.
	virtual void reset();

	virtual Menu *select();
};

// A MenuItem that calls MenuSystem::back() when selected.
class BackMenuItem : public MenuItem {
public:
	BackMenuItem(const char *name, const char *icon, SelectFnPtr select_fn, MenuSystem *ms);

	virtual void render(MenuComponentRenderer const &renderer) const;

protected:
	virtual Menu *select();

protected:
	MenuSystem *_menu_system;
};

class NumericMenuItem : public MenuItem {
public:
	// Callback for formatting the numeric value into a String.
	using FormatValueFnPtr = const String (*)(const float value);

public:
	// Constructor.
	NumericMenuItem(const char *name, const char *icon, SelectFnPtr select_fn, float value, float min_value, float max_value,
					float increment = 1.0, FormatValueFnPtr format_value_fn = nullptr);

	// Sets the custom number formatter.
	void set_number_formatter(FormatValueFnPtr format_value_fn);

	float get_value() const;
	float get_min_value() const;
	float get_max_value() const;

	void set_value(float value);
	void set_min_value(float value);
	void set_max_value(float value);

	String get_formatted_value() const;

	virtual void render(MenuComponentRenderer const &renderer) const;

protected:
	virtual bool next(bool loop = false);
	virtual bool prev(bool loop = false);

	virtual Menu *select();

protected:
	float _value;
	float _min_value;
	float _max_value;
	float _increment;
	FormatValueFnPtr _format_value_fn;
};

// A MenuComponent that can contain other MenuComponents.
class Menu : public MenuComponent {
	friend class MenuSystem;

public:
	Menu(const char *name, const char *icon, SelectFnPtr select_fn = nullptr);

	// Adds a MenuItem to the Menu.
	void add_item(MenuItem *p_item);

	// Adds a Menu to the Menu.
	void add_menu(Menu *p_menu);

	MenuComponent const *get_current_component() const;
	MenuComponent const *get_menu_component(uint8_t index) const;

	uint8_t get_num_components() const;
	uint8_t get_current_component_num() const;
	uint8_t get_previous_component_num() const;

	void render(MenuComponentRenderer const &renderer) const;

protected:
	void set_parent(Menu *p_parent);
	Menu const *get_parent() const;

	// Activates the current selection.
	Menu *activate();

	virtual bool next(bool loop = false);
	virtual bool prev(bool loop = false);
	virtual Menu *select();
	virtual void reset();

	void add_component(MenuComponent *p_component);

private:
	MenuComponent *_p_current_component;
	MenuComponent **_menu_components;
	Menu *_p_parent;
	uint8_t _num_components;
	uint8_t _current_component_num;
	uint8_t _previous_component_num;
};

class MenuSystem {
public:
	MenuSystem(MenuComponentRenderer const &renderer);

	void display() const;
	bool next(bool loop = false);
	bool prev(bool loop = false);
	void reset();
	void select(bool reset = false);
	bool back();

	Menu &get_root_menu() const;
	Menu const *get_current_menu() const;

private:
	Menu *_p_root_menu;
	Menu *_p_curr_menu;
	MenuComponentRenderer const &_renderer;
};

class MenuComponentRenderer {
public:
	virtual void render(Menu const &menu) const = 0;
	virtual void render_menu_item(MenuItem const &menu_item) const = 0;
	virtual void render_back_menu_item(BackMenuItem const &menu_item) const = 0;
	virtual void render_numeric_menu_item(NumericMenuItem const &menu_item) const = 0;
	virtual void render_menu(Menu const &menu) const = 0;
};

#endif