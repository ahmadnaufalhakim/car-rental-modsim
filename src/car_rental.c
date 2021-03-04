#include "simlib.h"
#include <stdio.h>

#define LAST_EVENT_LOCATION 3   /* Used to store last event location */
#define DESTINATION         4   /* Used to store which location a person will go to */
#define ARRIVAL_TIME        5   /* Used to store a person's arrival time */

#define LOCATION_TERMINAL_1 1   /* Terminal 1 location */
#define LOCATION_TERMINAL_2 2   /* Terminal 2 location */
#define LOCATION_CAR_RENTAL 3   /* Car rental location */

#define MAX_BUS_CAPACITY    20  /* Maximum bus queue capacity (to terminal 1 + to terminal 2 + to car rental) */

#define EVENT_PERSON_ARRIVAL    1   /* Person arrival event */
#define EVENT_BUS_ARRIVAL       4   /* Bus arrival event */
#define EVENT_BUS_DEPARTURE     5   /* Bus departure event */
#define EVENT_UNLOAD_PERSON     6   /* Unloading person from bus event */
#define EVENT_LOAD_PERSON       7   /* Loading person to bus event */
#define EVENT_END_SIMULATION    8   /* End simulation event */

#define LIST_TERMINAL_1         1   /* Queue in terminal 1 */
#define LIST_TERMINAL_2         2   /* Queue in terminal 2 */
#define LIST_CAR_RENTAL         3   /* Queue in car rental */
#define LIST_BUS_TO_TERMINAL_1  4   /* Queue inside bus for people going to terminal 1 */
#define LIST_BUS_TO_TERMINAL_2  5   /* Queue inside bus for people going to terminal 2 */
#define LIST_BUS_TO_CAR_RENTAL  6   /* Queue inside bus for people going to car rental */

#define INTERARRIVAL_TIME_RATE_TERMINAL_1    14  /* Exponential interarrival rate for terminal 1 */
#define INTERARRIVAL_TIME_RATE_TERMINAL_2    10  /* Exponential interarrival rate for terminal 2 */
#define INTERARRIVAL_TIME_RATE_CAR_RENTAL    24  /* Exponential interarrival rate for car rental */

#define STREAM_INTERARRIVAL_TERMINAL_1  1   /* Random-number stream for interarrival times at terminal 1 */
#define STREAM_INTERARRIVAL_TERMINAL_2  2   /* Random-number stream for interarrival times at terminal 2 */
#define STREAM_INTERARRIVAL_CAR_RENTAL  3   /* Random-number stream for interarrival times at car rental */
#define STREAM_UNLOADING                4   /* Random-number stream for time to unload people from the bus */
#define STREAM_LOADING                  5   /* Random-number stream for time to load people into the bus */
#define STREAM_DESTINATION              6   /* Random-number stream for determining destination of an arrival at the car rental */

#define VARIABLE_NUM_QUEUE_TERMINAL_1                           1   /* Number of person in terminal 1 queue */
#define VARIABLE_NUM_QUEUE_TERMINAL_2                           2   /* Number of person in terminal 2 queue */
#define VARIABLE_NUM_QUEUE_CAR_RENTAL                           3   /* Number of person in car rental queue */
#define VARIABLE_DELAY_QUEUE_TERMINAL_1                         4   /* Delay in terminal 1 queue */
#define VARIABLE_DELAY_QUEUE_TERMINAL_2                         5   /* Delay in terminal 2 queue */
#define VARIABLE_DELAY_QUEUE_CAR_RENTAL                         6   /* Delay in car rental queue */
#define VARIABLE_NUM_QUEUE_BUS                                  7   /* Number of person in bus queue */
#define VARIABLE_TIME_BUS_STOP_TERMINAL_1                       8   /* The time the bus stopped in terminal 1 */
#define VARIABLE_TIME_BUS_STOP_TERMINAL_2                       9   /* The time the bus stopped in terminal 2 */
#define VARIABLE_TIME_BUS_STOP_CAR_RENTAL                       10  /* The time the bus stopped in car rental */
#define VARIABLE_TIME_BUS_LOOP                                  11  /* The time for the bus to make a loop (departure from the car rental to the next such departure) */
#define VARIABLE_TIME_PERSON_IN_SYSTEM                          12  /* The time a person is in the system */

