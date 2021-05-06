"""
    Hello.py
    Script to print hello
"""

import igeCore as core
import igeVmath as vmath
import igeScene as scene
from igeScene import Script

class Hello(Script):
    def __init__(self, owner):
        super().__init__(owner)
        self.updated = False

    def onAwake(self):
        print("onAwake")

    def onStart(self):
        print("onStart")

    def onUpdate(self, dt):
        if not self.updated:
            print(f'onUpdate, dt = {dt}')
            print(f'owner, name = {self.owner.name}')
            self.updated = True

    def onRender(self):
        print("onRender")

    def onDestroy(self):
        print("onDestroy")
