# Application 2

**Name:** Jonah Sprandel  
**UCFID:** 5439542  

---

## Space Theme

In this application, the system is themed as a **space satellite monitoring system**:  

- The **LED blink task** represents a status beacon on a satellite, providing a visual indication of system health.  
- The **sensor task** monitors the solar array’s light exposure, logging telemetry to ensure the satellite receives enough energy.  
- The **print task** simulates satellite communication updates sent to a ground station.  

Task priorities reflect their importance in a spacecraft system: sensor readings are critical and high priority, while status beacons and telemetry messages are less critical and lower priority. The LED blink task and print task are deemed to be of equal priority in this thematic space and are therefore given equal RTOS task priority. 

Also, after extensive troubleshooting, I found the LDR module output is **digital only**, not analog, so full testing of the lux calculation is limited. However, connecting the ESP32 ADC pin directly to the analog signal line on the LDR module (pin 5 of the LM393) produced reasonable readings. Conceptually, the sensor code works, even though continuous real-world analog testing is limited.

---

## Q1. Task Timing and Jitter

**Question:** Compare the timing of the LED blink and console print tasks with that of the sensor task. How regular is each task’s period in practice? For the sensor task (using `vTaskDelayUntil`), do the sensor readings and alert messages occur at consistent intervals (e.g. every 500 ms)? In contrast, do you observe any drift or variation in the LED blink or print intervals over time? Explain why `vTaskDelayUntil` provides a more stable period for the sensor task, referencing how it calculates the next wake-up tick based on an absolute time reference. What might cause jitter in the LED or print task periods when using `vTaskDelay`? (Hint: consider the effect of the sensor task running at the moment they are ready to run, and how that might delay them slightly.)  

**Answer:**  
In practice, each task's period is very regular. All the tasks occurred at consistent intervals, even the low-priority ones that used `vTaskDelay`. However, `vTaskDelayUntil` provides a more stable period because it calculates the next wake-up tick based on an absolute reference (initial tick + N × period), preventing error accumulation. In contrast, `vTaskDelay` relies on relative delays and may be briefly delayed due to preemption by the higher priority sensor task. In my space theme, this design ensures that **solar telemetry is precise**, while minor beacon or message delays are tolerable.

---

## Q2. Priority-Based Preemption

**Question:** Describe a scenario observed in your running system that demonstrates FreeRTOS’s priority-based preemptive scheduling. For example, what happens if the console print task is about to print (or even mid-way through printing) exactly when the sensor task’s next period arrives? Does the sensor task interrupt the print task immediately, or does it wait? Based on your understanding of FreeRTOS, which task would the scheduler choose to run at a moment when both become Ready, and why? Provide a brief timeline or example (using tick counts or event ordering) to illustrate the preemption. (*If you didn’t explicitly catch this in simulation, answer conceptually: assume the print task was running right when the sensor task unblocked – what should happen?*)  

**Answer:**  
The sensor task has priority 3, while the print and LED tasks have priority 1. If the print task is logging telemetry when the sensor task unblocks, the sensor task immediately preempts the print task to execute. At a moment when both become ready, the scheduler chooses the sensor task because it always selects the task with the higher priority. This design ensures critical satellite sensor tasks are guaranteed to meet deadlines, while less important tasks are still timely.

**Example timeline:**  
- Tick 1000 ms: Print task running.  
- Tick 1002 ms: Sensor task unblocks for solar array telemetry.  
- Scheduler preempts the print task.  
- Sensor task executes telemetry and logs light readings.  
- Sensor task finishes ~100 µs later.  
- Print task resumes exactly where it left off.  

---

## Q3. Effect of Task Execution Time

