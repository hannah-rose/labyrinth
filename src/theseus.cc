#include <iostream>
#include "theseus.h"

using namespace enviro;

/* TheseusController Methods */

/* State Methods */

void Moving::entry(const Event& e) {

    /* Save initial right and left sensor values */
    senL_0 = sensor_value(senL);
    senR_0 = sensor_value(senR);
    countL = 0;
    countR = 0;
}

void Moving::during() {

    /* Don't rotate */
    prevent_rotation();

    /* Look for the Minotaur */
    if (sensor_reflection_type(0)=="minotaur" || sensor_reflection_type(1)=="minotaur"|| sensor_reflection_type(2)=="minotaur" || sensor_reflection_type(3)=="minotaur") {
        label("Minotaur", 20, 5);
        emit(Event("chase_m"));
    } else {
        clear_label();
    }

    /* Save periphery sensor values */
    senL_val = sensor_value(senL);
    senR_val = sensor_value(senR);

    /* First, check for a wall in front of us */
    if (sensor_value(sen)<10) {
        emit(Event("look around", sen));

    } else {
        /* Watch for intersections by monitoring changes on peripheral sensors */
        /* Left */
        if (senL_val>(senL_0+10)) { 
            countL++;
            if (countL>15) {
                std::cout << "Look left!\n";
                emit(Event("look around", sen));
            }
        } else if (senL_val<senL_0) {
            senL_0 = senL_val;
            countL=0;
        }
        /* Right */
        if (senR_val>(senR_0+10)) { 
            countR++;
            if (countR>15) {
                std::cout << "Look right!\n";
                emit(Event("look around", sen));
            }
        } else if (sensor_value(senR)<senR_0) {
            senR_0 = senR_val;
            countR=0;
        }

        /* Move in direction of travel */
        omni_track_velocity(rate_x,rate_y);
    }
}


void Assessing::during() {

    /* Hold still */
    omni_track_velocity(0,0);

    /* Close unavailable routes */
    if (sen_vals[0]<30) {sen_vals[0] = -99;}
    if (sen_vals[1]<30) {sen_vals[1] = -99;}
    if (sen_vals[2]<30) {sen_vals[2] = -99;}
    if (sen_vals[3]<30) {sen_vals[3] = -99;}

    /* Don't retrace your steps */
    sen_vals[past] = 0;

    /* Basic decision - which open path is the longest? */
    max_sen = std::max_element(sen_vals.begin(),sen_vals.end()) - sen_vals.begin();
    max_sen_int = int(max_sen);

    /* Check for a dead end */
    if (max_sen_int==past) {
        std::cout << "Dead end!\n";
        /* Switch to backtrack mode */
        explore = false;

    } else {
        /* Explore mode */
        explore = true;

        /* Remember this intersection */
        this_int = std::make_tuple(sen_vals[0], sen_vals[1], sen_vals[2], sen_vals[3]);
        m.push_back(this_int);
    }

    /* Switch directions based on the chosen sensor */
    switch (max_sen_int) {
        case 0:
            emit(Event("head east"));
            break;
        case 1:
            emit(Event("head south"));
            break;
        case 2:
            emit(Event("head west"));
            break;
        case 3:
            emit(Event("head north"));
            break;
        default: 
            break;
    }

}

void Assessing::entry(const Event& e) {
    past = e.value();

    /* Use the last direction traveled to set the past variable and avoid backtracking */
    switch (int(past)) {
        case 0:
            past = 2;
            break;
        case 1:
            past = 3;
            break;
        case 2:
            past = 0;
            break;
        case 3:
            past = 1;
            break;
        default: 
            break;
    }

    /* If we are in exploring mode, take the current sensor values. If we are backtracking, use the stored memory */
    if (explore) {
        sen_vals[0] = sensor_value(0);
        sen_vals[1] = sensor_value(1);
        sen_vals[2] = sensor_value(2);
        sen_vals[3] = sensor_value(3);
    } else {
        this_int = m.back();
        m.pop_back();
        sen_vals[0] = std::get<0>(this_int);
        sen_vals[1] = std::get<1>(this_int);
        sen_vals[2] = std::get<2>(this_int);
        sen_vals[3] = std::get<3>(this_int);
    }   
}

void Chase::during() {
    /* Chase the Minotaur */
    if (agent_exists(1)) {
        Agent& t = find_agent(1);
        omni_move_toward(t.position().x, t.position().y);
    } else {
        emit(Event("look around", 0));
    }
}



