/**
 * @file
 * \ingroup examples
 * \example examples/multiple_instances.cpp
 * @brief this examples creates multiple instances of a simple on-off switch
 *
 * @copyright Copyright © 2022 Stephan Lachnit <stephanlachnit@debian.org>
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#include "scriptsizefsm/scriptsizefsm.hpp"

/// @{
/**
 * @brief event definitions
 */
class OnEvent : public scriptsizefsm::Event {};
class OffEvent : public scriptsizefsm::Event {};
/// @}

class FSM;  // forward declaration of the FSM class

/**
 * @brief generic state definition
 */
class GenericState : public scriptsizefsm::State<FSM> {
  public:

    /**
     * @brief reaction to the OnEvent
     */
    virtual void react(FSM* const fsm, const OnEvent& event) const {};

    /**
     * @brief reaction to the OffEvent
     */
    virtual void react(FSM* const fsm, const OffEvent& event) const {};
};

/**
 * @brief state for which the switch is on
 */
class OnState : public GenericState {
  public:

    /**
     * @brief reaction to the OffEvent
     */
    void react(FSM* const fsm, const OffEvent& event) const override;
};

/**
 * @brief state for which the switch is off
 */
class OffState : public GenericState {
  public:

    /**
     * @brief reaction to the OnEvent
     */
    void react(FSM* const fsm, const OnEvent& event) const override;
};

/**
 * @brief final state machine class
 */
class FSM : public scriptsizefsm::FSM<FSM, GenericState> {
    friend scriptsizefsm::FSM<FSM, GenericState>;

  protected:

    /**
     * @brief final state machine constructor
     */
    FSM(const GenericState* const init_state)
      : scriptsizefsm::FSM<FSM, GenericState>(init_state) {};
};

void OnState::react(FSM* const fsm, const OffEvent& event) const
{
    transit<OffState>(fsm);
};

void OffState::react(FSM* const fsm, const OnEvent& event) const
{
    transit<OnState>(fsm);
};

int main()
{
    // create two instances of the same FSM
    auto fsm1 = scriptsizefsm::start<FSM, OffState>();
    auto fsm2 = scriptsizefsm::start<FSM, OnState>();

    std::cout << "Send events to FSM:\n"
              << " OnEvent:    1\n"
              << " OffEvent:   2\n"
              << " no event:   3\n"
              << "Press r to reset and q to quit" << std::endl;

    // normal loop function from before, returns true on quit and false else
    auto fsm_loop_function = [](FSM& fsm, const char* fsm_name) {
        char c;
        std::cout << fsm_name << ": 1 2 3 r q? ";
        std::cin >> c;

        switch(c) {
        case '1':
            fsm.react(OnEvent());
            break;
        case '2':
            fsm.react(OffEvent());
            break;
        case '3':
            break;
        case 'r':
            fsm.reset();
            break;
        case 'q':
            return true;
        default:
            std::cerr << "Wrong usage!" << std::endl;
            break;
        }
        return false;
    };

    // normal print function from before
    auto fsm_print_state = [](const FSM& fsm, const char* fsm_name) {
        auto* state = fsm.is_in_state<OnState>() ? "on" : "off";
        std::cout << fsm_name << " is " << state << std::endl;
    };

    fsm_print_state(fsm1, "FSM1");
    fsm_print_state(fsm2, "FSM2");

    // call the first and the the second FSM
    while(true) {
        if(fsm_loop_function(fsm1, "FSM1")) {
            return EXIT_SUCCESS;
        };
        if(fsm_loop_function(fsm2, "FSM2")) {
            return EXIT_SUCCESS;
        };
        fsm_print_state(fsm1, "FSM1");
        fsm_print_state(fsm2, "FSM2");
    }
}