**Question:** In our design, all tasks have small execution times (they do minimal work before blocking again). Suppose the sensor task took significantly longer to execute (for instance, imagine it performed a complex calculation taking, say, 300 ms of CPU time per cycle). How would that affect the lower-priority tasks? Discuss what would happen if the sensor task’s execution time sometimes **exceeds** its period (i.e., it can’t finish its work before the next 500 ms tick). What symptoms would you expect to see in the system (e.g. missed readings, delayed LED toggles, etc.)? Relate this to real-time scheduling concepts like missed deadlines or CPU utilization from the RTOS theory (Chapters 3 and 6 of the Harder textbook). What options could a system designer consider if the high-priority task started starving lower tasks or missing its schedule (think about reducing workload, adjusting priorities, or using two cores)?

**Answer:**  
If the sensor task took significantly longer (e.g., 300 ms per cycle), lower-priority tasks like the beacon or communication updates could be delayed because the scheduler always prioritizes the highest-priority task that's ready. If the sensor task sometimes exceeds its 500 ms period, the system could miss LED toggles, printing from the print task, and sensor deadlines. The CPU utilization would be extremely high because the sensor task would be taking up all of the time, leaving no idle time. In the space theme context, this could affect satellite monitoring, power management, and general success of the satellite's operations. Designers could mitigate this by optimizing the sensor calculations to shorten the sensor task time, lowering the sensor priority to allow other tasks to run, use multiple cores, or increase the sensor task period.

---

## Q4. `vTaskDelay` vs `vTaskDelayUntil`

**Question:** Why did we choose `vTaskDelayUntil` for the sensor task instead of using `vTaskDelay` in a simple loop? Explain in your own words the difference between these two delay functions in FreeRTOS, and the specific problem that `vTaskDelayUntil` solves for periodic real-time tasks. Consider what could happen to the sensor sampling timing over many iterations if we used `vTaskDelay(500 ms)` instead – how might small errors accumulate? Also, for the LED blink task, why is using `vTaskDelay` acceptable in that context? (Think about the consequences of slight timing drift for a status LED vs. a sensor sampling task.)  

**Answer:**  
We chose `vTaskDelayUntil` for the sensor task because it schedules the next wake-up based on the absolute reference tick, maintaining precise timing. Using `vTaskDelay(500 ms)` could accumulate timing errors due to the relative timing. The difference between the two is that `vTaskDelayUntil` is absolute and `vTaskDelay` is relative. Using an absolute timing reference removes the possibility of drift due to variations in execution time. `vTaskDelay` delays a task by a certain amount of time after a task finishes, but it doesn't account for task execution time fluctuations. This is the specific problem that `vTaskDelayUntil` solves.

If we use `vTaskDelay(500 ms)` over many iterations for the sensor sampling, this could lead to drift in ADC sampling, meaning readings could gradually occur later than intended, which could be unacceptable for real-time monitoring systems like solar array light measurements.

For the LED beacon, using `vTaskDelay` is acceptable because minor timing drift does not affect the safety or operation of the spacecraft.

---

## Q5. Thematic Integration Reflection

**Question:** Relate the functioning of your three tasks to a real-world scenario in one of the thematic contexts (Space Systems, Healthcare, or Hardware Security). Describe an example of what each task could represent. Explain how task priority might reflect the importance of that function in the real system (e.g. why the sensor monitoring is high priority in a medical device). 

**Answer:**  
- **Sensor task:** Monitors solar array light and logs telemetry. High priority ensures reliable energy monitoring.  
- **LED blink task:** Status beacon for the spacecraft, low priority because slight timing drift is acceptable.  
- **Print task:** Simulates satellite communication updates. Medium priority provides informative logs without blocking critical telemetry.  

Task priorities mirror spacecraft system importance: critical tasks (solar monitoring) are high-priority, while visual and informational tasks can tolerate minor delays.

---

## Q6. Bonus – Starvation Experiment

**Question:** Design an experiment which causes starvation in the system (e.g., sensor task never gives time to the other tasks). Describe the code you used and the results. Leave the code in your environment but comment it out. Return your code for submission to a working ideal state. 

**Answer:**  
I designed a simple experiment to achieve starvation by keeping the sensor task at the highest priority but removing the delay. By removing the `vTaskDelayUntil` call, the sensor task continuously executes and never yields CPU time. The code is exactly the same as the normal sensor task, but there is no delay at the end.
