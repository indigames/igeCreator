"""
    Hello.py
    Script to print hello
"""

import igeCore as core
import igeScene
from igeScene import Scene, SceneManager, SceneObject
from igeScene import Component, TransformComponent, CameraComponent, EnvironmentComponent, FigureComponent, SpriteComponent

counter = 0

print(f"version: %s", igeScene.getVersion())

sceneManager = SceneManager.getInstance()
print(sceneManager)

if sceneManager is not None:
    print(sceneManager.currentScene)
    if sceneManager.currentScene is not None:
        print(sceneManager.currentScene.name)

def Awake():
    global counter
    name = SceneManager.getInstance().currentScene.name
    print(f'Python function Awake(), scene = %s', name)

def Start():
    print('Python function Enable() called')

def Enable():
    print('Python function Start() called')

def Disable():
    print('Python function Disable() called')

def Update(dt):
    global name
    global counter
    counter += 1
    print(f'Update(%f), counter = %d', dt, counter)

def FixedUpdate(dt):
    print(f'Python function FixedUpdate(%f) called', dt)

def LateUpdate():
    print(f'Python function LateUpdate(%f) called', dt)

def Render():
    print('Python function Render() called')

def Destroy():
    print('Python function Destroy() called')
