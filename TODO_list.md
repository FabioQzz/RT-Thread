# TODO LIST (GENERAL)

#### 1. TASK SPECIFICATION
- **A**: Need one task to handle the sensor related to the sensing of the electrical potential of the cells (HARD)
  - **1.** Must have an associated task in order to generate the input data (periodically, e.g. 10samples/s)
- **B**: A task to count the heart beats number
  - **1.** Must have an associated task in order to generate the heartbeats (rate should vary)
- **C**: Task to check information coming from an external source (easily implemented as a wait on external signal)
- **D**: A task capable to trigger the leads (based on task (a) ) (HARD)
  - **1.** After triggering the leads, task **A** (or **B** )will check again the values and start **D** again eventually 
- **E**: Sending data to an external device
  - **1.** Triggered by **C**, should have low priority but it access a memory protected area 
- **F**: (OPTIONAL) Implement an automatic adjustment of the frequency based on heart activity
------------

#### 2. TASK IMPLEMENTATION
**Note 1**: We are negleting the impact of the task used to generate the values since they are needed only in an emulated environment
- For task **A**, the period depends on the computation we are making: in theory, this task should check whether or not the electrical values from the sensors are 'normal' or they are showing some problem with the patient. So it will be very likely a comparison between a certain average of the last N values with the received one. We are assuming a received value every 0.1 seconds. Another way to implement **A** is to check if the last set of M values is lower that a known threshold, so if i receive M values and each of them is lower that the threshold K, i should trigger task **D**.
- For **B**, we think is a good compromise to count all the received interrupts (heartbeats) in a certain amount of time (e.g. number of heartbeats in 1 minute). In this case, we can save an array where each cell corresponds to 1 minute and the value of each cell is the number of heartbeats. In this case, we need 1 byte for each cell (from 0 to 255 is enough to represent HB/min) and for one whole day we just need 1440 Bytes.
- There is not much to say for task **C**, we just need to instantiate the task and make it wait on a signal that can be sent from an external source.
- In the case of task **D**, we can directly create it from task **A**, since it's more like a one-shot task triggered very rarely. However, we should remember that it has the maximum priority over all other tasks. After it's completed, **A** (or **B**) will decide if it is necessary to run **D** again (Or we assume that **D** always works?).
- Very likely, **E** will send the heartbeat information (over time) to the external source. this task is not prioritized. The only issue is where we can save the incoming heartbeats when we are sending a set of the old ones.
------------

#### 3. TASK TIMING PARAMETERS
( Note: this section can be filled only when we estimate all T and WCET )
At the end, i will certainly have task with very high priorities and very short deadlines, and others with softer constraints.
I can assume periods equals to deadlines. This table will be filled according to parameters described in the next section. For a **first approximation**, i can set the following parameters:
1. TASK 'A': T = 100ms 	, WCET = ?
2. TASK 'B': T = 200ms 	, WCET = ?
3. TASK 'C': T = 1s		, WCET = ?
4. TASK 'D': T = 100ms 	, WCET = ?
5. TASK 'E': T = 1s		, WCET = ? 
6. TASK 'F': -- NOT CONSIDERED NOW --
------------

#### 4. HOW TO ESTIMATE EFFECTIVE T and WCET values?
To estimate these values, we will certainly need more information, mainly based on delays of sensors (read time, transmission time) useful to estimate WCETs.
Estimating periods is relatively easy in case of soft case, because it depends mainly on us (e.g. how many times per second/minute we want to send feedback to a remote device? )
