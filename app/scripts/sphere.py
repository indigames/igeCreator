"""
    Sphere.py
    Script to control sphere object
"""

from igeScene import Script

class Sphere(Script):
    def __init__(self, owner):
        super().__init__(owner)
        print("__init__!")
        self.updated = False

    def onStart(self):
        print("onStart")
        print(f'Owner: {self.owner}')

        body = self.owner.getComponent("PhysicSphere")
        print(f'applyForce, body: {body}')
        if body is not None:
            print(f'applyForce, body: {body}')
            body.applyForce(0, 0, -1000.0)
            print('applyForce OK')
        navAgent = self.owner.getComponent("NavAgent")
        if navAgent is not None:
            print(f'agent: {navAgent}')
            navAgent.targetPosition = (0, 0, -30.0)
            print(f'agent pos: {navAgent.targetPosition}')
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
