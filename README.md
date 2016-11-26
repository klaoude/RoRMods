# RoR STATS HUD MOD

##FEATURES
1. Ability to see the lifebars (and health value) of all your teammates.

2. Ability to see Stats from one player at a time. Stats shown are:
  * Damage
  * Attack Speed
  * Critical Hit Chance
  * Regeneration
  * Strength (resistance to damage, call it how you want it)
  
3. Solo Mod, so you only see your stats when you're playing in SinglepLayer

4. You can toggle off the mod so nothing on your screen is changed. (Teammates will still receive your stats)


##HOW TO USE

###REQUIREMENTS
1. **Play in windowed mod**, our mod isn't working in fullscreen (we're working on it but it might never come, if someone knows how to properly hook Directx9, give us a call)

2. Mod looks better with 16:9 aspect ratio (HUD will show at the right places), but you can use any other aspect ratio you want, just be prepared that the HUD might be slightly off in terms of position.

3. **if you move the game's window, the mod doesn't move with it**, be careful with that. (we're working on that too)

4. Open the port succeeding RoR port you're playing on. (if you're using 1337 for RoR, open 1338 for our Mod)

###config.ini
The config file should look like this:
```
[options]
nickname = YourNickNameHere
ip = theIpAddressUsedInRoR
```

###INSTRUCTIONS
When you launch the mod, texts appears at the top of the game's window. Follow the instructions, press **F1 to host** the server of the mod, **F2 to join** a server, **F3 do dismiss** this text and play solo.  

To **toggle** On/Off the mod, press **F8**.  

To cycle through the stats of all the player, use **Page Up** and **Page Down** keys. The highlighted player (yellow nickname, items and lvl) is whom the stats are from. If no one is highlighted, the stats are yours. 
