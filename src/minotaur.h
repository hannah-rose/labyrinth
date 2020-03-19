#ifndef __Minotaur_AGENT__H
#define __Minotaur_AGENT__H 

#include "enviro.h"

using namespace enviro;

class MovingForward : public State, public AgentInterface {
    public:
    void entry(const Event& e) {}
    void during() { 
        track_velocity(4,0);
        if ( sensor_value(0) < 40 ) {
            emit(Event("turn"));
        }    
    }
    void exit(const Event& e) {}
};

class Rotating : public State, public AgentInterface {
    public:
    void entry(const Event& e) { 
        rate = rand() % 2 == 0 ? 2 : -2; 
        //decorate("<circle x='-5' y='5' r='5' style='fill: red'></circle>");
        //label(sensor_reflection_type(0), 20, 5);
    }
    void during() { 
        track_velocity(0,rate); 
        if ( sensor_value(0) > 140 ) {
            emit(Event("wander"));
        }   
    }
    void exit(const Event& e) {
        decorate(""); 
        clear_label();
    }
    double rate;      
};

class MinotaurController : public StateMachine, public AgentInterface {

    public:
    MinotaurController() : StateMachine() {

        set_initial(moving_forward);

        add_transition("turn", moving_forward, rotating);
        add_transition("wander", rotating, moving_forward);
    }

    /*void init() {
        watch("screen_click", [&](Event &e) {
            teleport(e.value()["x"], e.value()["y"],0);
            e.stop_propagation();
        });
        StateMachine::init();
    }*/

    MovingForward moving_forward;
    Rotating rotating;

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