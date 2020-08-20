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

    def onUpdate(self, dt):
        if not self.updated:
            print(f'onUpdate, dt = {dt}')
            print(f'owner, name = {self.owner.name}')
            self.updated = True

