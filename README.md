# Welcome to the unRTOS Revolution! 🚀

unRTOS is a lightweight, custom cooperative task scheduler designed for embedded applications. It provides multitasking capabilities without the overhead of a full Real-Time Operating System, perfectly tailored for microcontroller projects running on dual-core processors like the Raspberry Pi Pico's RP2040.

**Powered by unWire** - our revolutionary lock-free inter-core messaging architecture that turns cooperative scheduling constraints into performance advantages.

## Why unRTOS?

Traditional RTOS solutions can be overkill for embedded projects, introducing unnecessary complexity and memory overhead. unRTOS takes a different approach:

- **Lightweight**: Minimal memory footprint and CPU overhead
- **Deterministic**: Predictable timing behavior crucial for real-time applications
- **Dual-Core Ready**: Built from the ground up to leverage multi-core processors
- **unWire Messaging**: Revolutionary lock-free inter-core communication
- **Simple**: Easy to understand, debug, and extend
- **Flexible**: Adaptable to various embedded project requirements

## Core Features

### 🔄 Cooperative Multitasking
Tasks run cooperatively without preemption, giving you full control over when tasks yield execution.

### ⏱️ Tick-Based Scheduling
- 1ms timer ticks provide precise timing control
- Tasks can be scheduled with initial offsets and periodic intervals
- Automatic timer management and task execution

### 🔀 Multi-Core Support
- Seamless execution across multiple processor cores
- Core-specific task assignment
- Automatic core startup and synchronization

### 📨 unWire Inter-Core Messaging
Revolutionary **unWire** architecture provides lock-free message passing between cores and tasks. Each task has dedicated message channels from every core, eliminating synchronization overhead while maintaining deterministic behavior.

**Key unWire Features:**
- **Zero Synchronization**: No locks, no atomic operations, no waiting
- **Per-Core Channels**: Each core writes to its own dedicated message lane  
- **Cooperative Safety**: Natural atomicity from cooperative scheduling
- **Deterministic**: Predictable timing with no blocking operations

## Quick Start

### Adding Tasks

Use the convenient macro to add tasks with initialization:

```c
// Add a task that runs every 100ms after a 50ms initial delay
UNRTOS_ADD_INIT_TASK(0, my_task, 50, 100);
```

Or add tasks manually:

```c
// Initialize your task first
my_task_init();

// Then add it to the scheduler
unrtos_add_task(core_id, my_task_function, offset_ms, period_ms);
```

### Task Functions

Tasks are simple void functions:

```c
void my_task_function(void) {
    // Your task code here
    // Remember: this is cooperative - don't block for too long!
}
```

### unWire Messaging

Post messages to any task from any core - completely lock-free:

```c
// Post data to a specific task (works across cores!)
unrtos_task_message_post(target_task, data, data_size);
```

Tasks consume messages from all cores in their own execution context:

```c
// Subscribe to your own task's message channels
task_message_sub_t sub = unrtos_task_message_subscribe(my_task_function);

// Consume messages from all cores
uint8_t buffer[10];
size_t received = unrtos_task_message_consume(sub, buffer, sizeof(buffer));
```

**The unWire Magic:** Each task has separate message queues for each core (`unwire[core_id]`), so Core 0 and Core 1 can simultaneously post to the same task without any synchronization!

## Configuration

Customize unRTOS behavior in `unrtos_config.h`:

```c
#define unrtosNUM_CORES             (2)    // Number of cores to use
#define unrtosTICK_PERIOD_MS        (1)    // Timer tick period
#define unrtosMAX_TASKS             (10)   // Max tasks per core
#define unrtosMAX_TASK_MESSAGES     (25)   // Message queue size
#define unrtosMAX_TASK_MESSAGE_SUBS (5)    // Max message subscribers
```

## Architecture

### Startup Sequence
1. Core 0 starts and launches additional cores sequentially
2. Each core calls `app_setup(core_id)` for initialization
3. Scheduler starts on each core independently
4. Core 0 manages the global timer tick

### Scheduling Algorithm
1. Global timer increments every tick (1ms)
2. Each task has a countdown timer
3. When timer reaches 0, task executes
4. Timer resets to the task's period value
5. Process repeats for all tasks on each core

### unWire Message Architecture
The revolutionary unWire design eliminates the need for locks or synchronization:

```c
// Each task has per-core message channels
typedef struct unrtosTask_s {
    task_function_t fn;
    time_t period, tmr;
    unwire_t unwire[unrtosNUM_CORES];  // <- The unWire magic!
} unrtosTask_t;
```

**How unWire Works:**
- **Core 0** writes only to `task.unwire[0]`
- **Core 1** writes only to `task.unwire[1]`  
- **Task** reads from all `unwire[]` channels when it runs
- **Result**: Zero contention, zero locks, maximum performance

### Memory Layout
Tasks are stored in static arrays per core, keeping memory usage predictable and avoiding dynamic allocation. unWire channels add `unrtosNUM_CORES * unrtosUNWIRE_MAX_ITEMS` per task - a small price for lock-free messaging!

## Best Practices

### ✅ Do
- Keep task functions short and non-blocking
- Use the message system for inter-task communication
- Initialize tasks before adding them to the scheduler
- Monitor tick warnings for missed deadlines

### ❌ Don't
- Block for extended periods in task functions
- Add tasks after the scheduler starts
- Use blocking I/O operations
- Rely on task execution order within the same tick

## Use Cases

unRTOS with unWire is perfect for embedded applications requiring deterministic multi-core communication:
- **Display Systems**: Smooth, timed screen updates with cross-core UI coordination
- **Sensor Networks**: Lock-free data sharing between collection and processing cores
- **Control Systems**: Real-time monitoring with guaranteed message delivery
- **IoT Devices**: Concurrent communication and device logic without blocking
- **Game Engines**: Parallel game logic, rendering, and input with unWire message passing
- **Audio/Video Processing**: Multi-core pipelines with deterministic data flow

## File Structure

```
unrtos/
├── unrtos.c              # Main scheduler implementation
├── unrtos.h              # Public API
├── unrtos_config.h       # Configuration settings
├── unrtos_types.h        # Type definitions
├── README.md              # This file
├── boards/                # Board-specific configurations
├── hal/                   # Hardware abstraction layer
├── private/               # Internal implementation details
├── task_messages/         # unWire message passing system
└── util/                  # Utility functions
```

## Contributing

When extending unRTOS:
1. Keep the lightweight philosophy
2. Maintain deterministic behavior
3. Document configuration options
4. Test on both cores
5. Consider impact on existing tasks

---

*unRTOS Revolution powered by unWire - Making embedded multitasking simple, deterministic, and lock-free!*