Here is the documentation for the inputs and outputs of the commonly used APIs in the NI-DAQmx C API:

1. `int32 DAQmxCreateTask(const char taskName[], TaskHandle *taskHandle)`

   - Inputs:
     - `taskName`: A string specifying the name of the task to be created.
     - `taskHandle`: A pointer to a TaskHandle variable that will hold the reference to the created task.

   - Outputs:
     - `taskHandle`: The reference to the created task.

2. `int32 DAQmxCreateAIVoltageChan(TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], int32 terminalConfig, float64 minVal, float64 maxVal, int32 units, const char customScaleName[])`

   - Inputs:
     - `taskHandle`: The handle of the task to which the AI voltage channel will be added.
     - `physicalChannel`: A string specifying the physical channel name or the virtual channel name.
     - `nameToAssignToChannel`: A string specifying the name to assign to the channel.
     - `terminalConfig`: An integer specifying the terminal configuration of the channel (e.g., `DAQmx_Val_Diff` for differential or `DAQmx_Val_RSE` for referenced single-ended).
     - `minVal`: The minimum value expected to be measured by the AI channel.
     - `maxVal`: The maximum value expected to be measured by the AI channel.
     - `units`: An integer specifying the units of the measurements (e.g., `DAQmx_Val_Volts`).
     - `customScaleName`: A string specifying the name of the custom scale for the channel.

   - Outputs:
     - None.

3. `int32 DAQmxCreateAOVoltageChan(TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, const char customScaleName[])`

   - Inputs:
     - `taskHandle`: The handle of the task to which the AO voltage channel will be added.
     - `physicalChannel`: A string specifying the physical channel name or the virtual channel name.
     - `nameToAssignToChannel`: A string specifying the name to assign to the channel.
     - `minVal`: The minimum value expected to be output by the AO channel.
     - `maxVal`: The maximum value expected to be output by the AO channel.
     - `units`: An integer specifying the units of the output values (e.g., `DAQmx_Val_Volts`).
     - `customScaleName`: A string specifying the name of the custom scale for the channel.

   - Outputs:
     - None.

4. `int32 DAQmxWriteAnalogF64(TaskHandle taskHandle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, bool32 dataLayout, const float64 writeArray[], int32 *sampsPerChanWritten, bool32 *reserved)`

   - Inputs:
     - `taskHandle`: The handle of the task to which the analog output will be written.
     - `numSampsPerChan`: The number of samples per channel to write.
     - `autoStart`: A boolean value specifying whether the task should start automatically.
     - `timeout`: The timeout value in seconds.
     - `dataLayout`: A boolean value specifying the layout of the writeArray data.
     - `writeArray`: An array of float64 values containing the data to be written.

   - Outputs:
     - `sampsPerChanWritten`: The actual number of samples per channel written.
     - `reserved`: Reserved for future use.

5.

 `int32 DAQmxReadAnalogF64(TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, float64 readArray[], int32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved)`

   - Inputs:
     - `taskHandle`: The handle of the task from which the analog input will be read.
     - `numSampsPerChan`: The number of samples per channel to read.
     - `timeout`: The timeout value in seconds.
     - `fillMode`: A boolean value specifying the fill mode for incomplete reads.
     - `arraySizeInSamps`: The size of the readArray in samples.

   - Outputs:
     - `readArray`: An array of float64 values containing the read data.
     - `sampsPerChanRead`: The actual number of samples per channel read.
     - `reserved`: Reserved for future use.

6. `int32 DAQmxWaitUntilTaskDone(TaskHandle taskHandle, float64 timeout)`

   - Inputs:
     - `taskHandle`: The handle of the task to wait for completion.
     - `timeout`: The timeout value in seconds.

   - Outputs:
     - None.

7. `int32 DAQmxStopTask(TaskHandle taskHandle)`

   - Inputs:
     - `taskHandle`: The handle of the task to stop.

   - Outputs:
     - None.

8. `int32 DAQmxClearTask(TaskHandle taskHandle)`

   - Inputs:
     - `taskHandle`: The handle of the task to clear.

   - Outputs:
     - None.

Note: The inputs and outputs mentioned here are based on the general usage of the APIs. For more detailed information and additional parameters, refer to the official NI-DAQmx C API documentation and specific function reference.