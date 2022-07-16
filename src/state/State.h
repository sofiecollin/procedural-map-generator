#ifndef STATE_H
#define STATE_H

#include <string>

class State
{
 public:
    State() {}
    virtual ~State() {}

    virtual void activate() {}
    virtual void deactivate() {}
  
    virtual void update(float delta_time) {}
    virtual void render() const = 0; // Removed const. Maybe look more at the shader data loading and see if we can make const again
    virtual void check_input() = 0;
    virtual std::string name() const = 0;

    std::string update_state()
    {
	std::string tmp {change_state};
	change_state = "";
	return tmp;
    }

protected:
    std::string change_state {};
};

#endif
