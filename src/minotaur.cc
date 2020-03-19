#include <iostream>
#include "minotaur.h"

using namespace enviro;

/* Methods */
void MovingForward::during() { 
    /* Watch for Theseus */
    if (sensor_reflection_type(0)=="Theseus" || sensor_reflection_type(1)=="Theseus") {
        label("Theseus", 20, 5);
    } else {
        clear_label();
    }
    
    track_velocity(4,0);
    if ( sensor_value(0) < 40 ) {
        emit(Event("turn"));
    }    
}

void Rotating::during() { 
    /* Watch for Theseus */
    if (sensor_reflection_type(0)=="Theseus" || sensor_reflection_type(1)=="Theseus") {
        label("Theseus", 20, 5);
        emit(Event("chase"));
    } else {
        clear_label();
    }

    track_velocity(0,rate); 
    if ( sensor_value(0) > 140 ) {
        emit(Event("wander"));
    }   
}

void Chase::during() {
    /* Chase Theseus */
    if (agent_exists(0)) {
        Agent& t = find_agent(0);
        move_toward(t.position().x, t.position().y);
    } else {
        emit(Event("wander"));
    }
}