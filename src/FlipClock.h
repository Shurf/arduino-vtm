#include <Arduino.h>

class FlipClock
{
public:
    FlipClock();

    void setPosition(int hours, int tens, int minutes);

private:
    void disable_stepper(const byte stepper_num);

    void half_step(const byte stepper_num);

    // Take the specified amount of steps for a stepper connected to the specified pins, with a
    // specified delay (in microseconds) between each step.
    void step_num(const byte stepper_num, unsigned int steps, const unsigned int wait);

    // Step until the endstop is pressed and released, and update the stepper position to 0.
    // If homing doesn't end after 2 rotations, the endstop is assumed to have failed and the program aborts.
    void step_to_home(const byte stepper_num, const unsigned int wait);

    // Step to the specified position. If the current position is greater than the target position, this
    // function will re-home and then step to the target position.
    void step_to_position(const byte stepper_num, unsigned int target_pos, const unsigned int wait);

    // Steps to the specified digit on the display
    // To save power, stepper is powered off after running, so exact positional accuracy is not maintained.
    // This is allowable since the displays self-calibrate via the endstops every rotation.
    void step_to_digit(const byte stepper_num, const byte digit, const unsigned int wait);

    // Stops the program. Flashes LED slowly if LED pin is defined.
    void e_stop();

    // Stepper pin definitions
    // 1st row - Hours
    // 2nd row - Minutes (Tens)
    // 3rd row - Minutes (Ones)
    // If a display is turning backwards, reverse the order of the pins for that display.
    const byte stepperPins[3][4] = {{32, 33, 25, 26},
                                   {14, 12, 13, 23},
                                   {5, 18, 19, 21}};

    // Endstop pin definitions
    // The endstops for the tens and ones displays can be wired together and share a pin if needed.
    // This is necessary on the Pro Micro if you want to use the TX/RX pins for something else.
    // Same order as before: Hours, Minutes (Tens), Minutes (Ones)
    const byte endstopPins[3] = {27, 22, 2};

    // Set these to the number that is displayed after the endstop is triggered.
    // Note: If the starting digit for the hours display is 12, enter 0 below.
    const byte startingDigits[3] = {7, 5, 9};

    // The following parameter configures how much the stepper turns after homing.
    // Starting offset should be increased until the top flap touches the front stop.
    const unsigned int startingOffset[3] = {200, 80, 200};

    // Half steps needed for the motors to perform a single rotation.
    // Usually 4096 for 28byj-48 steppers
    const int stepsPerRev = 4096;

    // The default delay after each motor step in microseconds.
    // Adjusts the speed of the displays. Higher value = slower
    const int stepperDelay = 1400;

    // Adjust this to the time in seconds it takes to compile and upload to the Arduino
    // If the clock runs slow by a few seconds, increase this value, and vice versa. Value cannot be negative.
    const int uploadOffset = 5;

    // Delay amount in ms added to the end of each loop iteration.
    // Some people have reported that reading the RTC time too often causes timing issues, so this value reduces the polling frequency.
    const int pollingDelay = 50;

    // Number of repeated endstop reads during homing. Must be positive.
    // Leave this value alone unless homing is unreliable.
    const int endstopDebounceReads = 3;

    // Position in steps of each stepper (relative to homing point)
    unsigned int stepperPos[3] = {0, 0, 0};

    // Current drive step for each stepper - 0 to 7 for half-stepping
    byte driveStep[3] = {0, 0, 0};
};