/**
 * @file
 * \ingroup tests
 * @brief test for examples/extended_switch.cpp
 *
 * @copyright Copyright © 2022 Stephan Lachnit <stephanlachnit@debian.org>
 * SPDX-License-Identifier: MIT
 */

#include <cassert>

#include "scriptsizefsm/scriptsizefsm.hpp"

#ifdef NDEBUG
#error "Compiling with NDEBUG defeats the purpose of this test"
#endif

class OnEvent : public scriptsizefsm::Event {
  public:

    OnEvent(double _current)
      : current(_current) {};
    double current;
};

class OffEvent : public scriptsizefsm::Event {};

class FSM;

class GenericState : public scriptsizefsm::State<FSM> {
  public:

    virtual void entry(FSM* const fsm) const override {};
    virtual void react(FSM* const fsm, const OnEvent& event) const {};
    virtual void react(FSM* const fsm, const OffEvent& event) const {};
};

class OnState : public GenericState {
  public:

    void react(FSM* const fsm, const OnEvent& event) const override;
    void react(FSM* const fsm, const OffEvent& event) const override;
};

class OffState : public GenericState {
  public:

    void entry(FSM* const fsm) const override;
    void react(FSM* const fsm, const OnEvent& event) const override;
};

class FSM : public scriptsizefsm::FSM<FSM, GenericState> {
    friend scriptsizefsm::FSM<FSM, GenericState>;
    friend OnState;
    friend OffState;

  public:

    inline double getCurrent()
    {
        return current_;
    };

  protected:

    inline void setCurrent(double current)
    {
        current_ = current;
    };
    FSM(const GenericState* const init_state, double current)
      : scriptsizefsm::FSM<FSM, GenericState>(init_state),
        initial_current_(current),
        current_(current) {};
    void resetter() override
    {
        setCurrent(initial_current_);
    };

  private:

    const double initial_current_;
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
    fsm->setCurrent(0.);
};

void OffState::react(FSM* const fsm, const OnEvent& event) const
{
    fsm->setCurrent(event.current);
    transit<OnState>(fsm);
};

int main()
{
    constexpr double init_current {10.};
    constexpr double some_current {20.};

    // Init -> OnState + init_current
    auto fsm = scriptsizefsm::start<FSM, OnState>(init_current);
    assert(fsm.is_in_state<OnState>());
    assert(fsm.getCurrent() == init_current);

    // OnState + OffEvent -> OffState + zero
    fsm.react(OffEvent());
    assert(fsm.is_in_state<OffState>());
    assert(fsm.getCurrent() == 0.);

    // OffState + reset -> OnState + init_current
    fsm.reset();
    assert(fsm.is_in_state<OnState>());
    assert(fsm.getCurrent() == init_current);

    // OnState + OnEvent -> OnState + some_current
    fsm.react(OnEvent(some_current));
    assert(fsm.is_in_state<OnState>());
    assert(fsm.getCurrent() == some_current);

    return 0;
}
