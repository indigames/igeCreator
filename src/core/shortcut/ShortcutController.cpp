#include "core/shortcut/ShortcutController.h"
#include "core/Editor.h"
#include <functional>
#include <imgui.h>
NS_IGE_BEGIN

uint32_t ShortcutController::m_entryCounter = 0;


ShortcutController::ShortcutController()
{
	m_bIsEnable = false;
	m_activeEntry = nullptr;
	autorelease();
	retain();
	init();
}

ShortcutController::~ShortcutController()
{
	m_activeEntry = nullptr;
	m_foundTrigger.clear();
	if (m_indexedShortcutEntries.size() > 0) {
		for (auto& entries : m_indexedShortcutEntries) {
			for (auto* entry : entries.second) {
                if(entry->getReferenceCount() > 0)
                    entry->release();
			}
			entries.second.clear();
		}
	}
	m_indexedShortcutEntries.clear();
	m_defaultShortcutEntries.clear();
	m_entryCounter = 0;
}


bool ShortcutController::init()
{
	m_bIsEnable = true;
	Editor::getApp()->getInputHandler()->getKeyEventListener()->setOnKeyPressedCallback(std::bind(&ShortcutController::onKeyboardPressCallBack, this, std::placeholders::_1));
	Editor::getApp()->getInputHandler()->getKeyEventListener()->setOnKeyReleasedCallback(std::bind(&ShortcutController::onkeyboardReleaseCallBack, this, std::placeholders::_1));

	m_defaultShortcutEntries.reserve(150);

	return true;
}

void ShortcutController::update(float dt)
{
	updateFocus();
}

void ShortcutController::updateFocus()
{
	m_bIsLock = ImGui::IsAnyItemActive();
	
}

uint32_t ShortcutController::createShortcutEntry(const std::string& name, std::function<void()> callback) {
	return createShortcutEntry(name, ShortcutEntry::ShortcutStage::Begin, ShortcutEntry::ShortcutType::Action, callback);
}

uint32_t ShortcutController::createShortcutEntry(const std::string& name, ShortcutEntry::ShortcutStage stage, ShortcutEntry::ShortcutType type, std::function<void()> callback) {
	uint32_t id = m_entryCounter;
	ShortcutEntry* entry = new ShortcutEntry(id, name, stage, type, callback);
	m_defaultShortcutEntries[id] = entry;
	m_entryCounter++;
	return id;
}

void ShortcutController::removeShortcutEntry(uint32_t entryID) {
	ShortcutEntry* entry = m_defaultShortcutEntries[entryID];
	if (entry->getKeyCombination().size() > 0) {
		for (auto key : entry->getKeyCombination()) {
			auto subEntries = m_indexedShortcutEntries[(int)key.getKeyCode()];
			auto found = std::find(subEntries.begin(), subEntries.end(), entry);
			if (found != subEntries.end())
				subEntries.erase(found);
		}
	}
	entry->release();
	m_defaultShortcutEntries[entryID] = nullptr;
}

void ShortcutController::removeShortcutEntry(const std::string& name) {
	findShortcutEntryIDWithName(name, m_foundIds);
	if (m_foundIds.size() > 0) {
		for (uint32_t id : m_foundIds) {
			removeShortcutEntry(id);
		}
	}
}

void ShortcutController::findShortcutEntryIDWithName(const std::string& name, std::vector<uint32_t>& ids)
{
	ids.clear();
	for (auto entry : m_defaultShortcutEntries) {
		if (entry.second->getName() == name) {
			ids.push_back(entry.first);
		}
	}
}

void ShortcutController::assignKeyCombinationToEntry(uint32_t entryID, KeyCombination key) {
	ShortcutEntry* entry = m_defaultShortcutEntries[entryID];
	if (entry) {
		entry->addKeyCombination(key);
		m_defaultShortcutEntries[entryID] = entry;

		auto subEntries = m_indexedShortcutEntries[(int)key.getKeyCode()];
		subEntries.push_back(entry);
		m_indexedShortcutEntries[(int)key.getKeyCode()] = subEntries;
	}
}