double bus_arrival_time = 0.0;
double bus_last_departure_car_rental = 0.0;

/* Function to calculate current number of person in the bus */
int current_bus_size() {
    return list_size[LIST_BUS_TO_TERMINAL_1] + list_size[LIST_BUS_TO_TERMINAL_2] + list_size[LIST_BUS_TO_CAR_RENTAL];
}

void print_stats() {
    printf("\n================ [STATS] ================");
    
    printf("\n(a) Average and maximum number in each queue");
    timest(0, -VARIABLE_NUM_QUEUE_TERMINAL_1);
    printf("\n1. Terminal 1   :   - Average: %.3lf", transfer[1]);
    printf("\n                    - Maximum: %.3lf", transfer[2]);
    timest(0, -VARIABLE_NUM_QUEUE_TERMINAL_2);
    printf("\n2. Terminal 2   :   - Average: %.3lf", transfer[1]);
    printf("\n                    - Maximum: %.3lf", transfer[2]);
    timest(0, -VARIABLE_NUM_QUEUE_CAR_RENTAL);
    printf("\n3. Car Rental   :   - Average: %.3lf", transfer[1]);
    printf("\n                    - Maximum: %.3lf", transfer[2]);

    printf("\n(b) Average and maximum delay in each queue");
    sampst(0, -VARIABLE_DELAY_QUEUE_TERMINAL_1);
    printf("\n1. Terminal 1   :   - Average: %.3lf", transfer[1]);
    printf("\n                    - Maximum: %.3lf", transfer[3]);
    sampst(0, -VARIABLE_DELAY_QUEUE_TERMINAL_2);
    printf("\n2. Terminal 2   :   - Average: %.3lf", transfer[1]);
    printf("\n                    - Maximum: %.3lf", transfer[3]);
    sampst(0, -VARIABLE_DELAY_QUEUE_CAR_RENTAL);
    printf("\n3. Car Rental   :   - Average: %.3lf", transfer[1]);
    printf("\n                    - Maximum: %.3lf", transfer[3]);

    printf("\n(c) Average and maximum number on the bus");
    timest(0, -VARIABLE_NUM_QUEUE_BUS);
    printf("\n- Average: %.3lf", transfer[1]);
    printf("\n- Maximum: %.3lf", transfer[2]);

    printf("\n(d) Average, maximum, and minimum time the bus stopped at each location");
    sampst(0, -VARIABLE_TIME_BUS_STOP_TERMINAL_1);
    printf("\n1. Terminal 1   :   - Average: %.3lf", transfer[1]);
    printf("\n                    - Maximum: %.3lf", transfer[3]);
    printf("\n                    - Minimum: %.3lf", transfer[4]);
    sampst(0, -VARIABLE_TIME_BUS_STOP_TERMINAL_2);
    printf("\n2. Terminal 2   :   - Average: %.3lf", transfer[1]);
    printf("\n                    - Maximum: %.3lf", transfer[3]);
    printf("\n                    - Minimum: %.3lf", transfer[4]);
    sampst(0, -VARIABLE_TIME_BUS_STOP_CAR_RENTAL);
    printf("\n3. Car Rental   :   - Average: %.3lf", transfer[1]);
    printf("\n                    - Maximum: %.3lf", transfer[3]);
    printf("\n                    - Minimum: %.3lf", transfer[4]);

    printf("\n(e) Average, maximum, and minimum time the bus making a loop");
    sampst(0, -VARIABLE_TIME_BUS_LOOP);
    printf("\n- Average: %.3lf", transfer[1]);
    printf("\n- Maximum: %.3lf", transfer[3]);
    printf("\n- Minimum: %.3lf", transfer[4]);

    printf("\n(f) Average, maximum, and minimum time a person is in the system");
    sampst(0, -VARIABLE_TIME_PERSON_IN_SYSTEM);
    printf("\n- Average: %.3lf", transfer[1]);
    printf("\n- Maximum: %.3lf", transfer[3]);
    printf("\n- Minimum: %.3lf", transfer[4]);
}

