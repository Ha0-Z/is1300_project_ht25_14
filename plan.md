Hi, I need to work with you to do a project in embedded system.
In this project we are going to do a representation of a crossroad with stm nucleo-l476rg and a costum shield.
We will start with to create a software and hard archeteture. with as much detail as possible, but we will stick with the surface. We will work concrete later on, but for now we stick with the abstract level.

Things to consider in the project (Since the course contains multple modules things to consider will be divide to same format as the course layout.):
Module 1:
[From lecture 1]
In report all source of information should be referenced with ieee style. 
The report should be written in latex.
We need to do the project in following design process: [Requirements -> Specification -> System Design & Modeling ->  Hardware Design /  Software Design -> System Integration -> System Testing].(This have a huge weight in report. We don't need need to follow this strictly in project execution phase.)
Good aproach is test driven development. (ref 1: lecture1. ref 2: J. W. Grenning, Test Driven Development for Embedded C, 1st ed. Raleigh: Pragmatic
Programmers, LLC, The, 2011. )
It's good to have brief introduction to the  archetecture of the nucleo board, incuding cpu, memory, gpio, boot, Nested Vector Interrupt Controller(nvic), vector tables, power supply, clock in the report. 

Module 2 programming and project managemetns:
[From lecture c programming]
In the project the stack and heap could be used to allocate the memory, in report you need to explain why.
The memory layout could be used in the project development is text, Initialized Data Segment, Uninitialized Data Segment, Stack, Heap and Environment
Variables. 
Good datatype to use in project development: Pointers, volatile, cost, static in side function...

[From development process lecture]
The verification and testing need to occur from system design to System interation.(design process)
In requirement process have 3 types of requirements. Functional requirement(describe the system features), quality requirements(performance, realibility, testability and more) and architectural requirements(how sub systems connects, naming conventions and more)
For hardware design contains design interface to the envorioment(actuators, aensors), select hardware components(power considerations and consider availabitlity of components) design the hardware(pcb). But since we are being assigned with hardware therefore we do not need to go in to this part in the report.
For software design contains following, breaking down the archetecture to modules. Think about resuability, utilize existing firmware, RTOS and HAL library. We also need to consider design contraints like performance, timing properties and limited memory size.
We also need to do a graph of our software design in the report.
Software design should we lay alot of effords.
For validation and testing we need to do unit test, subsystem test, system integration test, regression test and acceptance test. In practice we do not need do all the tests in project execution but we need to take them up in report. We some time not only need to verify the functionality but also  analyse the timing properties, memory consumption and more.
Good approach of validation and testing is V-model XT.  
[End development prcess lecture]
The source code should be version controll with git.
The source code documentation should be documentated with doxygen. 

Module 3 Hardware:
[Block Diagram and Schematics lecture]
We need to use block diagram to describe the system archetecure of the porject.
We can also reference the schematics of the hardware.
[Power Supply lecture]
There are sevral power provider options for nucleo board: USB, VIN, E5V
In the report we should cover briefly about the power of our platform.
[Digital and Analog Interfacesv lecture]
There are mutiple factors to consider for io. For output we can think about Push-pull, Open drain. For input there are floating, pull-up, pull-down. We need to discuss it in the report.(All of them can we choose in gpio configuration on nucleoboard for each pin.)
We also need to consider debouncing problem.
There are sevral sensors on the board, but since it is not required to solve in the project instruction therefore we can skip them. Same reason apply to analog inputs. Also skip the actuators(motor) and drive circuitry.
[Communication lecture]
There are different type of communications we can ignore wireless communication, for over cable we have CAN intergrated but we are not going to use them, we will use usb to communicate with the board and the computer.(RS232/UART and FlexRay are in the lecture slide but we will ignore them), lastly between circuits, we are going to use spi and skip i2c USART/UART.  
For SPI, we need to think about there are 4 interface(SDO, SDI, SCLK, CS) + ground. SPI is a simple interface, Can be programmed by bit-banging, The MCU has a dedicated hardware interface for SPI. And there are 3 SPI interfaces on the board (40Mbit/s in controller mode / 24 Mbit/s in device mode). 
As I said previously we will skip i2c, rs232, can, bluetooth.

Module 4 Software:
[Software architecture]
As I said previously we need to have system diagram there are sevral factors to consider.
Since we will not go through them in project development therefore we will skip them for now. 
[Test Driven development]
Since this is a small project we will not have much testing, there fore the focus of tdd is to present in the report that we have coverd it up and have good understanding of it. We will not create a specific presedures.
[Executing without os]
Same principle here we will not create specific procedure in the project development.

Module 5 Real-Time System:
[Time Critical Computing]
We need to thinking about: deadline (hard, firm and soft deadline), Sqeuential/ Concurrent programming(but since the board only have one cpu core there will on sequential in the end). 
In the project we will have multiple process or thread. We should motivate and choose the most suitible option. Same logic applies to preemptive/ cooperative multitasking.    
Things about tasks which needs to be considering:
Time constant in each task instance, there are task with fixed time and variabled time. 
Periodic tasks (worst case execution time, period, deadline). 
Worst case execution time  can be measured and analysised. 
We also need to calculate the cpu utilization. 
[Real-Time Operating System]
Microkernal(External Interrupts, System Calls, Hardware / Software Exceptions, Clock
Interrupts)
We have following recouses in RTOS kernal: scheduler, function library, library of services, user-defined data.
The OS tasks we need to think about following: Preemption, Task States.
[Scheduling Algorithms]
We need to decide if a test is schedulable. 
There are several techniques, I did not understood this lecture very well, therefore I will only provide you the topic. 
Time Division Multiple Access, Round Robin Scheduling, Preemptive Multitasking, Fixed Priority Scheduling, Rate Monotonic Scheduling, Fixed Priority Scheduling, Utilization Bound RMS, Earliest Deadline First, Utilization Bound EDF, The Critical Instant.
In the report We could use simso.
[Communication and Shared Resources]
I will also only provide you the topic. 
Avoiding Interference, Mutual Exclusion, Critical Section, Mutex, Semaphore, Deadlock, Dining Philosophers, Priority Inheritance Protocol, Mailboxes and Queues, 

Module 6: Diverse:
[Low-Power and Timer]
Loss in CMOS Circuits, Reducing Power Consumption, Low Power Modes STML476RG, STM32L4 Low Power MCU, SysTick Timer, Independent Watchdog, Window Watchdog, Real Time Clock, TIM2 – TIM5, Time Base Unit, Counter Clock Selection, Counter Modes, Input Capture Mode, PWM Input Mode.
