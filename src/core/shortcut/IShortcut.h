
#include "core/Macros.h"
#include "core/Ref.h"

#include <vector>
NS_IGE_BEGIN

class IShortcut : public Ref
{
public:
	virtual void registerShortcut() = 0;
	virtual void unregisterShortcut() = 0;
	IShortcut();
	virtual ~IShortcut();
};

class IShortcutRegistration : public Ref
{
public:
	static IShortcutRegistration* getInstance();

	IShortcutRegistration();
	~IShortcutRegistration();

	void addObject(IShortcut* object);
	void clear();
	bool contains(IShortcut* object) const;
	void registerShortcuts();

private:
	static IShortcutRegistration* s_singleInstance;
	std::vector<IShortcut*> _managedObjectArray;

};

NS_IGE_END