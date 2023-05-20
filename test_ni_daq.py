# %%
import numpy as np
import nidaqmx
from tqdm import tqdm

# %%
# Define the input and output channels
input_channel = "PXI1Slot2/ai0"
output_channels = ["PXI1Slot3/ao0", "PXI1Slot3/ao2", "PXI1Slot3/ao4", "PXI1Slot3/ao6"]

# Define the SPGD parameters
learning_rate = 5
scale = 0.4
output_scale=1

# %%
def check_range(v,scale):
    idx = np.where(np.abs(v) + scale > 9.5)
    if idx[0].size > 0:
        v[idx[0]] = 5
    return v

# %%
def check_range_pos(v,scale, ul=9.5, ll=0.5):
    for i in range(4):
        if v[i]>9.5-scale or v[i]<0.5+scale:
            v[i]=5
    return v

# %%
with nidaqmx.Task() as input_task, nidaqmx.Task() as output_task:
    
    # Configure the input task
    input_task.ai_channels.add_ai_voltage_chan(input_channel)

    # Configure the output task
    output_task.ao_channels.add_ao_voltage_chan(",".join(output_channels))
    v = 2*np.random.normal(0.2,size=(4,))#2*np.ones(4)

    # for _ in tqdm(range(100),ncols=120):
    while True:
        delta_v = scale*np.random.rand(4)

        v = check_range(v,scale)

        # print('reading..')
        output_task.write(v,auto_start=True)
        output_task.wait_until_done()
        input_signal_v = output_scale*input_task.read()

        # print('positive perturbation')
        pos_v =v+delta_v#check_range(v+delta_v)
        output_task.write(pos_v,auto_start=True)
        output_task.wait_until_done()
        input_signal_plus = output_scale*input_task.read()

        # print('negative perturbation')
        neg_v = v-delta_v#check_range(v-delta_v)
        output_task.write(neg_v,auto_start=True)
        output_task.wait_until_done()
        input_signal_minus = output_scale*input_task.read()

        gradient = (input_signal_plus - input_signal_minus) * delta_v

        v += learning_rate*gradient
        print('grad:',gradient.mean(), 'v:',v)


# %%
# with nidaqmx.Task() as input_task, nidaqmx.Task() as output_task:
    
#     # Configure the input task
#     input_task.ai_channels.add_ai_voltage_chan(input_channel)

#     # Configure the output task
#     output_task.ao_channels.add_ao_voltage_chan(",".join(output_channels))
#     v = 9.7*np.ones(4)

#     # for _ in tqdm(range(100),ncols=120):
#     while True:
#         delta_v = 1*np.random.rand(4)

#         v = check_range(v)

#         # print('reading..')
#         output_task.write(v,auto_start=True)
#         output_task.wait_until_done()
#         input_signal_v = input_task.read()

#         # print('positive perturbation')
#         pos_v =check_range(v+delta_v)
#         output_task.write(pos_v,auto_start=True)
#         output_task.wait_until_done()
#         input_signal_plus = input_task.read()

#         # print('negative perturbation')
#         neg_v = check_range(v-delta_v)
#         output_task.write(neg_v,auto_start=True)
#         output_task.wait_until_done()
#         input_signal_minus = 10*input_task.read()

#         gradient = (input_signal_plus - input_signal_minus) * delta_v

#         v += learning_rate*gradient
#         print('grad:',gradient, 'v:',v)



