# enduroSat
task

Use the "folder_setup.sh" script for setting up everything you need.
Let me know if you encounter any issues. 

If the script is having a hard time, you can also find everything you need in the Setup.txt file



The documents describe the state machines Master(deviceA) and Slave(deviceB).

If there's are multiple arrows without anything attached to them, it means that this is a start state.
They are left like this because of the criteria which states that "deviceB's states should be pseudo-random"

Arrows pointing in two directions mean that you can go from state IDLE to state ERROR or the other way around (IDLE -> ERROR and ERROR IDLE are both valid)


In the documentation you can find more information about different flags, functions and their use.

documentation:  https://docs.google.com/document/d/1D2u0EJWoE5WjvPIoDr9wv08szge0Fju7JxAlzpD_opM/edit?tab=t.0
There are flow charts which explain how the state works. It is not mentioned in the documentation due to the randomness of taskB's states(and simplicity).