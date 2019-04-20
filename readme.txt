WASD to move
Arrows/Mouse for camera
Z to jump
F to interact (doors/save points)
X or LMB to shoot
F9 to enable/disable IMGUI
C to enable/disable mouselock (for camera)
Mouse is used to angle camera during mouselock
R to reset scene (places the player at the start or their last saved position)
P to go back to the title screen

Pools are used for the Bullets.

NEW:

- Raytracing support added as another Bullet3D feature.
- Tweening can now be tested with a subclass called TweenedImGuiEnemy. It's inside Imgui/GameObjects/TweenTestSubject during runtime.
- There's Weighted Random Audio Cues (used by player) and a ShuffleAudioList (used by the scene)
- Game Objects are now properly synced with movement as well as forced location changes due to overwriting position or forces.
- removed the FBO invalidation which existed because I didn't actually want to use it at the time but didn't want to comment out 40 lines
- Now the FBO object doies exist in the world and draws the world onto a cube so you can see it works
- By default the FBO object is off because the effect is stupidly heavy and crushes FPS but it is located almost at 0,0,0 (initial spawn)





