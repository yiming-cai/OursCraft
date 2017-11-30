### Domino in "OursCraft"
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; By Yiming CAI, Yue PAN, Ke CHEN

---

#### Description
The project is divided by two parts. The first part is about making an editor like "minecraft" to support ( from easy to hard, we can optionally add each of them ):
* freely objects placing with "square maze rule" ( we should only place our objects by row/column unit ) (**easy**)
* freely lights placing for three types of lights and parameters settings (**easy**)
* toon shading (**easy**)
* skybox(background) setting ( pre-setting some skybox materials for users to change ) and texture reflection (**easy**)
* particle effect supporting (**easy**)
* placing sound effect  (**easy**)
* shadow of the objects from the lights (**medium**)
* water reflection and refraction (**medium**)
* "NPC" and surrounding animation (interaction) (**medium**)
* motion blur (**hard**)
* Screen space ambient occlusion (**hard**)
* support "components objects" like city buildings and save it for next use ( create a building and save its model ) ("can be EC?")

The first part we make is to show our "diy" engine's supporting in many kinds of function in computer graphic. And we can use this engine to make a beautiful environment by ourselves.
After creating an environment, we add the second part.
We create a special object class called "**domino**", we can define it as "a box with any given size( l * w * h)" and any given six faces's texture or colors.
They should be under the rules below:
* There shouldn't be any object placed on the top of **domino**
* domino can have arbitrary size that does not obey the rule of "square maze rule" ( e.g. we can make the domino any height freely if we wish to make dominos fall from low place to high place : ) )
* Once we "touch" a domino from one direction, it should obey the physical rule like "go down continuously". So there will be collision detection with tight boundng boxes (**medium**) and basic physics supporting (**maybe EC?**) and audio effect(**easy**)

And then finally we can create a world with an amazing domino project, then making tracking cameras to record **its fallen** from many different views.
 
#### Division
talk in Thu

#### Schedule

| Due Date  | Part | Responsibility Man|
| :--------: | :--------:| :------: |
| 2017.12.1 Fri | Finish the blog website(like github repo url or website) ||
| 2017.12.3 Sun | Finish basic engine (object placing,skybox, lighting, etc.) for first snapshot ||
| 2017.12.3 Sun | Finish basic domino part (including configuring domino and collision) ||
| 2017.12.4 Mon | Finish the first blog and Finish the combanation||
| 2017.12.8 Fri | DIY basic environment(including dominoes placement)||
| 2017.12.8 Fri | Copy backup for another man to start some advanced parts ||
| | by this time, we should finish a "at least can be shown" project =_=||
| 2017.12.10 Sun| Finish the advanced part like NPC&OBJ animation, water reflection, motion blur... anyway, the time to "pick score" from the final project Inc |
| 2017.12.11 Mon| Finish the blog||
| 2017.12.12 Tue| Finish camera tracking settlement and then record our project||
| 2017.12.13 Wed| Prepare for presentation ||
| 2017.12.14 Thu| Good Luck||  
  


