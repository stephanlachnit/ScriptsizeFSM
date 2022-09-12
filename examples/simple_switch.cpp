/**
 * @file
 * \ingroup examples
 * \example examples/simple_switch.cpp
 * @brief his is a minimal example of how to use ScriptsizeFSM with a simple on-off switch
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
 *
 * The generic state class contains the reaction function to each event the FSM may encounter.
 */
class GenericState : public scriptsizefsm::State<FSM> {
  public:

    /**
     * @brief reaction to the OnEvent
     *
     * We leave this as an virtual function which does nothing by default. We will override it
     * in the definitions of the concrete states.
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
     * @brief reaction to the OnEvent
     *
     * To ensure this function is properly defined and is used when reacting to OnEvent in
     * OnState, we declare this function as specifically as an override.
     */
    void react(FSM* const fsm, const OnEvent& event) const override;

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

    /**
     * @brief reaction to the OffEvent
     */
    void react(FSM* const fsm, const OffEvent& event) const override;
};

/**
 * @brief final state machine class
 *
 * We make this a scriptsizefsm::FSM refering to itself and the generic state we implemented.
 */
class FSM : public scriptsizefsm::FSM<FSM, GenericState> {
    // We need to be friend with the parent so it can call the constructor in the start function.
    friend scriptsizefsm::FSM<FSM, GenericState>;

  protected:

    /**
     * @brief final state machine constructor
     * @note this is required to call the constructor of the parent
     */
    FSM(const GenericState* const init_state)
      : scriptsizefsm::FSM<FSM, GenericState>(init_state) {};
};

void OnState::react(FSM* const, const OnEvent& event) const
{
    // a reaction doesn't require a state transition...
    std::cout << "OnState: OnEvent: doing nothing" << std::endl;
};

void OnState::react(FSM* const fsm, const OffEvent& event) const
{
    // ...but if want a state transition we call it with `transit<NewState>(fsm)`
    std::cout << "OnState: OffEvent: switching off" << std::endl;
    transit<OffState>(fsm);
};

void OffState::react(FSM* const fsm, const OnEvent& event) const
{
    std::cout << "OffState: OnEvent: switching on" << std::endl;
    transit<OnState>(fsm);
};

void OffState::react(FSM* const, const OffEvent& event) const
{
    std::cout << "OffState: OffEvent: doing nothing" << std::endl;
};

int main()
{
    // start the fsm using `scriptsizefsm::start<MyFSMClass, InitStateClass>(constructor_args)`
    auto fsm = scriptsizefsm::start<FSM, OffState>();

    std::cout << "Send events to FSM:\n"
              << " OnEvent:    1\n"
              << " OffEvent:   2\n"
              << "Press r to reset and q to quit" << std::endl;

    while(true) {
        char c;
        std::cout << "1 2 r q? ";
        std::cin >> c;

        switch(c) {
        case '1':
            // to react, construct a OnEvent and call the FSM react function with it
            fsm.react(OnEvent());
            break;
        case '2':
            fsm.react(OffEvent());
            break;
        case 'r':
            fsm.reset();
            break;
        case 'q':
            return EXIT_SUCCESS;
        default:
            std::cerr << "Wrong usage!" << std::endl;
            break;
        }
    }
}
