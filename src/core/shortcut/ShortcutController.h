#pragma once
#ifndef __SHORTCUT_CONTROLLER_H__
#define __SHORTCUT_CONTROLLER_H__

#include <utils/PyxieHeaders.h>
#include "core/Macros.h"
#include "core/Ref.h"
#include "core/shortcut/ShortcutEntry.h"
#include "core/shortcut/ShortcutDictionary.h"

#include <unordered_map>
#include <vector>

USING_NS_PYXIE

NS_IGE_BEGIN

class ShortcutController : public Ref
{
public:	
	void update(float dt);

	void setEnable(bool active) { m_bIsEnable = active; }
	bool isEnable() { return m_bIsEnable; }

	void addShortcut(KeyCode key, bool alt, bool ctr, bool shift, ShortcutEntry::ShortcutStage stage, ShortcutEntry::ShortcutType type, std::function<void()> callback);

	uint32_t createShortcutEntry(const std::string& name, std::function<void()> callback);
	uint32_t createShortcutEntry(const std::string& name, ShortcutEntry::ShortcutStage stage, ShortcutEntry::ShortcutType type, std::function<void()> callback);

	void removeShortcutEntry(uint32_t entryID);
	void removeShortcutEntry(const std::string& name);

	void assignKeyCombinationToEntry(uint32_t entryID, KeyCombination key);
	void assignKeyCombinationToEntry(uint32_t entryID, KeyCode key, bool isAlt, bool isCtrl, bool isShift);
	void assignKeyCombinationToCommand(const std::string& targetCommand, KeyCombination key);
	void assignKeyCombinationToCommand(const std::string& targetCommand, KeyCode key, bool isAlt, bool isCtrl, bool isShift);
	
	void findShortcutEntryIDWithName(const std::string& name, std::vector<uint32_t> &ids);
	//void removeShortcut(KeyCode key, bool alt, bool ctr, bool shift);

	ShortcutController();
	virtual ~ShortcutController();

protected:
	bool init();
	//! Update Current Imgui Focus Object 
	void updateFocus();
	

	void onKeyboardPressCallBack(const std::shared_ptr<pyxie::Event>& evt);
	void onkeyboardReleaseCallBack(const std::shared_ptr<pyxie::Event>& evt);

	void triggerKeyCombination(KeyCombination key, ShortcutEntry::ShortcutStage state);

	//! find List ShortcutEntries match with keySquence
	bool findShortcutEntries(KeyCombination keySequence, ShortcutEntry::ShortcutStage stage, ShortcutEntry::ShortcutType type, std::vector<ShortcutEntry*> &entries);
protected:
	static uint32_t m_entryCounter;
	
	bool m_bIsEnable = true;
	bool m_bIsLock = false;
	KeyCombination m_currentKeyCombination;
	ShortcutEntry* m_activeEntry;
	std::unordered_map<uint32_t, std::vector<ShortcutEntry*>> m_indexedShortcutEntries;
	std::vector<ShortcutEntry*> m_foundTrigger;
	std::vector<uint32_t> m_foundIds;
	std::unordered_map<uint32_t, ShortcutEntry*> m_defaultShortcutEntries;
};


#define ASSIGN_COMMAND_TO_DICT(K, V)						Editor::getShortcut()->createShortcutEntry(K, V);
#define ASSIGN_KEY_TO_ENTRY(K, V)							Editor::getShortcut()->assignKeyCombinationToEntry(K, V);
#define ASSIGN_KEY_TO_ENTRY(K, V, ALT, CTRL, SHIFT)			Editor::getShortcut()->assignKeyCombinationToEntry(K, V, ALT, CTRL, SHIFT);
#define ASSIGN_KEY_TO_COMMAND(K, V)							Editor::getShortcut()->assignKeyCombinationToCommand(K, V);
#define ASSIGN_KEY_TO_COMMAND(K, V, ALT, CTRL, SHIFT)		Editor::getShortcut()->assignKeyCombinationToCommand(K, V, ALT, CTRL, SHIFT);

#define REMOVE_COMMAND(K)									Editor::getShortcut()->removeShortcutEntry(K);
NS_IGE_END


#endif