void ShortcutController::assignKeyCombinationToEntry(uint32_t entryID, KeyCode key, bool isAlt, bool isCtrl, bool isShift) {
	if (key == KeyCode::KEY_NOKEY) return;
	int keymodifier = 0;
	if (isAlt) keymodifier |= SYSTEM_KEYCODE_ALT_MASK;
	if (isCtrl) keymodifier |= SYSTEM_KEYCODE_CTRL_MASK;
	if (isShift) keymodifier |= SYSTEM_KEYCODE_SHIFT_MASK;
	KeyCombination keyCombination = KeyCombination(key, keymodifier);
	assignKeyCombinationToEntry(entryID, keyCombination);
}

void ShortcutController::assignKeyCombinationToCommand(const std::string& name, KeyCombination key) {
	findShortcutEntryIDWithName(name, m_foundIds);
	if (m_foundIds.size() > 0) {
		ShortcutEntry* entry = m_defaultShortcutEntries[m_foundIds[0]];
		if (entry) {
			entry->addKeyCombination(key);
			m_defaultShortcutEntries[m_foundIds[0]] = entry;

			auto subEntries = m_indexedShortcutEntries[(int)key.getKeyCode()];
			subEntries.push_back(entry);
			m_indexedShortcutEntries[(int)key.getKeyCode()] = subEntries;
		}
	}
}

void ShortcutController::assignKeyCombinationToCommand(const std::string& name, KeyCode key, bool isAlt, bool isCtrl, bool isShift) {
	if (key == KeyCode::KEY_NOKEY) return;
	int keymodifier = 0;
	if (isAlt) keymodifier |= SYSTEM_KEYCODE_ALT_MASK;
	if (isCtrl) keymodifier |= SYSTEM_KEYCODE_CTRL_MASK;
	if (isShift) keymodifier |= SYSTEM_KEYCODE_SHIFT_MASK;
	KeyCombination keyCombination = KeyCombination(key, keymodifier);
	assignKeyCombinationToCommand(name, keyCombination);
}

void ShortcutController::addShortcut(KeyCode key, bool alt, bool ctr, bool shift, ShortcutEntry::ShortcutStage stage, ShortcutEntry::ShortcutType type, std::function<void()> callback)
{
	if (key == KeyCode::KEY_NOKEY) return;
	int keymodifier = 0;
	if (alt) keymodifier |= SYSTEM_KEYCODE_ALT_MASK;
	if (ctr) keymodifier |= SYSTEM_KEYCODE_CTRL_MASK;
	if (shift) keymodifier |= SYSTEM_KEYCODE_SHIFT_MASK;
	KeyCombination keyCombination = KeyCombination(key, keymodifier);
	auto subEntries = m_indexedShortcutEntries[(int)key];
	int id = ((int)key) * 10 + keymodifier;
	ShortcutEntry* entry = new ShortcutEntry(id, "", stage, type, callback);
	entry->addKeyCombination(keyCombination);
	subEntries.push_back(entry);
	m_indexedShortcutEntries[(int)key] = subEntries;
}


