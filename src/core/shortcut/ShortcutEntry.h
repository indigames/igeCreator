#pragma once
#ifndef __SHORTCUT_ENTRY_H__
#define __SHORTCUT_ENTRY_H__

#include <utils/PyxieHeaders.h>
#include "core/Macros.h"
#include "core/Ref.h"

#include <string>
#include <vector>
#include <functional>
#include <event/Event.h>

USING_NS_PYXIE
USING_NS_IGE_SCENE
NS_IGE_BEGIN

class KeyCombination
{
public:
	KeyCombination();
	KeyCombination(KeyCode key, int shortcutModifiers = 0);
	~KeyCombination();

	void setKeyCode(KeyCode key);
	KeyCode getKeyCode() { return m_keycode; }

	void setModifiers(int modifiers);
	int getModifiers() { return m_modifiers; }

	//! OnMacOS => Options
	bool isAlt();
	//! OnMacOS => Command
	bool isCtrl();
	bool isShift();

	//std::string toString();

	bool Equals(KeyCombination other)
	{
		return m_keycode == other.m_keycode && m_modifiers == other.m_modifiers;
	}

	inline bool operator==(const KeyCombination& k) const;
	inline bool operator!=(const KeyCombination& k) const;
protected:

	KeyCode m_keycode;
	int m_modifiers;
};

class ShortcutEntry : public Ref
{
public:
	enum ShortcutStage
	{
		Begin		= 0,		//! Action on press
		End			= 1, 		//! Action on release
	};

	enum ShortcutType
	{
		//! Use for function which no need target
		Action		= 0,
		//! Use for function need a target (Focus, Snap ...)
		Clutch		= 1,
		//! Use for menu
		Menu		= 2,
	};

	using Callback = std::function<void()>;

	ShortcutEntry(uint32_t id, const std::string& name, ShortcutStage stage, ShortcutType type, Callback callback);
	virtual ~ShortcutEntry();

	inline uint32_t getId() const { return m_id; }
	const std::string getName() const { return m_name; }
	const std::vector<KeyCombination> getKeyCombination() const { return m_keyCombinations; }
	ShortcutStage getShortcutStage() { return m_shortcutStage; }
	ShortcutType getShortcutType() { return m_shortcutType; }

	bool addKeyCombination(KeyCombination keyCombination);
	bool removeKeyCombination(KeyCombination keyCombination);

	bool startsWith(KeyCombination prefix);

	void execute();
protected:
	uint32_t m_id;
	//! Entry name
	std::string m_name;
	ShortcutStage m_shortcutStage;
	ShortcutType m_shortcutType;
	std::function<void()> m_callback;

	//! viet.nv: Different KeyCombinations can call same function, so we store as a list
	std::vector<KeyCombination> m_keyCombinations;
};

NS_IGE_END

#endif // __SHORTCUT_ENTRY_H__