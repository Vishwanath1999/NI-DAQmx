#include <stdio.h>
#include <stdlib.h>
#include "NIDAQmx.h"

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
    int32 error = 0;
    TaskHandle aiTaskHandle = 0;
    TaskHandle aoTaskHandle = 0;
    float64 readData = 0.0;
    float64 writeData[5] = {0.0};  // Array for AO0 to AO4 voltages

    // Create a task for analog input
    DAQmxErrChk(DAQmxCreateTask("AI_Task", &aiTaskHandle));
    DAQmxErrChk(DAQmxCreateAIVoltageChan(aiTaskHandle, "PXI1Slot2/ai0", "", DAQmx_Val_Diff, -10.0, 10.0, DAQmx_Val_Volts, NULL));
    
    // Create a task for analog output
    DAQmxErrChk(DAQmxCreateTask("AO_Task", &aoTaskHandle));
    DAQmxErrChk(DAQmxCreateAOVoltageChan(aoTaskHandle, "PXI1Slot3/ao0:4", "", -10.0, 10.0, DAQmx_Val_Volts, NULL));
    
    // Read voltage from AI0
    DAQmxErrChk(DAQmxStartTask(aiTaskHandle));
    DAQmxErrChk(DAQmxReadAnalogScalarF64(aiTaskHandle, 10.0, &readData, NULL));
    printf("Voltage read from AI0: %.3f V\n", readData);
    
    // Write voltages to AO0 to AO4
    writeData[0] = 1.0;  // AO0
    writeData[1] = 2.0;  // AO1
    writeData[2] = 3.0;  // AO2
    writeData[3] = 4.0;  // AO3
    writeData[4] = 5.0;  // AO4
    DAQmxErrChk(DAQmxWriteAnalogF64(aoTaskHandle, 1, 1, 10.0, DAQmx_Val_GroupByChannel, writeData, NULL, NULL));
    
    // Cleanup and exit
    printf("Voltages written to AO0 to AO4.\n");
    
Error:
    if (aiTaskHandle != 0)
    {
        DAQmxStopTask(aiTaskHandle);
        DAQmxClearTask(aiTaskHandle);
    }
    
    if (aoTaskHandle != 0)
    {
        DAQmxStopTask(aoTaskHandle);
        DAQmxClearTask(aoTaskHandle);
    }
    
    if (error != 0)
    {
        char errorBuffer[256];
        DAQmxGetExtendedErrorInfo(errorBuffer, 256);
        printf("DAQmx Error %d: %s\n", error, errorBuffer);
    }
    
    printf("Press Enter to quit.\n");
    getchar();
    return 0;
}
