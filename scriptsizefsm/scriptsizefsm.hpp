/**
 * @file
 * @brief Small object-oriented header-only final state machine library
 *
 * ScriptsizeFSM is heavily inspired by [TinyFSM](https://github.com/digint/tinyfsm), but has a
 * non-static design that allows to instantiate multiple identical but independent final state
 * machines. A user has to implement events that trigger reactions of the FSM, (static) States that
 * contain the reaction functions (as well as entry and exit function) and the FSM itself. In this
 * implementation, the FSM can contain member variables and even can have its own constructor.
 *
 * @copyright Copyright © 2022 Stephan Lachnit <stephanlachnit@debian.org>
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace scriptsizefsm {

    /// @{
    /**
     * \internal
     * @brief internal state instance helper definitions
     *
     * Note: this is taken straight from TinyFSM (version 0.3.3).
     */
    template<class T_State>
    struct _state_instance {
        using value_type = T_State;
        using type = _state_instance<T_State>;
        static T_State value;
    };
    template<typename S>
    typename _state_instance<S>::value_type _state_instance<S>::value;
    /// @}

    /**
     * @brief Event class
     *
     * Possible state transition in the FSM are handled by reacting to events.
     * An event may contain variables describing details of the event.
     */
    class Event {};

    /**
     * @brief State class
     * @tparam T_FSM class to the FSM implementation
     *
     * A generic state class contains an entry function, an exit function and a reaction function
     * for all events. Each real state is a child of this generic class, overriding the functions.
     *
     * Important: states are always static and thus should never contain member variables. Any
     * state information should be contained within the FSM class. Use the pointer to the FSM if
     * such functionality is necessary.
     */
    template<class T_FSM>
    class State {

      public:

        /**
         * @brief State default destructor
         */
        virtual ~State() = default;

        /**
         * @brief entry function of a state
         * @param fsm pointer to the FSM
         *
         * This function is called every time the FSM enters this state.
         */
        virtual void entry(T_FSM* const fsm) const {};

        /**
         * @brief exit function of a state
         * @param fsm pointer to the FSM
         *
         * This function is called every time the FSM exits this state.
         */
        virtual void exit(T_FSM* const fsm) const {};

        /**
         * @brief reaction function of a state
         * @param fsm pointer to the FSM reacting
         * @param event reference to the event causing the reaction
         * @note in the generic state add a reaction function for all possible events
         *
         * This function is called every time the FSM encounters the given event. State
         * transition of the FSM should be implemented here using `transit<NewState>()`.
         */
        virtual void react(T_FSM* const fsm, const Event& event) const {};

      protected:

        /**
         * @brief state transition helper function
         * @tparam T_State state to transition to
         *
         * This is a helper function, the state instance itself is static and stays unchanged.
         */
        template<class T_State>
        inline void transit(T_FSM* const fsm) const
        {
            fsm->template transit<T_State>();
        }
    };

    /**
     * @brief Finite State Machine class
     * @tparam T_FSM_Child class of the actual FSM implementation
     * @tparam T_State_Generic class of the generic state containing all reactions
     *
     * A FSM may
     */
    template<class T_FSM_Child, class T_State_Generic>
    class FSM {

        friend State<T_FSM_Child>;

      public:

        /**
         * @brief starts the FSM
         * @tparam T_State_Init initial state of the FSM
         * @tparam T_Arg argument types for the FSM constructor
         * @param args arguments for the FSM constructor
         */
        template<class T_State_Init, typename... T_Arg>
        static T_FSM_Child start(T_Arg... args)
        {
            return T_FSM_Child {&_state_instance<T_State_Init>::value, args...};
        }

        /**
         * @brief FSM default destructor
         */
        virtual ~FSM() = default;

        /**
         * @brief reacts to a given event
         * @tparam T_Event event class to react to
         * @param event event to react to
         * @note T_State_Generic needs to have a react function for the event
         */
        template<class T_Event>
        inline void react(const T_Event& event)
        {
            current_state_->react(this_, event);
        }

        /**
         * @brief resets the FSM
         *
         * This function exits the current state and enters the initial state.
         */
        void reset()
        {
            current_state_->exit(this_);
            current_state_ = init_state_;
            resetter();
            current_state_->entry(this_);
        };

        /**
         * @brief checks if the FSM is in a given state
         * @tparam state to check for
         * @return bool that is true if FSM is in given state
         */
        template<class T_State>
        inline bool is_in_state() const
        {
            return current_state_ == &_state_instance<T_State>::value;
        }

      protected:

        /**
         * @brief FSM state transition function
         * @tparam state to transition to
         */
        template<class T_State>
        void transit()
        {
            current_state_->exit(this_);
            current_state_ = &_state_instance<T_State>::value;
            current_state_->entry(this_);
        }

        /**
         * @brief FSM constructor
         * @param init_state initial state of the FSM
         */
        FSM(const T_State_Generic* const init_state)
          : this_(static_cast<T_FSM_Child*>(this)),
            init_state_(init_state),
            current_state_(init_state) {};

        /**
         * @brief additional function called on reset
         */
        virtual void resetter() {};

      private:

        /**
         * \internal
         * @brief pointer to FSM implementation
         */
        T_FSM_Child* this_;

        /**
         * \internal
         * @brief pointer to the initial state
         */
        const T_State_Generic* const init_state_;

        /**
         * \internal
         * @brief pointer to the current state
         */
        const T_State_Generic* current_state_;
    };

    /**
     * @brief starts a FSM
     * @tparam T_FSM FSM implementation to start
     * @tparam T_State_Init initial state of the FSM
     * @tparam T_Arg argument types for the FSM constructor
     * @param args arguments for the FSM constructor
     */
    template<class T_FSM, class T_State_Init, typename... T_Arg>
    T_FSM start(T_Arg... args)
    {
        return T_FSM::template start<T_State_Init, T_Arg...>(args...);
    };

}  // namespace scriptsizefsm
