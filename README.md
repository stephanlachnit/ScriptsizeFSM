<!--
Copyright © 2022 Stephan Lachnit <stephanlachnit@debian.org>
SPDX-License-Identifier: MIT
-->

# ScriptsizeFSM

ScriptsizeFSM is an object-oriented header-only final state machine C++17 library heavily inspired
by [TinyFSM](https://github.com/digint/tinyfsm), but has a non-static design that allows to
instantiate multiple identical but independent final state machines.

Object oriented means, that each event is a class, each state is a class, the FSM is a class and
reactions to events are implemented as function in the corresponding state.

## Simple example

```c++
#include <iostream>
#include "scriptsizefsm/scriptsizefsm.hpp"

// event definitions
class OnEvent : public scriptsizefsm::Event {};
class OffEvent : public scriptsizefsm::Event {};

// forward declaration of the FSM class, required for generic state definition
class FSM;

// generic state definition
// the generic state class contains the reaction function to each event the FSM may encounter
class GenericState : public scriptsizefsm::State<FSM>
{
    public:
        // reaction to the OnEvent
        virtual void react(FSM* const fsm, const OnEvent& event) const {};

        // reaction to the OffEvent
        virtual void react(FSM* const fsm, const OffEvent& event) const {};
};

// state for which the switch is on
class OnState : public GenericState
{
    public:
        // only react to the OffEvent
        void react(FSM* const fsm, const OffEvent& event) const override;
};

// state for which the switch is off
class OffState : public GenericState
{
    public:
        // only react to the OnEvent
        void react(FSM* const fsm, const OnEvent& event) const override;
};

// final state machine class
class FSM : public scriptsizefsm::FSM<FSM, GenericState>
{
    friend scriptsizefsm::FSM<FSM, GenericState>;

    protected:
        FSM(const GenericState* const init_state)
          : scriptsizefsm::FSM<FSM, GenericState>(init_state) {};
};

void OnState::react(FSM* const fsm, const OffEvent& event) const
{
    // turn the switch off
    transit<OffState>(fsm);
};

void OffState::react(FSM* const fsm, const OnEvent& event) const
{
    // turn the switch on
    transit<OnState>(fsm);
};

int main()
{
    // start the FSM in the OffState
    auto fsm = scriptsizefsm::start<FSM, OffState>();

    // lambda to print state
    auto print_state = [&fsm]()
    {
        if (fsm.is_in_state<OnState>())
        {
            std::cout << "Switch is on" << std::endl;
        }
        else
        {
            std::cout << "Switch is off" << std::endl;
        }
    };

    // react to OnEvent, aka turn on
    fsm.react(OnEvent());
    print_state();

    // react to OffEvent, aka turn off
    fsm.react(OffEvent());
    print_state();
}
```

For more examples, take a look at the [examples](examples/) directory.

## Build examples

You can build the examples with [Meson](https://mesonbuild.com/):

```shell
meson setup builddir -Dbuild_examples=true
meson compile -C builddir
```

The examples are then located in the `builddir/` directory.

## License
Licensed under MIT.
