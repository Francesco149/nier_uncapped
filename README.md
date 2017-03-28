FPS Uncapper for NieR: Automata.

All credits go to Altimor:
https://www.reddit.com/user/Altimor who discovered this method to
manipulate min/max timestep.

I simply spent like 2 hours finding decently reliable pointers to
detect the main menu and the pause menu.

This tool allows the game to run above 60fps during gameplay and
dynamically re-enables the FPS cap in the pause menu, title screen
and other menus that don't behave properly at high FPS.

Video demonstration (minor playable character spoiler):
https://streamable.com/mg55t

# Disclaimer

This is a super early release.
This code was put together in < 2 hours and still needs massive
clean-up and optimization. Will update soon. This has been barely
tested and only on windows 7 x64.

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