void event_person_arrival() {
    if (transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_1) {
        // Record a person in terminal 1 queue
        transfer[ARRIVAL_TIME] = sim_time;
        transfer[DESTINATION] = LOCATION_CAR_RENTAL;
        list_file(LAST, LIST_TERMINAL_1);

        // Keep track of number of person in terminal 1 queue
        timest(list_size[LIST_TERMINAL_1], VARIABLE_NUM_QUEUE_TERMINAL_1);

        // Schedule next person arrival event in terminal 1
        transfer[LAST_EVENT_LOCATION] = LOCATION_TERMINAL_1;
        event_schedule(sim_time + expon(60.0 * 60.0 / INTERARRIVAL_TIME_RATE_TERMINAL_1, STREAM_INTERARRIVAL_TERMINAL_1), EVENT_PERSON_ARRIVAL);

    } else if (transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_2) {
        // Record a person in terminal 2 queue
        transfer[ARRIVAL_TIME] = sim_time;
        transfer[DESTINATION] = LOCATION_CAR_RENTAL;
        list_file(LAST, LIST_TERMINAL_2);

        // Keep track of number of person in terminal 2 queue
        timest(list_size[LIST_TERMINAL_2], VARIABLE_NUM_QUEUE_TERMINAL_2);

        // Schedule next person arrival event in terminal 2
        transfer[LAST_EVENT_LOCATION] = LOCATION_TERMINAL_2;
        event_schedule(sim_time + expon(60.0 * 60.0 / INTERARRIVAL_TIME_RATE_TERMINAL_2, STREAM_INTERARRIVAL_TERMINAL_2), EVENT_PERSON_ARRIVAL);

    } else {
        transfer[ARRIVAL_TIME] = sim_time;

        // Deciding which destination the person will go by using probability distribution
        double prob_distrib[2] = {0.583, 1};
        int destination = random_integer(prob_distrib, STREAM_DESTINATION);
        transfer[DESTINATION] = (destination == 1)
            ? LOCATION_TERMINAL_1
            : LOCATION_TERMINAL_2;

        // Record a person in car rental queue
        list_file(LAST, LIST_CAR_RENTAL);

        // Keep track of number of person in car rental queue
        timest(list_size[LIST_CAR_RENTAL], VARIABLE_NUM_QUEUE_CAR_RENTAL);

        // Schedule next person arrival event in car rental
        transfer[LAST_EVENT_LOCATION] = LOCATION_CAR_RENTAL;
        event_schedule(sim_time + expon(60.0 * 60.0 / INTERARRIVAL_TIME_RATE_CAR_RENTAL, STREAM_INTERARRIVAL_CAR_RENTAL), EVENT_PERSON_ARRIVAL);
    }
}

