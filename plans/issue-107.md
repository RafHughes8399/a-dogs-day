# PLAN FOR ISSUE 107 : [hud and menus] continued inventory work

The issue describes three different slices for this work to improve the hud interaction, rendering and expand the menu navigation

## Slice 1: Cursor Rendering
presently the cursor renders behind the hud, the fix is simple: just add a new render layer for the cursor which is placed above everything else

-> the render layer stuff already exists, its just that the hud rendering system renders after the rendering system and thus 
is drawn on top
maybe it is best if we scrap the hud and cursor layers in the render layer setup and then do the global cursor thing

yeah so we just draw the cursor layer last.

ok solved.

## Slice 2 : Menu Navigation System Expansion

build blank screens for each of the other menus. 

pause menu:  centre of the screen, should be 320 x 320

shop menu: same sizing as inventory. i think we can create a sub class button for shop item buttons. the underlying item button is important to maintain, it would just need a price component to render alongside  

quests menu: not sure about quests for now

map menu: is a map even necessary ? 

and pause the game when a menu is opened ? or keep it going ? are we doing fallout style ? at the very least the pause menu should pause

need to introduce builders for the screens and then a pause and unpause event
that is triggered on menu navigation to and from the pause menu respectively

i think dimensions should also be adjusted. lets give a 128 edge buffer on the width of the screeen and a 256 buffer on the height. done

ok so we need to build the shop menu which can be the same size. and the pause menu which is slightly different.\
lets also humor quests. can just be a side bar on the left for now. like full height. maybe 0.3 width

## Slice 3 : Button Hovering
need to introduce a hud collision check when the cursor moves. it should check active hud elements only and only buttons. so we need to confirm we have a way to check the active / inactive state of the hud component 
and perhaps we need to introduce an element type so we can determine if it is actually a button. 

the cursor would be able to collide with mutliple buttons. if that is the case ,we resolve with comparing the % overlap to determine a winner

hovering adjusts the button size temporarily and adds a slight tint. the tint depends on the button. 

so we need a hover component that defines the scale adjustment and the tint that would be applied


## Slice 4 : Testing Multiple Inventory Pages
some more testing needs to occur for the inventory page system. need to fill a page and check that another page is created when the current is full.

also need to adjust button creation on the page depending on its index. [first page should have no left navigation button, last page should have no right navigation button]. simple enough changes to the builder

the page navigation needs its on click behaviour defined. can emit a simple enough event that carries an index adjustment [ +- 1] which the inventory system listens to and adjusts its index. it should also include a bounds check even though the option is not given when the page is a head or tail, just for safety


## Slice 5 : Picking Up Decorations
this code already exists in legacy ? that is the logic for the decoration following the cursor. i think it might but its easy enough to replicate so it's not a major concern. 

i think the carrier component and system can be repurposed for the cursor. it already has the positional updating and tracking logic.

it would require the on click behaviour for the move deocration button to 
update the carrier component of the cursor. 

perhaps a pivot for the cursor id to be global / universal so it can be referenced by anyone, anywhere ? since it interacts with so much ? 

and the cursor left click behaviour should be updated to first check the carrier component. if it is carrying anythign, then we check if can place down. if can, place it down, if not, then do nothing.

This may also require that placeable tint and grid to reappear too. but that can be setup in the future

the necessary decoration movement events must also be emitted.

for now this can just encompass decorations. we can consider stations next

## Slice 6 : Art
- page navigation buttons
- lets do a pothos and a monsterra
- agapantha too 
- i think a dj dog where it spins around on the record would be pretty funny
- lets do a podcast with abby chatfield walter and steven the lurcher
- and the husky choir  and / or the dog excavation site