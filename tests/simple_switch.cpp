/**
 * @file
 * \ingroup tests
 * @brief test for examples/simple_switch.cpp
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
    // Init with OffState -> OffState
    auto fsm = scriptsizefsm::start<FSM, OffState>();
    assert(fsm.is_in_state<OffState>());

    // OffState + OffEvent -> OffState
    fsm.react(OffEvent());
    assert(fsm.is_in_state<OffState>());

    // OffState + OnEvent -> OnState
    fsm.react(OnEvent());
    assert(fsm.is_in_state<OnState>());

    // OnState + OnEvent -> OnState
    fsm.react(OnEvent());
    assert(fsm.is_in_state<OnState>());

    // OnState + OffEvent -> OffState
    fsm.react(OffEvent());
    assert(fsm.is_in_state<OffState>());

    // OffState + reset -> OffState
    fsm.reset();
    assert(fsm.is_in_state<OffState>());

    // OnState + reset -> OffState
    fsm.react(OnEvent());
    fsm.reset();
    assert(fsm.is_in_state<OffState>());

    return 0;
}