void event_bus_arrival() {
    bus_arrival_time = sim_time;

    if (transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_1) {
        transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_1;
        if (list_size[LIST_BUS_TO_TERMINAL_1] > 0) {
            event_schedule(sim_time + uniform(16, 24, STREAM_UNLOADING), EVENT_UNLOAD_PERSON);
        } else if (list_size[LIST_TERMINAL_1] > 0 && current_bus_size() < MAX_BUS_CAPACITY) {
            event_schedule(sim_time + uniform(15, 25, STREAM_LOADING), EVENT_LOAD_PERSON);
        } else {
            event_schedule(bus_arrival_time + 5.0 * 60.0, EVENT_BUS_DEPARTURE);
        }
    } else if (transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_2) {
        transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_2;
        if (list_size[LIST_BUS_TO_TERMINAL_2] > 0) {
            event_schedule(sim_time + uniform(16, 24, STREAM_UNLOADING), EVENT_UNLOAD_PERSON);
        } else if (list_size[LIST_TERMINAL_2] > 0 && current_bus_size() < MAX_BUS_CAPACITY) {
            event_schedule(sim_time + uniform(15, 25, STREAM_LOADING), EVENT_LOAD_PERSON);
        } else {
            event_schedule(bus_arrival_time + 5.0 * 60.0, EVENT_BUS_DEPARTURE);
        }
    } else {
        transfer[LAST_EVENT_LOCATION] == LOCATION_CAR_RENTAL;
        if (list_size[LIST_BUS_TO_CAR_RENTAL] > 0) {
            event_schedule(sim_time + uniform(16, 24, STREAM_UNLOADING), EVENT_UNLOAD_PERSON);
        } else if (list_size[LIST_CAR_RENTAL] > 0 && current_bus_size() < MAX_BUS_CAPACITY) {
            event_schedule(sim_time + uniform(15, 25, STREAM_LOADING), EVENT_LOAD_PERSON);
        } else {
            event_schedule(bus_arrival_time + 5.0 * 60.0, EVENT_BUS_DEPARTURE);
        }
    }
}

void event_bus_departure() {
    if (transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_1) {
        sampst(sim_time - bus_arrival_time, VARIABLE_TIME_BUS_STOP_TERMINAL_1);
        transfer[LAST_EVENT_LOCATION] = LOCATION_TERMINAL_2;
        event_schedule(sim_time + 1.0 / 30.0 * 60.0 * 60.0, EVENT_BUS_ARRIVAL);
    } else if (transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_2) {
        sampst(sim_time - bus_arrival_time, VARIABLE_TIME_BUS_STOP_TERMINAL_2);
        transfer[LAST_EVENT_LOCATION] = LOCATION_CAR_RENTAL;
        event_schedule(sim_time + 4.5 / 30.0 * 60.0 * 60.0, EVENT_BUS_ARRIVAL);
    } else {
        sampst(sim_time - bus_last_departure_car_rental, VARIABLE_TIME_BUS_LOOP);
        bus_last_departure_car_rental = sim_time;
        sampst(sim_time - bus_arrival_time, VARIABLE_TIME_BUS_STOP_CAR_RENTAL);
        transfer[LAST_EVENT_LOCATION] = LOCATION_TERMINAL_1;
        event_schedule(sim_time + 4.5 / 30.0 * 60.0 * 60.0, EVENT_BUS_ARRIVAL);
    }
}

