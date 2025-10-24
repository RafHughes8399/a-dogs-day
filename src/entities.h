/**
 * header file that defines entitiy class hierarchy
 */
#ifndef ENTITIES_H
#define ENTITIES_h

#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "raglib.h"
#include "sprite.h"
namespace entities{
    enum status_codes{
        nothing = 0,
        moved = 1,
        dead = 2
    };
    // ------------------------- entities ------------------------- // 
    class entity {
        public:
        // ------------------ interaction and update strategies ------------------ // 
            class interaction_strategy{
                public:
                    virtual ~interaction_strategy() = default;
                    interaction_strategy() = default;
                    interaction_strategy(const interaction_strategy& other) = default;
                    interaction_strategy(interaction_strategy&& other) = default;
        
                    interaction_strategy& operator=(const interaction_strategy& other) = default;
                    interaction_strategy& operator=(interaction_strategy&& other) = default;
        
                    virtual void interact(entities::entity& interactor, entities::entity& interactee) = 0;
        
            };
             class default_interaction : public interaction_strategy{
                public:
                ~default_interaction() = default;
                default_interaction()
                :interaction_strategy(){};
                
                default_interaction(const default_interaction& other) = default;
                default_interaction(default_interaction&& other) = default;
                
                default_interaction& operator=(const default_interaction& other) = default;
                default_interaction& operator=(default_interaction&& other) = default;
                
                void interact(entities::entity& interactor, entities::entity& interactee) override;
                
            };
            class cursor_interaction : public interaction_strategy{
                public:
                    ~cursor_interaction() = default;
                    cursor_interaction()
                    :interaction_strategy(){};
        
                    cursor_interaction(const cursor_interaction& other) = default;
                    cursor_interaction(cursor_interaction&& other) = default;
        
                    cursor_interaction& operator=(const cursor_interaction& other) = default;
                    cursor_interaction& operator=(cursor_interaction&& other) = default;
        
                    void interact(entities::entity& interactor, entities::entity& interactee) override;
            };
            class update_strategy{
                public:
                    virtual ~update_strategy() = default;
                    update_strategy() = default;
                    update_strategy(const update_strategy& other) = default;
                    update_strategy(update_strategy&& other) = default;
        
                    update_strategy& operator=(const update_strategy& other) = default;
                    update_strategy& operator=(update_strategy&& other) = default;
        
                    virtual int update(entities::entity& entity, float delta) = 0;
        
            };
            /** 
             * 
             class default_update : public update_strategy{
                public:
                ~default_update() = default;
                default_update()
                :update_strategy(){};
                
                default_update(const default_update& other) = default;
                default_update(default_update&& other) = default;
                
                default_update& operator=(const default_update& other) = default;
                default_update& operator=(default_update&& other) = default;
                
                int update(entities::entity& entity, float delta) override;
                
            };
            */
            class cursor_update : public update_strategy{
                public:
                    ~cursor_update() = default;
                    cursor_update()
                    :update_strategy(){};
        
                    cursor_update(const cursor_update& other) = default;
                    cursor_update(cursor_update&& other) = default;
        
                    cursor_update& operator=(const cursor_update& other) = default;
                    cursor_update& operator=(cursor_update&& other) = default;
        
                    int update(entities::entity& entity, float delta) override;
            };
            class paw_update : public update_strategy{
                public:
                    ~paw_update() = default;
                    paw_update()
                    :update_strategy(){};
        
                    paw_update(const paw_update& other) = default;
                    paw_update(paw_update&& other) = default;
        
                    paw_update& operator=(const paw_update& other) = default;
                    paw_update& operator=(paw_update&& other) = default;
        
                    int update(entity& entity, float delta) override;
            };
            virtual ~entity() = default;
            entity(sprite::sprite sprite, raglib::bounding_box_2 bounds, Vector2 position, int id,
            std::unique_ptr<interaction_strategy> interact, std::unique_ptr<update_strategy> update)
            : bounds_(bounds), sprite_(sprite), position_(position), id_(id), interact_(std::move(interact)),
            update_(std::move(update)){

            };
            entity(const entity& other) = default;
            entity(entity&& other) = default;

            entity& operator=(const entity& other) = default;
            entity& operator=(entity&& other) = default;
            bool operator==(entity& other){
                return id_ == other.id_;
            }

            raglib::bounding_box_2& get_bounds();
            sprite::sprite get_sprite();
            Vector2 get_position();
            int get_id();

            void set_position(Vector2 position);

            int update(float delta){
                return update_->update(*this, delta);
            }
            void render();
            void interact(entity& other){
                interact_->interact(*this, other);
            }

        protected:
            const int id_;
            
            raglib::bounding_box_2 bounds_;
            sprite::sprite sprite_;
            Vector2 position_;

            std::unique_ptr<interaction_strategy> interact_;
            std::unique_ptr<update_strategy> update_;
    };
    // the cursor for the player 
    /**
     * shaped as a paw, changes based on interactable behaviour, like a regular cursor, 
     * if you consider it, a cursor is an entity, because it will interact with other entities
     * 
     * they would differ by their interactions ? 
     *  instead of making a class you could do entity, entity builder and then interact strategy and assign it that one ? 
     * i like this idea 
     * 
     */
    class cursor : public entity{
        public:
            ~cursor() = default;
            cursor(sprite::sprite sprite, raglib::bounding_box_2 bounds, Vector2 position, int id, std::unique_ptr<interaction_strategy> interact, std::unique_ptr<update_strategy> update)
            : entity(sprite, bounds, position, id, std::move(interact), std::move(update)){};
            cursor(const cursor& other) = default;
            cursor(cursor&& other) = default;

            cursor& operator=(const cursor& other) = default;
            cursor& operator=(cursor&& other) = default;
        private:
            // the handler_

    };

    // ------------------ entity builder ------------------ //
    class entity_builder{
        public:
            std::unique_ptr<entity> build_cursor(Vector2 position, int id);
            std::unique_ptr<entity> build_paw_mark(Vector2 position, int id);

            ~entity_builder() = default;
            entity_builder() {};
            entity_builder(const entity_builder& other) = default;
            entity_builder(entity_builder&& other) = default;

            entity_builder& operator=(const entity_builder& other) = default;
            entity_builder& operator=(entity_builder&& other) = default;

    };
    extern entity_builder e_builder;
}
#endif