#ifndef __THESEUS_AGENT__H
#define __THESEUS_AGENT__H 

#include <string>
#include <math.h>
#include <tuple>
#include <vector>
#include "enviro.h"

namespace {

    using namespace enviro;

    class Moving : public State, public AgentInterface {
        public:
        /* Methods */
        void entry(const Event& e);
        void during();
        void exit(const Event& e) {}
        void set_rate(double rx, double ry) { rate_x = rx; rate_y = ry; }
        void set_sen(double s, double s1, double s2) { sen = s; senL = s1; senR = s2; }

        /* Data */
        double rate_x, rate_y, sen, senR, senR_val, senL, senL_val, senR_0, senL_0;
        int countL, countR;
    };

    class Assessing : public State, public AgentInterface {
        public:
        void entry(const Event& e);
        void during();
        void exit(const Event& e) {}
        //enum SensorDirection { East, South, West, North };
        //SensorDirection dir;
        enum Path { From, Open, Closed };
        typedef std::tuple<double,double,double,double> intersection;
        intersection this_int;
        typedef std::vector<intersection> memory;
        memory m;
        double max_sen, past;
        int max_sen_int;
        std::vector<double> sen_vals = {0,0,0,0};
        bool explore = true;
    };

    class Chase : public State, public AgentInterface {
        public:
        void entry(const Event& e) {}
        void during();
        void exit(const Event& e) {}
    };

    class TheseusController : public StateMachine, public AgentInterface {

        public:
        TheseusController() : StateMachine() {

            /* Start out by looking around */
            set_initial(assess);

            /* Transitions to move */
            add_transition("head south", assess, move_south);
            add_transition("head north", assess, move_north);
            add_transition("head west", assess, move_west);
            add_transition("head east", assess, move_east);

            /* Transition to look around */
            add_transition("look around", move_south, assess);
            add_transition("look around", move_north, assess);
            add_transition("look around", move_west, assess);
            add_transition("look around", move_east, assess);
            add_transition("look around", chase, assess);

            /* Transition to chase */
            add_transition("chase_m", move_south, chase);
            add_transition("chase_m", move_north, chase);
            add_transition("chase_m", move_east, chase);
            add_transition("chase_m", move_west, chase);

            move_east.set_sen(0,3,1);
            move_east.set_rate(5,0);

            move_south.set_sen(1,0,2);
            move_south.set_rate(0,5);

            move_west.set_sen(2,1,3);
            move_west.set_rate(-5,0);

            move_north.set_sen(3,2,0);
            move_north.set_rate(0,-5);

        }

        /* Allow the user to reset */
        void init() {
            watch("screen_click", [&](Event &e) {
                teleport(e.value()["x"], e.value()["y"],0);
                e.stop_propagation();
                emit(Event("look around", 0));
            });
            notice_collisions_with("minotaur", [&](Event &e) {
                /* Destruct the Minotaur */
                emit(Event("look around", 0));
            });
            StateMachine::init();
        }

        Moving move_south;
        Moving move_north;
        Moving move_west;
        Moving move_east;

        Assessing assess;

        Chase chase;

    };

    class Theseus : public Agent {

        public:
        Theseus(json spec, World& world) : Agent(spec, world) {
            add_process(c);
        }

        TheseusController c;

    };

    DECLARE_INTERFACE(Theseus);

}

#endif