void event_unload_person() {
    if (transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_1) {
        list_remove(FIRST, LIST_BUS_TO_TERMINAL_1);
        timest(current_bus_size(), VARIABLE_NUM_QUEUE_BUS);
        sampst(sim_time - transfer[ARRIVAL_TIME], VARIABLE_TIME_PERSON_IN_SYSTEM);

        transfer[LAST_EVENT_LOCATION] = LOCATION_TERMINAL_1;
        if (list_size[LIST_BUS_TO_TERMINAL_1] > 0) {
            event_schedule(sim_time + uniform(16, 24, STREAM_UNLOADING), EVENT_UNLOAD_PERSON);
        } else if (list_size[LIST_TERMINAL_1] > 0 && current_bus_size() < MAX_BUS_CAPACITY) {
            event_schedule(sim_time + uniform(15, 25, STREAM_LOADING), EVENT_LOAD_PERSON);
        } else {
            if ((sim_time - bus_arrival_time) < (5.0 * 60.0)) {
                event_schedule(bus_arrival_time + 5.0 * 60.0, EVENT_BUS_DEPARTURE);
            } else {
                event_schedule(sim_time, EVENT_BUS_DEPARTURE);
            }
        }
    } else if (transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_2) {
        list_remove(FIRST, LIST_BUS_TO_TERMINAL_2);
        timest(current_bus_size(), VARIABLE_NUM_QUEUE_BUS);
        sampst(sim_time - transfer[ARRIVAL_TIME], VARIABLE_TIME_PERSON_IN_SYSTEM);

        transfer[LAST_EVENT_LOCATION] = LOCATION_TERMINAL_2;
        if (list_size[LIST_BUS_TO_TERMINAL_2] > 0) {
            event_schedule(sim_time + uniform(16, 24, STREAM_UNLOADING), EVENT_UNLOAD_PERSON);
        } else if (list_size[LIST_TERMINAL_2] > 0 && current_bus_size() < MAX_BUS_CAPACITY) {
            event_schedule(sim_time + uniform(15, 25, STREAM_LOADING), EVENT_LOAD_PERSON);
        } else {
            if ((sim_time - bus_arrival_time) < (5.0 * 60.0)) {
                event_schedule(bus_arrival_time + 5.0 * 60.0, EVENT_BUS_DEPARTURE);
            } else {
                event_schedule(sim_time, EVENT_BUS_DEPARTURE);
            }
        }
    } else {
        list_remove(FIRST, LIST_BUS_TO_CAR_RENTAL);
        timest(current_bus_size(), VARIABLE_NUM_QUEUE_BUS);
        sampst(sim_time - transfer[ARRIVAL_TIME], VARIABLE_TIME_PERSON_IN_SYSTEM);

        transfer[LAST_EVENT_LOCATION] = LOCATION_CAR_RENTAL;
        if (list_size[LIST_BUS_TO_CAR_RENTAL] > 0) {
            event_schedule(sim_time + uniform(16, 24, STREAM_UNLOADING), EVENT_UNLOAD_PERSON);
        } else if (list_size[LIST_CAR_RENTAL] > 0 && current_bus_size() < MAX_BUS_CAPACITY) {
            event_schedule(sim_time + uniform(15, 25, STREAM_LOADING), EVENT_LOAD_PERSON);
        } else {
            if ((sim_time - bus_arrival_time) < (5.0 * 60.0)) {
                event_schedule(bus_arrival_time + 5.0 * 60.0, EVENT_BUS_DEPARTURE);
            } else {
                event_schedule(sim_time, EVENT_BUS_DEPARTURE);
            }
        }
    }
}

