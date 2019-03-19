Arrows/WASD to move
Z to jump
C to interact (doors/elevators)
X to shoot

ELEVATORS ARE THE GREY BRICK THINGS

Pools are used for the Bullets
The Objective is a sensor
We use revolute and prismatic joints
Everything aside from bullets is loaded from scene files
The teapot is from 3dsmax and uses the normal shader (can also be viewed in better detail on scene '4')

The biggest number I tested for score was 429496729 but I think the limitation is in the int, not the scoreboard
HUDCam is in HUD_Scene and uses Ortho

All debug draw is done and functional. To access it go under the debug checkbox. It will show the options.
Box2D even has pair commented out so I have the checkbox off.
Our center of mass debug is also off as I have no idea what that actually needs to look like