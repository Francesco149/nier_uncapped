FPS Uncapper for NieR: Automata.

All credits go to Altimor:
https://www.reddit.com/user/Altimor who discovered this method to
manipulate min/max timestep.

All I did was spend some time finding decently reliable pointers to
detect menus and other parts.

This tool allows the game to run above 60fps during gameplay and
dynamically re-enables the FPS cap in the pause menu, title screen
and other menus that don't behave properly at high FPS.

Video demonstration (minor playable character spoiler):
https://streamable.com/mg55t

Tested on Windows 10 x64 and Windows 7 x64.

# How to use
* Install FAR: http://steamcommunity.com/app/524220/discussions/0/135512104777399045
* Download the dll from https://github.com/Francesco149/nier_uncapped/releases
* Put nier_uncapped.dll in your Nier Automata folder (right click
  the game on steam, properties, local files, browse local files).
* Add the following at the top of your dxgi.ini in your Nier
  automata folder:
  
```ini
[Import.nier_uncapped]
Architecture=x64
Filename=nier_uncapped.dll
Role=ThirdParty
When=PlugIn
```

Scroll down to ```[Render.FrameRate]``` and change TargetFPS to
0.0.

* Run the game, disable vsync in the options and hope for the best.

# If it doesn't work
Kill nier automata from the task manager if it's still there.

Delete nier_uncapped.dll from the game's folder.

Revert the ini, but it shouldn't be necessary.

# Why would I want this
You might have a good pc and want to get the full performance it
can get on this game.

Higher FPS also feels much better, especially on high refresh rate
monitors.

# Disclaimer
This code was put together in < 2 hours and doesn't have much
error checking. It's not the prettiest code either.

I don't plan on maintaining this project for a long time, but the
code is public and people can pick up from this research.

The game's engine seems to be very inconsistent and some things are
hardcoded to 60fps while others use the min/max deltatime that is
uncapped by this mod.

It will likely take some time and more people who can reverse to
fully iron out all subtle bugs caused by uncapped fps.

# How to compile it yourself
* Clone this repo
* Install visual C++ Buld Tools 2015 (not needed if you already
  have visual studio 2015 with c++ support).
  http://landinghub.visualstudio.com/visual-cpp-build-tools
* Open Visual C++ 2015 x64 Native Build Tools Command Prompt

```
cd C:\Path\To\Repo
build.bat
```

* Dll will be in the repo's folder