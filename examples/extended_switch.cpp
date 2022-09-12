/**
 * @file
 * \ingroup examples
 * \example examples/extended_switch.cpp
 * @brief this add some features of ScriptsizeFSM to the simple on-off switch example
 *
 * @copyright Copyright © 2022 Stephan Lachnit <stephanlachnit@debian.org>
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#include "scriptsizefsm/scriptsizefsm.hpp"

/**
 * @brief OnEvent definition
 *
 * The OnEvent now has a current with which the switch is turned on.
 */
class OnEvent : public scriptsizefsm::Event {
  public:

    /**
     * @brief OnEvent constructor
     * @param _current current of the OnEvent
     */
    OnEvent(double _current)
      : current(_current) {};

    /**
     * @brief current of the OnEvent
     */
    double current;
};

/**
 * @brief OffEvent definition
 */
class OffEvent : public scriptsizefsm::Event {};

class FSM;  // forward declaration of the FSM class

/**
 * @brief generic state definition
 */
class GenericState : public scriptsizefsm::State<FSM> {
  public:

    /**
     * @brief entry function
     */
    virtual void entry(FSM* const fsm) const override {};

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
     * @brief reaction to the OnEvent
     */
    void react(FSM* const fsm, const OnEvent& event) const override;

    /**
     * @brief reaction to the OffEvent
     */
    void react(FSM* const fsm, const OffEvent& event) const override;
};

/**
 * @brief state for which the switch is off
 *
 * Now we don't define the reaction to OffEvent, the react function of GenericState is called
 * instead. There, we can for example catch unhandled events.
 */
class OffState : public GenericState {
  public:

    /**
     * @brief entry function of the OffState
     */
    void entry(FSM* const fsm) const override;

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

    // declare friendship with OnState and OffState so that they can set the current
    friend OnState;
    friend OffState;

  public:

    /**
     * @brief get the current through the switch
     * @return returns the current through the switch
     */
    inline double getCurrent()
    {
        return current_;
    };

  protected:

    /**
     * @brief set the current through the switch
     */
    inline void setCurrent(double current)
    {
        current_ = current;
    };

    /**
     * @brief final state machine constructor
     * @param init_state initial state
     * @param current current at the initial state
     */
    FSM(const GenericState* const init_state, double current)
      : scriptsizefsm::FSM<FSM, GenericState>(init_state),
        initial_current_(current),
        current_(current) {};

    /**
     * @brief function called on FSM reset, set the current to the inital current
     */
    void resetter() override
    {
        setCurrent(initial_current_);
    };

  private:

    /**
     * @brief initial current of the switch
     */
    const double initial_current_;

    /**
     * @brief current of the switch
     */
    double current_;
};

void OnState::react(FSM* const fsm, const OnEvent& event) const
{
    fsm->setCurrent(event.current);
};

void OnState::react(FSM* const fsm, const OffEvent& event) const
{
    transit<OffState>(fsm);
};

void OffState::entry(FSM* const fsm) const
{
    // when we enter the OffState, current should be zero
    fsm->setCurrent(0.);
};

void OffState::react(FSM* const fsm, const OnEvent& event) const
{
    fsm->setCurrent(event.current);
    transit<OnState>(fsm);
};

int main()
{
    // set initial sate to OnState with some current
    auto fsm = scriptsizefsm::start<FSM, OnState>(10.);

    auto print_state = [&fsm]() {
        // check state via `is_in_state<State>()`
        auto* state = fsm.is_in_state<OnState>() ? "on" : "off";
        std::cout << "FSM is " << state << ", current: " << fsm.getCurrent() << std::endl;
    };

    print_state();

    std::cout << "Send events to FSM:\n"
              << " OnEvent:    1\n"
              << " OffEvent:   2\n"
              << "Press r to reset and q to quit" << std::endl;

    while(true) {
        char c;
        std::cout << "1 2 r q? ";
        std::cin >> c;

        double current;

        switch(c) {
        case '1':
            // ask for current
            std::cout << "Current? ";
            std::cin >> current;
            // create OnEvent with that current
            fsm.react(OnEvent(current));
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

        print_state();
    }
}
