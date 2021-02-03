
#include "core/Macros.h"
#include "core/Ref.h"

#include <vector>
NS_IGE_BEGIN

class IShortcut : public Ref
{
public:
	IShortcut();
	virtual ~IShortcut();

	virtual void registerShortcut() = 0;
	virtual void unregisterShortcut() = 0;
};

class ShortcutRegistration : public Ref
{
public:
	static ShortcutRegistration* getInstance();

	ShortcutRegistration();
	~ShortcutRegistration();

	void addObject(IShortcut* object);
	void clear();
	bool contains(IShortcut* object) const;
	void registerShortcuts();

private:
	static ShortcutRegistration* s_singleInstance;
	std::vector<IShortcut*> _managedObjectArray;

};

NS_IGE_END