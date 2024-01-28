#include "FlipClock.h"

FlipClock::FlipClock()
{
    // Setting stepper pins to output
    // for (byte i = 0; i < 3; i++) {
    for (byte i = 0; i < 3; i++)
    {
        for (byte j = 0; j < 4; j++)
        {
            pinMode(stepperPins[i][j], OUTPUT);
        }
    }

    // Set endstop pins as input with pullups enabled
    pinMode(endstopPins[0], INPUT_PULLUP);
    pinMode(endstopPins[1], INPUT_PULLUP);
    pinMode(endstopPins[2], INPUT_PULLUP);

    // If the minute displays share an endstop pin (necessary on the Pro Micro), we need to make sure both of their endstops are unpressed before homing.
    // We can do this by alternatingly stepping each display in small increments until the endstop pin reads high.
    /*unsigned int total_steps = 0;
    if(endstop_pins[1] == endstop_pins[2]) {
        while(digitalRead(endstop_pins[1]) == LOW) {
        step_num(1, 200, STEPPER_DELAY);
        disable_stepper(1);

        // Still pressed, try the other display
        if(digitalRead(endstop_pins[1]) == LOW) {
            step_num(2, 200, STEPPER_DELAY);
            disable_stepper(2);
        }

        // Similar to homing, if a max number of steps is reached, the endstop is assumed to have failed and the program aborts.
        total_steps += 200;
        if(total_steps > STEPS_PER_REV) {
            e_stop();
        }
        }
    }*/

    Serial.println("Homing");

    // Step to home digit
    step_to_home(2, stepperDelay);        
    step_to_home(1, stepperDelay);        
    step_to_home(0, stepperDelay);
    
    Serial.println("Setup ended");
}

void FlipClock::setPosition(int hours, int tens, int minutes)
{
    step_to_digit(2, minutes, stepperDelay);
    step_to_digit(1, tens, stepperDelay);
    step_to_digit(0, hours, stepperDelay);
}

void FlipClock::disable_stepper(const byte stepper_num)
{
    digitalWrite(stepperPins[stepper_num][0], LOW);
    digitalWrite(stepperPins[stepper_num][1], LOW);
    digitalWrite(stepperPins[stepper_num][2], LOW);
    digitalWrite(stepperPins[stepper_num][3], LOW);
}

void FlipClock::half_step(const byte stepper_num)
{
    const byte pos = driveStep[stepper_num];
    digitalWrite(stepperPins[stepper_num][0], pos < 3);
    digitalWrite(stepperPins[stepper_num][1], ((pos + 6) % 8) < 3);
    digitalWrite(stepperPins[stepper_num][2], ((pos + 4) % 8) < 3);
    digitalWrite(stepperPins[stepper_num][3], ((pos + 2) % 8) < 3);
    driveStep[stepper_num] = (pos + 1) % 8;
}

// Take the specified amount of steps for a stepper connected to the specified pins, with a
// specified delay (in microseconds) between each step.
void FlipClock::step_num(const byte stepper_num, unsigned int steps, const unsigned int wait)
{
    stepperPos[stepper_num] = (stepperPos[stepper_num] + steps) % stepsPerRev;
    while (steps > 0)
    {
        half_step(stepper_num);
        steps--;
        delayMicroseconds(wait);
    }
}

// Step until the endstop is pressed and released, and update the stepper position to 0.
// If homing doesn't end after 2 rotations, the endstop is assumed to have failed and the program aborts.
void FlipClock::step_to_home(const byte stepper_num, const unsigned int wait)
{
    unsigned int total_steps = 0;
    byte endstopRepeats = 0;
    int i = 0;
    // Step until endstop reads low ENDSTOP_DEBOUNCE_READS times in a row
    while (endstopRepeats < endstopDebounceReads)
    {
        endstopRepeats = digitalRead(endstopPins[stepper_num]) == LOW ? endstopRepeats + 1 : 0;
        if (digitalRead(endstopPins[stepper_num]) == LOW)
            Serial.println("LOW");

        half_step(stepper_num);
        total_steps++;
        if (total_steps > stepsPerRev * 2u)
        {
            Serial.println("Loop1: too many steps");
            disable_stepper(stepper_num);
            e_stop();
        }
        delayMicroseconds(wait);
    }
    endstopRepeats = 0;

    Serial.println("First loop passed");

    // Step until endstop reads high ENDSTOP_DEBOUNCE_READS times in a row
    while (endstopRepeats < endstopDebounceReads)
    {
        endstopRepeats = digitalRead(endstopPins[stepper_num]) == HIGH ? endstopRepeats + 1 : 0;

        if (digitalRead(endstopPins[stepper_num]) == HIGH)
            Serial.println("HIGH");

        half_step(stepper_num);
        total_steps++;
        if (total_steps > stepsPerRev * 2u)
        {
            Serial.println("Loop2: too many steps");
            disable_stepper(stepper_num);
            e_stop();
        }
        delayMicroseconds(wait);
    }

    stepperPos[stepper_num] = 0;
}

// Step to the specified position. If the current position is greater than the target position, this
// function will re-home and then step to the target position.
void FlipClock::step_to_position(const byte stepper_num, unsigned int target_pos, const unsigned int wait)
{
    if (target_pos == stepperPos[stepper_num])
    {
        return;
    }

    // Limit target position to between 0 and STEPS_PER_REV-1
    target_pos %= stepsPerRev;

    if (target_pos < stepperPos[stepper_num])
    {
        step_to_home(stepper_num, wait);
        step_num(stepper_num, target_pos, wait);
    }
    else
    {
        step_num(stepper_num, target_pos - stepperPos[stepper_num], wait);
    }
}

// Steps to the specified digit on the display
// To save power, stepper is powered off after running, so exact positional accuracy is not maintained.
// This is allowable since the displays self-calibrate via the endstops every rotation.
void FlipClock::step_to_digit(const byte stepper_num, const byte digit, const unsigned int wait)
{
    // The ones display has 10 flaps, the others have 12 flaps
    const byte num_flaps = (stepper_num == 2) ? 10 : 12;

    const byte num_digits = (stepper_num == 0) ? 12 : (stepper_num == 1) ? 6
                                                                            : 10;

    const unsigned int target_pos = startingOffset[stepper_num] + (unsigned int)((num_digits + digit - startingDigits[stepper_num]) % num_digits) * stepsPerRev / num_flaps;

    // The tens display has 2 full sets of digits, so we'll need to step to the closest target digit.
    if (stepper_num == 1)
    {
        // The repeated digit is offset by a half-rotation from the first target.
        const unsigned int second_target = target_pos + stepsPerRev / 2;

        // If the current position is between the two target positions, step to the second target position, as it's the closest given that we can't step backwards.
        // Otherwise, step to the first target position.
        if (stepperPos[stepper_num] > target_pos && stepperPos[stepper_num] <= second_target)
        {
            step_to_position(stepper_num, second_target, wait);
        }
        else
        {
            step_to_position(stepper_num, target_pos, wait);
        }
    }
    else
    {
        // The ones and hour displays only have a single set of digits, so simply step to the target position.
        step_to_position(stepper_num, target_pos, wait);
    }

    disable_stepper(stepper_num);
}

// Stops the program. Flashes LED slowly if LED pin is defined.
void FlipClock::e_stop()
{
    Serial.println("e_stop() called");
    abort();
}