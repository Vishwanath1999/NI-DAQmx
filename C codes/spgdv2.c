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
    int32 nLoop = 10000;
    float64 gain = 5.0;
    float64 perb_scale = 0.4;
    float64 outputScale = 1.0;

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
    DAQmxErrChk(DAQmxCreateAIVoltageChan(inputTaskHandle, "PXI1Slot2/ai0", "", DAQmx_Val_Diff, -10.0, 10.0, DAQmx_Val_Volts, NULL));

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
    // while{1}
    for (i = 0; i < nLoop; i++)
    {
        // Generate random deltaV values
        for (int j = 0; j < nChannels; j++)
        {
            deltaV[j] = ((float64)rand() / RAND_MAX) * perb_scale;
            // 0.2 * ((float64)rand() / RAND_MAX - 0.5);
        }

        // Write output signal v
        DAQmxErrChk(DAQmxWriteAnalogF64(outputTaskHandle, 1, 1, 10.0, DAQmx_Val_GroupByChannel, v, NULL, NULL));
        // Wait until output write is complete
        DAQmxErrChk(DAQmxWaitUntilTaskDone(outputTaskHandle, 10.0));

        // Read input signal
        DAQmxErrChk(DAQmxReadAnalogF64(inputTaskHandle, 1, 10.0, DAQmx_Val_GroupByChannel, inputSignalV, nChannels, NULL, NULL));

        // Print voltage read from ai0
        // if (i % 100 == 0)
        // {
        //     printf("Input voltage (AI0): %.4fV\n", inputSignalV[0]);
        // }

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

        // Multiply input signal with output scale after positive perturbation
        for (int j = 0; j < nChannels; j++)
        {
            inputSignalPlus[j] *= outputScale;
        }

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

        // Multiply input signal with output scale after negative perturbation
        for (int j = 0; j < nChannels; j++)
        {
            inputSignalMinus[j] *= outputScale;
        }

        // Calculate gradient
        for (int j = 0; j < nChannels; j++)
        {
            gradient[j] = (inputSignalPlus[j] - inputSignalMinus[j]) * deltaV[j];
        }

        // Update v
        for (int j = 0; j < nChannels; j++)
        {
            v[j] += gain * gradient[j];

            // Reset AO voltage if it goes beyond 9.5 or below 0.5
            if (v[j] > 9.5 || v[j] < 0.5) {
                v[j] = 5.0;
            }
        }
    }

    clock_t endTime = clock();
    double timeTaken = (double)(endTime - startTime) / (CLOCKS_PER_SEC*nLoop);

    // Cleanup and exit
    printf("Gradient descent completed.\n");
    printf("Time taken: %.7f seconds\n", timeTaken*nLoop);
    printf("Time taken: %.7f seconds per loop\n", timeTaken);

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