void event_load_person() {
    if (transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_1) {
        list_remove(FIRST, LIST_TERMINAL_1);
        timest(list_size[LIST_TERMINAL_1], VARIABLE_NUM_QUEUE_TERMINAL_1);
        sampst(sim_time - transfer[ARRIVAL_TIME], VARIABLE_DELAY_QUEUE_TERMINAL_1);
        transfer[ARRIVAL_TIME] = sim_time;
        list_file(LAST, LIST_BUS_TO_CAR_RENTAL);
        timest(current_bus_size(), VARIABLE_NUM_QUEUE_BUS);

        transfer[LAST_EVENT_LOCATION] = LOCATION_TERMINAL_1;
        if (list_size[LIST_TERMINAL_1] > 0 && current_bus_size() < MAX_BUS_CAPACITY) {
            event_schedule(sim_time + uniform(15, 25, STREAM_LOADING), EVENT_LOAD_PERSON);
        } else {
            if ((sim_time - bus_arrival_time) < (5.0 * 60.0)) {
                event_schedule(bus_arrival_time + 5.0 * 60.0, EVENT_BUS_DEPARTURE);
            } else {
                event_schedule(sim_time, EVENT_BUS_DEPARTURE);
            }
        }
    } else if (transfer[LAST_EVENT_LOCATION] == LOCATION_TERMINAL_2) {
        list_remove(FIRST, LIST_TERMINAL_2);
        timest(list_size[LIST_TERMINAL_2], VARIABLE_NUM_QUEUE_TERMINAL_2);
        sampst(sim_time - transfer[ARRIVAL_TIME], VARIABLE_DELAY_QUEUE_TERMINAL_2);
        transfer[ARRIVAL_TIME] = sim_time;
        list_file(LAST, LIST_BUS_TO_CAR_RENTAL);
        timest(current_bus_size(), VARIABLE_NUM_QUEUE_BUS);

        transfer[LAST_EVENT_LOCATION] = LOCATION_TERMINAL_2;
        if (list_size[LIST_TERMINAL_2] > 0 && current_bus_size() < MAX_BUS_CAPACITY) {
            event_schedule(sim_time + uniform(15, 25, STREAM_LOADING), EVENT_LOAD_PERSON);
        } else {
            if ((sim_time - bus_arrival_time) < (5.0 * 60.0)) {
                event_schedule(bus_arrival_time + 5.0 * 60.0, EVENT_BUS_DEPARTURE);
            } else {
                event_schedule(sim_time, EVENT_BUS_DEPARTURE);
            }
        }
    } else {
        list_remove(FIRST, LIST_CAR_RENTAL);
        timest(list_size[LIST_CAR_RENTAL], VARIABLE_NUM_QUEUE_CAR_RENTAL);
        sampst(sim_time - transfer[ARRIVAL_TIME], VARIABLE_DELAY_QUEUE_CAR_RENTAL);
        transfer[ARRIVAL_TIME] = sim_time;
        if (transfer[DESTINATION] == LOCATION_TERMINAL_1) {
            list_file(LAST, LIST_BUS_TO_TERMINAL_1);
        } else {
            list_file(LAST, LIST_BUS_TO_TERMINAL_2);
        }
        timest(current_bus_size(), VARIABLE_NUM_QUEUE_BUS);

        transfer[LAST_EVENT_LOCATION] = LOCATION_CAR_RENTAL;
        if (list_size[LIST_CAR_RENTAL] > 0 && current_bus_size() < MAX_BUS_CAPACITY) {
            event_schedule(sim_time + uniform(15, 25, STREAM_LOADING), EVENT_LOAD_PERSON);
        } else {
            if ((sim_time - bus_arrival_time) < (5.0 * 60.0)) {
                event_schedule(bus_arrival_time + 5.0 * 60.0, EVENT_BUS_DEPARTURE);
            } else {
                event_schedule(sim_time, EVENT_BUS_DEPARTURE);
            }
        }
    }
}

void init_model() {
    int interarrival_time_rate[3] = {14, 10, 24};
    for (int i = 0; i < 3; i++) {
        transfer[LAST_EVENT_LOCATION] = i+1;
        event_schedule(sim_time + expon(60.0 * 60.0 / interarrival_time_rate[i], i+1), EVENT_PERSON_ARRIVAL);
    }
    transfer[LAST_EVENT_LOCATION] = LOCATION_TERMINAL_1;
    event_schedule(sim_time + 4.5 / 30.0 * 60.0 * 60.0, EVENT_BUS_ARRIVAL);
    event_schedule(sim_time + 80.0 * 60.0 * 60.0, EVENT_END_SIMULATION);

    sampst(0.0, 0);
    timest(0.0, 0);
}

int main() {
    printf("Simulation start time: %.5f", sim_time);
    init_simlib();
    init_model();
    while (next_event_type != EVENT_END_SIMULATION) {
        timing();
        switch (next_event_type) {
            case EVENT_PERSON_ARRIVAL :
                event_person_arrival();
                break;
            case EVENT_BUS_ARRIVAL :
                event_bus_arrival();
                break;
            case EVENT_BUS_DEPARTURE :
                event_bus_departure();
                break;
            case EVENT_UNLOAD_PERSON :
                event_unload_person();
                break;
            case EVENT_LOAD_PERSON :
                event_load_person();
                break;
        }
    }
    out_sampst(stdout, 1, 12);
    out_timest(stdout, 1, 12);
    printf("Simulation finish time: %.5f", sim_time);
    print_stats();
    return 0;
}