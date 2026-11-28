# Issue 112 : cooking food

high level for now, just identifying key features and AC
-> selecting a recipe and cooking that recipe via a hud and dog controlling.
i.e you select a recipe to cook, and then you send a dog to work the station

-> the selecting is to be done via the hud, where clicking the station shows a recipe carousel, one by one that the user can navigate through. the displayed recipe is the selected one

-> there is a progress bar to show the progress of the food being creating
-> when the food is finished being prepared it is placed on an adjacent connected food counter that can store it, if there is no counter the food is lost to the ether

bene


## The Recipe Hud Interface
this needs a rework on a multitude of levels i suppose. we are to take a different approach. it should be a carousel with one 
button showing at a time not four. 

currently the builder implementation for the hud panel is in the entity lifespan system, entity_lifespan_system.cpp [177, 199]. that is some retard ai slop.
```
void systems::entity_lifespan_system::create_cooking_station_hud(size_t owner){
    auto* recipes = component_managers::entity_recipes_manager_.get_component(owner);
    auto* position = component_managers::entity_positional_manager_.get_component(owner);
    if(recipes == nullptr or position == nullptr){ return; }

    auto origin = Vector2Add(position->get_position(), hud_config::cook_station_hud_offset);
    auto page_size = static_cast<size_t>(hud_config::recipe_rows * hud_config::recipe_columns);
    auto count = std::min(recipes->get_selected_recipes().size(), page_size);
    for(size_t recipe = 0; recipe < count; ++recipe){
        auto column = static_cast<float>(recipe % static_cast<size_t>(hud_config::recipe_columns));
        auto row = static_cast<float>(recipe / static_cast<size_t>(hud_config::recipe_columns));
        Vector2 button_position{
            origin.x + hud_config::reicpe_icon_width_buffer * 0.5f
                + column * (hud_config::recipe_icon_width + hud_config::reicpe_icon_width_buffer),
            origin.y + hud_config::recipe_icon_height_buffer * 0.5f
                + row * (hud_config::recipe_icon_height + hud_config::recipe_icon_height_buffer)};
        auto food = recipes->get_selected_recipes()[recipe].get_food_id();
        hud_systems::hud_lifespan_system::get_instance().create_element(owner,
            [food, button_position, recipe](size_t id){
                button_builders::build_recipe_button(food, id, button_position, recipe);
            });
    }
}
```
this should be in the hud element builder and called by the station builder. there are two parts to this though
1. the recipe component - it already mostly exists. 
2. the hud - needs to include the page navigators which 
maybe a refactor to a button is to assign it a click behaviour ? or just define a new button completely that uses a variant of the art. i think that is easier 
i just think the rendering logic needs to change. the question nereds to be answered where 
either we build and destroy buttons as we navigate recipes, or we build all the buttons and then change which one is active ? 

building and destroying is more expensive i think especially considering that it would occur quite frequently. os i tihbnk the better
approach is to load all the buttons and then only render the one that is active, plus the page numbers. i think we can mimic 
the inventory page setup for this as it is similar


more concretely. on creating the hud elemetn. the buttons are created and managed through a list of their ids. the active index is the one 
that is drawn, when the hud component is rendered

maybe a hud recipe component is necessary like the inventory storage component. and then that can tell you which one is active and should be rendered ? 
then it should also have the recipe_navigators. they can be their own button just reuse the art but scaled down to fit the element better
some things to think about 


we need to be more concrete about the structures. i think the recipe component can be overhauld to actually store the buttons. because the inventory storage component renders. it is the hud component that is active, i suppose we just need to change its renderable when the 
page change occurs. that is actually the simplest way. yeah just change the sprite. its not a const right ? or just change the button.


right because it is managed by the active component that is your conditional.  but that is for the entirety of the hud component not individual elements

well it should only have one button. then the page nav informs the recipe compoennt to change the button. thje recipe component figures out which button to change and then the recipe system ? which would swap out the button. maybe it can store all the buttons but like just 
update the active one to the right position. the rest can be drawn offscreen. but wouldnt be drawn beccause they exist outside of the viewframe ? 

yeah we will need a recipe system i think. or a cooking system in general that can manage progress and creation and counter checking and such 


ok so the breakdown is the recipe compoennt holds the information for the recipes available at the station

a new hud element is created and attached to the station. it knows all the buttons. sets their positions to -1, -1.. it knows 
the page navigation button too. upon creation it is given the button to start with and sets that position to the active_button position
the recipe nav buttons have a new click behaviour that update the active button, just a positional swap 

the recipe buttons click behaviour is needed. 

and a recipe system is needed to manage the produciton of food and theri creatoin

maybe a recipe_system. it can get all the recipe componetns

unless we introduce some active component for the indivudal hud elemnetns as well as the entity as a whole ? that could solve our problem
then incoporate that into the render check for the hud system ? so like if it has a hidden_component and is hidden then dont render ?

that would simplify things

amongst some art pieces including:
-> hud elements for recipes 
-> more recipes including the tiger pie, the bucket of pizzles, the foot fetish (chicken foot, rabbit paw, cow / horse hoof)
-> daft pug
-> ben but 
-> dogmatix and the cauldron
-> gromit on the plane, like on a runway
-> then that wildwood dog knitting
-> sandvich food art