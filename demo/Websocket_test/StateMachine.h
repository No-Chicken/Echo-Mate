// StateMachine.h
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>

class StateMachine {
public:
    StateMachine() = default;
    
    void AddState(const std::string& state_name, std::function<void()> on_enter) {
        states_[state_name] = on_enter;
    }

    void SetInitialState(const std::string& state_name) {
        current_state_ = state_name;
        if (states_.find(state_name) != states_.end()) {
            states_[state_name]();
        } else {
            std::cerr << "Error: Initial state not found.\n";
        }
    }

    void TransitionTo(const std::string& state_name) {
        if (states_.find(state_name) != states_.end()) {
            std::cout << "Transitioning from " << current_state_ << " to " << state_name << "\n";
            current_state_ = state_name;
            states_[state_name](); // Call on_enter function of the new state
        } else {
            std::cerr << "Error: State " << state_name << " not found.\n";
        }
    }

    const std::string& GetCurrentState() const {
        return current_state_;
    }

private:
    std::string current_state_;
    std::unordered_map<std::string, std::function<void()>> states_;
};

#endif // STATE_MACHINE_H