void ShortcutController::onKeyboardPressCallBack(const std::shared_ptr<pyxie::Event>& evt)
{
	if (evt->getType() == pyxie::Event::Type::KEYBOARD) {
		auto keyEvent = (const pyxie::KeyboardEvent*)evt.get();
		KeyCode keyCode = keyEvent->getKeyCode();
		int currentModifier = m_currentKeyCombination.getModifiers();
		switch (keyCode)
		{
		case pyxie::KeyCode::KEY_NOKEY:
			break;
		case pyxie::KeyCode::KEY_LALT:
		case pyxie::KeyCode::KEY_RALT:
			if((currentModifier & SYSTEM_KEYCODE_ALT_MASK) != SYSTEM_KEYCODE_ALT_MASK)
				currentModifier |= SYSTEM_KEYCODE_ALT_MASK;
			break;
		case pyxie::KeyCode::KEY_LCTRL:
		case pyxie::KeyCode::KEY_RCTRL:
			if ((currentModifier & SYSTEM_KEYCODE_ALT_MASK) != SYSTEM_KEYCODE_CTRL_MASK)
				currentModifier |= SYSTEM_KEYCODE_CTRL_MASK;
			break;
		case pyxie::KeyCode::KEY_LSHIFT:
		case pyxie::KeyCode::KEY_RSHIFT:
			if ((currentModifier & SYSTEM_KEYCODE_ALT_MASK) != SYSTEM_KEYCODE_SHIFT_MASK)
				currentModifier |= SYSTEM_KEYCODE_SHIFT_MASK;
			break;
		default:
			if (m_currentKeyCombination.getKeyCode() != keyCode) {
				// Store Current Combination
				m_currentKeyCombination.setKeyCode(keyCode);

				//! Trigger Begin shortcut Entry
				triggerKeyCombination(m_currentKeyCombination, ShortcutEntry::ShortcutStage::Begin);
			}
			break;
		}
		m_currentKeyCombination.setModifiers(currentModifier);
	}
}

void ShortcutController::onkeyboardReleaseCallBack(const std::shared_ptr<pyxie::Event>& evt)
{
	if (evt->getType() == pyxie::Event::Type::KEYBOARD) {
		auto keyEvent = (const pyxie::KeyboardEvent*)evt.get();
		KeyCode keyCode = keyEvent->getKeyCode();
		int currentModifier = m_currentKeyCombination.getModifiers();
		switch (keyCode)
		{
		case pyxie::KeyCode::KEY_NOKEY:
			break;
		case pyxie::KeyCode::KEY_LALT:
		case pyxie::KeyCode::KEY_RALT:
			currentModifier ^= SYSTEM_KEYCODE_ALT_MASK;
			break;
		case pyxie::KeyCode::KEY_LCTRL:
		case pyxie::KeyCode::KEY_RCTRL:
			currentModifier ^= SYSTEM_KEYCODE_CTRL_MASK;
			break;
		case pyxie::KeyCode::KEY_LSHIFT:
		case pyxie::KeyCode::KEY_RSHIFT:
			currentModifier ^= SYSTEM_KEYCODE_SHIFT_MASK;
			break;
		default:
			if (m_currentKeyCombination.getKeyCode() == keyCode) {
				//Trigger End Entry
				triggerKeyCombination(m_currentKeyCombination, ShortcutEntry::ShortcutStage::End);
				//Reset current Key
				m_currentKeyCombination.setKeyCode(KeyCode::KEY_NOKEY);
			}
			break;
		}
		m_currentKeyCombination.setModifiers(currentModifier);
	}
}

void ShortcutController::triggerKeyCombination(KeyCombination key, ShortcutEntry::ShortcutStage state)
{
	if (m_bIsLock) return;
	if (findShortcutEntries(key, state, ShortcutEntry::ShortcutType::Action, m_foundTrigger)) {
		for (auto entry : m_foundTrigger) {
			entry->execute();
		}
	}
}

bool ShortcutController::findShortcutEntries(KeyCombination keySequence, ShortcutEntry::ShortcutStage stage, ShortcutEntry::ShortcutType type, std::vector<ShortcutEntry*> &entries)
{
	entries.clear();
	auto subEntries = m_indexedShortcutEntries[(int)keySequence.getKeyCode()];
	if(subEntries.size() == 0)
		return false;	
	bool kq = false;
	for (auto entry : subEntries) {
		if (entry->startsWith(keySequence) && stage == entry->getShortcutStage() && type == entry->getShortcutType()) {
			entries.push_back(entry);
			kq = true;
		}
	}
	return kq;
}

NS_IGE_END
