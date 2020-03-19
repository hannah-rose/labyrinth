#ifndef __Minotaur_AGENT__H
#define __Minotaur_AGENT__H 

#include "enviro.h"

using namespace enviro;

class MovingForward : public State, public AgentInterface {
    public:
    void entry(const Event& e) {}
    void during();
    void exit(const Event& e) {}
};

class Chase : public State, public AgentInterface {
    public:
    void entry(const Event& e) {}
    void during();
    void exit(const Event& e) {}
};

class Rotating : public State, public AgentInterface {
    public:
    void entry(const Event& e) { 
        rate = rand() % 2 == 0 ? 2 : -2; 
    }
    void during();
    void exit(const Event& e) {}
    double rate;      
};

class MinotaurController : public StateMachine, public AgentInterface {

    public:
    MinotaurController() : StateMachine() {

        set_initial(moving_forward);

        add_transition("turn", moving_forward, rotating);
        add_transition("wander", rotating, moving_forward);
        add_transition("wander", chase, moving_forward);
        add_transition("chase", moving_forward, chase);
        add_transition("chase", rotating, chase);
    }

    /* Collision handler */
    void init() {
        notice_collisions_with("Theseus", [&](Event &e) {
            /* Destruct the Minotaur */
             remove_agent(1);
        });
        StateMachine::init();
    }

    MovingForward moving_forward;
    Rotating rotating;
    Chase chase;

};

class Minotaur : public Agent {
    public:
    Minotaur(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    MinotaurController c;
};

DECLARE_INTERFACE(Minotaur)

#endif