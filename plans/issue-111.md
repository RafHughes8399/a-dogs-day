# Issue 111 - inventory categories, non-decorations inclusion in the inventory, and creating an item  from the inventory


## Inventory Categories
a collection of side hud panels are to be attached to the inventory, each with the category (by logo or by name, i think by logo, just use one that is like synonymous, like recogniseable - for example 
the stove for the stations)

they are interactable, so buttons. the on click should change the active invenmtopry category index


>I think this is 
a panel within a panel ? composite panel for huds ? 
or just a list of panels, even that would do. then the over arching list has the origin and the elements have their offset and layer yeah and layer 
while a good idea. let save it for post demo. if 
we think about the stages , like this is interface prettiering, it snot
>

currently the categories are:
- waiters [pending that feature]
- customers [because they should have different money giving stats, tipping and exp for serving]
- decorations
- stations

so build for that. Its a simple enough extensions that just requires one extra check to determine which specific inventory to insert it into. treat these as sub-inventories. they work identical to the one that . there just needs to be one extra wrapped layer that determines which one to put it into

adjust the spacing of the inventory frame to add in the veritcal and horizontal buffers. like the frame
and the side panel

and actually do the art for the page navigators haha


and expand to allow for stations and stuff, its just because they dont have a definitoin in the enyclopedia, i think it will also need to be extended to hold the category too 

## Creating an item from the inventory
basic flow of this looks like:
in on inventory_item_button click behaviour, emit an create decoration event.
listened by the entity lifespan system which then executes the builder and the gives it to the cursor carrier component
listend also by the menu nav which temporarily hides it
listened also by the control input system which temporarily overides the control for esc to 

so we could need a control state and then the event just updates the control state

if placed down it places down simple , if esc is pressed the entity is destroyed. both roads lead back to the inventory
