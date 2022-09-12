/**
 * @file
 * \ingroup tests
 * @brief test for examples/multiple_instances.cpp
 *
 * @copyright Copyright © 2022 Stephan Lachnit <stephanlachnit@debian.org>
 * SPDX-License-Identifier: MIT
 */

#include <cassert>

#include "scriptsizefsm/scriptsizefsm.hpp"

#ifdef NDEBUG
#error "Compiling with NDEBUG defeats the purpose of this test"
#endif

class OnEvent : public scriptsizefsm::Event {};
class OffEvent : public scriptsizefsm::Event {};

class FSM;

class GenericState : public scriptsizefsm::State<FSM> {
  public:

    virtual void react(FSM* const fsm, const OnEvent& event) const {};
    virtual void react(FSM* const fsm, const OffEvent& event) const {};
};

class OnState : public GenericState {
  public:

    void react(FSM* const fsm, const OffEvent& event) const override;
};

class OffState : public GenericState {
  public:

    void react(FSM* const fsm, const OnEvent& event) const override;
};

class FSM : public scriptsizefsm::FSM<FSM, GenericState> {
    friend scriptsizefsm::FSM<FSM, GenericState>;

  protected:

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
    // Init -> Off/On
    auto fsm1 = scriptsizefsm::start<FSM, OffState>();
    auto fsm2 = scriptsizefsm::start<FSM, OnState>();
    assert(fsm1.is_in_state<OffState>());
    assert(fsm2.is_in_state<OnState>());

    // Off/On + On/Off -> On/Off
    fsm1.react(OnEvent());
    fsm2.react(OffEvent());
    assert(fsm1.is_in_state<OnState>());
    assert(fsm2.is_in_state<OffState>());

    // On/Off + reset/reset -> Off/On
    fsm1.reset();
    fsm2.reset();
    assert(fsm1.is_in_state<OffState>());
    assert(fsm2.is_in_state<OnState>());

    // Off/On + -/Off -> Off/Off
    fsm2.react(OffEvent());
    assert(fsm1.is_in_state<OffState>());
    assert(fsm2.is_in_state<OffState>());

    // Off/Off + On/- -> On/Off
    fsm1.react(OnEvent());
    assert(fsm1.is_in_state<OnState>());
    assert(fsm2.is_in_state<OffState>());

    return 0;
}
