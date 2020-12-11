"""
    Sphere.py
    Script to control sphere object
"""

import igeCore as core

import igeScene
from igeScene import Script, SceneManager, Scene


class Sphere(Script):
    def __init__(self, owner):
        super().__init__(owner)
        print("__init__!")
        self.updated = False
        self.navAgent = None
        self.scene = None

    def onStart(self):
        print("onStart")
        print(f'Owner: {self.owner}')

        body = self.owner.getComponent("PhysicSphere")
        print(f'applyForce, body: {body}')
        if body is not None:
            print(f'applyForce, body: {body}')
            body.applyForce(0, 0, -1000.0)
            print('applyForce OK')
        self.navAgent = self.owner.getComponent("NavAgent")
        if self.navAgent is not None:
            print(f'agent: {self.navAgent}')
            self.navAgent.targetPosition = (0, 0, -30.0)
            print(f'agent pos: {self.navAgent.targetPosition}')
        self.scene = SceneManager.getInstance().currentScene
        print(f'scene: {self.scene}')

    def onUpdate(self, dt):
        if not self.updated:
            print(f'onUpdate, dt = {dt}')
            print(f'owner, name = {self.owner.name}')
            self.updated = True
        if core.isFingerReleased(0):
            pos = core.getFingerPosition(0)
            print(f' pos: {pos}')
            if self.scene is not None:
                print(f' scene: {self.scene}')
                camera = self.scene.activeCamera
                print(f' camera: {camera}')
                if camera is not None:
                    hit = self.scene.raycast(pos, camera)
                    if hit is not None:
                        print(f' hit: {hit}')
                        self.navAgent.targetPosition = hit["hitPosition"]
                        print(f'agent pos: {self.navAgent.targetPosition}')

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
