#include "core/shortcut/ShortcutEntry.h"

NS_IGE_BEGIN

KeyCombination::KeyCombination() {
	m_keycode = KeyCode::KEY_NOKEY;
	m_modifiers = 0;
}
KeyCombination::KeyCombination(KeyCode key, int shortcutModifiers) 
	: m_keycode(key),
	m_modifiers(shortcutModifiers)
{
}

KeyCombination::~KeyCombination() {

}

bool KeyCombination::isAlt() {
	return ((m_modifiers & SYSTEM_KEYCODE_ALT_MASK) == SYSTEM_KEYCODE_ALT_MASK);
}

bool KeyCombination::isCtrl() {
	return ((m_modifiers & SYSTEM_KEYCODE_CTRL_MASK) == SYSTEM_KEYCODE_CTRL_MASK);
}

bool KeyCombination::isShift() {
	return ((m_modifiers & SYSTEM_KEYCODE_SHIFT_MASK) == SYSTEM_KEYCODE_SHIFT_MASK);
}

void KeyCombination::setKeyCode(KeyCode key) {
	m_keycode = key;
}

void KeyCombination::setModifiers(int modifiers) {
	m_modifiers = modifiers;
}

//std::string KeyCombination::toString() {
//	std::string keyCodeString;
//
//}


bool KeyCombination::operator==(const KeyCombination& k) const
{
	return (m_keycode == k.m_keycode && m_modifiers == k.m_modifiers);
}

bool KeyCombination::operator!=(const KeyCombination& k) const
{
	return !(*this == k);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////            Shortcut Entry           ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

//ShortcutEntry::ShortcutEntry()
//	: m_id(-1),
//	m_shortcutStage(ShortcutStage::Begin),
//	m_shortcutType(ShortcutType::Action)
//{
//	m_keyCombinations.reserve(10);
//	m_callback = nullptr;
//}
ShortcutEntry::ShortcutEntry(uint32_t id, const std::string& name, ShortcutStage stage, ShortcutType type, Callback callback)
	: m_id(id),
	m_name(name),
	m_shortcutStage(stage),
	m_shortcutType(type)
{
	m_keyCombinations.reserve(10);
	m_callback = callback;
	retain();
}

ShortcutEntry::~ShortcutEntry() {
	m_keyCombinations.clear();
	m_callback = nullptr;
}

bool ShortcutEntry::addKeyCombination(KeyCombination keyCombination)
{
	if (m_keyCombinations.size() == 10) return false;
	
	auto it = std::find(m_keyCombinations.begin(), m_keyCombinations.end(), keyCombination);
	if (it != m_keyCombinations.end())
		return false;
	m_keyCombinations.push_back(keyCombination);
	return true;
}

bool ShortcutEntry::removeKeyCombination(KeyCombination keyCombination) {
	auto it = std::find(m_keyCombinations.begin(), m_keyCombinations.end(), keyCombination);
	if (it != m_keyCombinations.end())
		m_keyCombinations.erase(it);
	return false;
}

bool ShortcutEntry::startsWith(KeyCombination prefix)
{
	if (m_keyCombinations.size() == 0) return false;
	
	auto it = std::find(m_keyCombinations.begin(), m_keyCombinations.end(), prefix);
	if (it != m_keyCombinations.end())
		return true;
	return false;
}

void ShortcutEntry::execute() {
	if (m_callback == nullptr) return;
	m_callback();
}

NS_IGE_END