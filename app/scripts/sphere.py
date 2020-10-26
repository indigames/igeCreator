"""
    Hello.py
    Script to print hello
"""

import igeCore as core
import igeVmath as vmath
from igeScene import Script

class Hello(Script):
    def __init__(self, owner):
        super().__init__(owner)
        print("__init__!")
        self.updated = False

    def onStart(self):
        print("onStart")
        print(f'Owner: {self.owner}')
        body = self.owner.getComponent("PhysicSphere")
        if body is not None:
            body.applyForce(0, 0, -1000.0)


    def onUpdate(self, dt):
        if not self.updated:
            print(f'onUpdate, dt = {dt}')
            print(f'owner, name = {self.owner.name}')
            self.updated = True

    # def onRender(self):
    #     pass

    # def onCollisionStart(self, other):
    #     pass

    # def onCollisionStay(self, other):
    #     pass

    # def onCollisionStop(self, other):
    #     pass

    # def onTriggerStart(self, other):
    #     pass

    # def onTriggerStay(self, other):
    #     pass

    # def onTriggerStop(self, other):
    #     pass
