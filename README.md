# 📘 Project Overview

This project implements a Payroll Management System on an Arduino Uno, designed to receive and process serial messages to manage employee payroll accounts. The system supports account creation, modification, deletion, and dynamic display on an LCD shield, providing a rich embedded interaction model between hardware inputs and serial communication.

The program architecture is based on Finite State Machines (FSM) for robust state handling during initialization, synchronization, and main operation phases. Each phase ensures deterministic transitions and predictable serial/LCD behavior under constrained memory conditions.

# ⚙️ System Features
Core Features (BASIC Implementation)

Serial Communication Protocol

Custom message parsing (ADD, SAL, GRD, PST, CJT, DEL).

Rigid syntax enforcement with real-time ERROR: feedback.

Periodic synchronization phase using BEGIN handshake.

Controlled character handling (\r, \n) per protocol specification.

LCD Display Interface

Two-line LCD dynamically showing current payroll record.

Structured layout:

+----------------+
|^A BCDE £££££.££|
|v1234567 FGHIJKL|
+----------------+


Scroll navigation via UP/DOWN buttons.

Backlight color logic:

🟩 Green – Employee enrolled in pension (PEN)

🟥 Red – Not enrolled (NPEN)

🟪 Purple – Student ID display mode

Button Control Logic

UP / DOWN: Scroll through accounts.

SELECT (held >1s): Displays student ID + free SRAM (if FREERAM implemented).

Input debouncing handled to prevent rapid state switching.

Error Handling

Validation of ID format, job grade, job title, salary bounds, and pension status.

Real-time ERROR: serial feedback.

Safe rejection of malformed or duplicate commands.

# 🧠 Architecture & Finite State Machines
FSM 1 – System Lifecycle Control
State	Description	Transitions
BOOT	Arduino startup; LCD set to yellow	→ SYNC_WAIT
SYNC_WAIT	Repeatedly sends R until BEGIN received	→ RUNNING on valid sync
RUNNING	Main operation; processes incoming serial messages	↔ RUNNING, DISPLAY_ID
DISPLAY_ID	Triggered when SELECT held; shows student ID and optional SRAM data	→ RUNNING on release
FSM 2 – Serial Command Parser
State	Description	Transitions
IDLE	Waiting for incoming serial data	→ READ_CMD
READ_CMD	Detects message prefix (ADD, SAL, etc.)	→ VALIDATE
VALIDATE	Parses and validates syntax	→ EXECUTE / ERROR
EXECUTE	Executes operation and updates display	→ IDLE
ERROR	Sends ERROR: to Serial	→ IDLE

This two-tier FSM model ensures non-blocking behavior, allowing concurrent input reading while maintaining display responsiveness.

# 💾 Memory Management

SRAM Conservation:

Static arrays for payroll records with a configurable maximum account cap to avoid fragmentation.

Use of F() macro to store constant strings in Flash memory (PROGMEM).

EEPROM Integration (Extension):

Persistent account storage across power cycles.

Controlled activation using ROM-[phrase] command.

EEPROM entries validated via unique sync phrase.

🔧 Implemented Extensions
Extension	Description	Status
UDCHARS	Custom up/down arrow LCD characters	✅ Implemented
FREERAM	Displays available SRAM in ID mode	✅ Implemented
EEPROM	Persistent data storage	✅ Implemented
SCROLL	Scrolls job titles exceeding LCD width	✅ Implemented
HCI	Interactive pension filtering + tax/pension views	✅ Implemented

After synchronization, the Arduino reports:

UDCHARS,FREERAM,EEPROM,SCROLL,HCI

# 🧮 Tax & Pension Logic (HCI Extension)

Pension Contribution: 6.1% of gross salary

Monthly Salary View: Salary ÷ 12

Tax Bands:

Band	Range (£)	Rate
Personal Allowance	0 – 12,570	0%
Basic Rate	12,571 – 50,270	20%
Higher Rate	50,271 – 125,140	40%
Additional Rate	>125,140	45%

Displayed dynamically in the HCI mode through sequential RIGHT button presses.

# 🧩 Hardware Setup

Components Used:

Arduino Uno R3

Adafruit RGB LCD Shield (I²C interface)

Onboard navigation buttons (UP, DOWN, LEFT, RIGHT, SELECT)

USB serial interface (for command input)

Libraries:

#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <EEPROM.h>
#include <avr/eeprom.h>
#include <TimeLib.h>
#include <TimerOne.h>
#include <MemoryFree.h>

# 🧪 Debugging Process

Serial Echo Validation: Debug lines prefixed with DEBUG: for safe exclusion from automated testing.

State Visualization: Printed state transitions over Serial during development.

Incremental Testing:

Tested ADD → SAL → PST sequence before extending FSM.

Verified error detection with malformed commands.

Integrated LCD logic last to isolate parsing bugs from display errors.

Stress Testing:

Added up to N accounts until near-SRAM limit to validate memory safety.
