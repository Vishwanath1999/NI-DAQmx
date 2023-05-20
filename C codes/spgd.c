#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "NIDAQmx.h"

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
    int32 error = 0;
    TaskHandle inputTaskHandle = 0;
    TaskHandle outputTaskHandle = 0;
    int32 nChannels = 4;
    float64* inputSignal = NULL;
    float64* outputSignal = NULL;
    float64* deltaV = NULL;
    float64* v = NULL;
    float64* posV = NULL;
    float64* negV = NULL;
    float64* inputSignalV = NULL;
    float64* inputSignalPlus = NULL;
    float64* inputSignalMinus = NULL;
    float64* gradient = NULL;
    int32 i;

    // Configure random number generator
    srand((unsigned int)time(NULL));

    // Allocate memory for arrays
    inputSignal = (float64*)malloc(nChannels * sizeof(float64));
    outputSignal = (float64*)malloc(nChannels * sizeof(float64));
    deltaV = (float64*)malloc(nChannels * sizeof(float64));
    v = (float64*)malloc(nChannels * sizeof(float64));
    posV = (float64*)malloc(nChannels * sizeof(float64));
    negV = (float64*)malloc(nChannels * sizeof(float64));
    inputSignalV = (float64*)malloc(nChannels * sizeof(float64));
    inputSignalPlus = (float64*)malloc(nChannels * sizeof(float64));
    inputSignalMinus = (float64*)malloc(nChannels * sizeof(float64));
    gradient = (float64*)malloc(nChannels * sizeof(float64));

    // Create input task
    DAQmxErrChk(DAQmxCreateTask("Input_Task", &inputTaskHandle));
    for (i = 0; i < nChannels; i++)
    {
        char channelName[20];
        sprintf(channelName, "PXI1Slot2/ai%d", i);
        DAQmxErrChk(DAQmxCreateAIVoltageChan(inputTaskHandle, channelName, "", DAQmx_Val_Diff, -10.0, 10.0, DAQmx_Val_Volts, NULL));
    }

    // Create output task
    DAQmxErrChk(DAQmxCreateTask("Output_Task", &outputTaskHandle));
    for (i = 0; i < nChannels; i++)
    {
        char channelName[20];
        sprintf(channelName, "PXI1Slot3/ao%d", i);
        DAQmxErrChk(DAQmxCreateAOVoltageChan(outputTaskHandle, channelName, "", -10.0, 10.0, DAQmx_Val_Volts, NULL));
    }

    // Generate random initial values for v
    for (i = 0; i < nChannels; i++)
    {
        v[i] = 2 * (float64)rand() / RAND_MAX;
    }

    clock_t startTime = clock();

    // Main loop
    for (i = 0; i < 10000; i++)
    {
        // Generate random deltaV values
        for (int j = 0; j < nChannels; j++)
        {
            deltaV[j] = 0.2 * ((float64)rand() / RAND_MAX - 0.5);
        }

        // Write output signal v
        DAQmxErrChk(DAQmxWriteAnalogF64(outputTaskHandle, 1, 1, 10.0, DAQmx_Val_GroupByChannel, v, NULL, NULL));
        // Wait until output write is complete
        DAQmxErrChk(DAQmxWaitUntilTaskDone(outputTaskHandle, 10.0));

        // Read input signal
        DAQmxErrChk(DAQmxReadAnalogF64(inputTaskHandle, 1, 10.0, DAQmx_Val_GroupByChannel, inputSignalV, nChannels, NULL, NULL));

        // Print voltages read
        // printf("Input voltages: ");
        // for (int j = 0; j < nChannels; j++)
        // {
        //     printf("%.4fV ", inputSignalV[j]);
        // }
        // printf("\n");

        // Update positive perturbation signal
        for (int j = 0; j < nChannels; j++)
        {
            posV[j] = v[j] + deltaV[j];
        }
        // Write positive perturbation signal
        DAQmxErrChk(DAQmxWriteAnalogF64(outputTaskHandle, 1, 1, 10.0, DAQmx_Val_GroupByChannel, posV, NULL, NULL));
        // Wait until output write is complete
        DAQmxErrChk(DAQmxWaitUntilTaskDone(outputTaskHandle, 10.0));

        // Read input signal with positive perturbation
        DAQmxErrChk(DAQmxReadAnalogF64(inputTaskHandle, 1, 10.0, DAQmx_Val_GroupByChannel, inputSignalPlus, nChannels, NULL, NULL));

        // Update negative perturbation signal
        for (int j = 0; j < nChannels; j++)
        {
            negV[j] = v[j] - deltaV[j];
        }
        // Write negative perturbation signal
        DAQmxErrChk(DAQmxWriteAnalogF64(outputTaskHandle, 1, 1, 10.0, DAQmx_Val_GroupByChannel, negV, NULL, NULL));
        // Wait until output write is complete
        DAQmxErrChk(DAQmxWaitUntilTaskDone(outputTaskHandle, 10.0));

        // Read input signal with negative perturbation
        DAQmxErrChk(DAQmxReadAnalogF64(inputTaskHandle, 1, 10.0, DAQmx_Val_GroupByChannel, inputSignalMinus, nChannels, NULL, NULL));

        // Calculate gradient
        for (int j = 0; j < nChannels; j++)
        {
            gradient[j] = (inputSignalPlus[j] - inputSignalMinus[j]) * deltaV[j];
        }

        // Update v
        for (int j = 0; j < nChannels; j++)
        {
            v[j] += 0.2 * gradient[j];
        }
    }

    clock_t endTime = clock();
    double timeTaken = (double)(endTime - startTime) / CLOCKS_PER_SEC; // in seconds

    // Cleanup and exit
    printf("Gradient descent completed.\n");
    printf("Time taken: %.2f seconds\n", timeTaken);

Error:
    if (inputTaskHandle != 0)
    {
        DAQmxStopTask(inputTaskHandle);
        DAQmxClearTask(inputTaskHandle);
    }

    if (outputTaskHandle != 0)
    {
        DAQmxStopTask(outputTaskHandle);
        DAQmxClearTask(outputTaskHandle);
    }

    if (error != 0)
    {
        char errorBuffer[256];
        DAQmxGetExtendedErrorInfo(errorBuffer, 256);
        printf("DAQmx Error %d: %s\n", error, errorBuffer);
    }

    // Free allocated memory
    free(inputSignal);
    free(outputSignal);
    free(deltaV);
    free(v);
    free(posV);
    free(negV);
    free(inputSignalV);
    free(inputSignalPlus);
    free(inputSignalMinus);
    free(gradient);

    printf("Press Enter to quit.\n");
    getchar();
    return 0;
}
