**Traffic Signaling System Simulation with FreeRTOS**

This program simulates a traffic signaling system using three tasks in FreeRTOS. The tasks are designed to represent different aspects of a traffic scenario, including normal traffic flow and emergency vehicle handling.

### Task Descriptions:

1. **Task 1 - Add Traffic:**
   - This task randomly adds traffic to any one lane among four lanes.
   - It waits for a random amount of time to simulate unbalanced traffic.
   - Uses a mutex to ensure safe access to the traffic lanes.

2. **Task 2 - Emergency Vehicle Arrival:**
   - Simulates the entry or arrival of an emergency vehicle in any lane at random.
   - Waits for a random amount of time before repeating.
   - Uses a mutex to ensure safe access to the traffic lanes.
   - Adds lane numbers to the emergency queue to signal the presence of an emergency vehicle.

3. **Task 3 - Traffic Signaling:**
   - Checks if there is an emergency vehicle waiting in any lane by monitoring the emergency queue.
   - If an emergency vehicle is present, it gives priority to that lane, clears the lane, and waits for the specified time.
   - If no emergency vehicle is present, it calculates the longest queue and gives green signal to that lane, clears the lane, and waits for the specified time.
   - Uses mutexes to ensure safe access to the traffic lanes and emergency queue.

### Highlights:

- **Mutexes:**
  - Mutexes are used to protect access to the traffic lanes. Each lane has its mutex to prevent simultaneous access by multiple tasks. This ensures data consistency and prevents race conditions.
  
- **Queues:**
  - A queue is used to manage emergency vehicles. When an emergency vehicle arrives, its lane number is added to the emergency queue. Task 3 monitors this queue to handle emergency situations efficiently.

### Code Overview:

- The code consists of three tasks, each representing a different aspect of the traffic signaling system.
- Task 1 adds traffic to lanes, Task 2 simulates emergency vehicle arrival, and Task 3 manages traffic signaling based on emergency situations and traffic congestion.
- Mutexes and queues are used to synchronize access to shared resources and manage communication between tasks.

### Conclusion:

This simulation demonstrates how FreeRTOS tasks, mutexes, and queues can be utilized to implement a robust traffic signaling system. By properly managing resources and handling emergency situations, the system ensures safe and efficient traffic flow in various scenarios.