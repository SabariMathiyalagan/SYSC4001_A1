/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 * @author Sabari Mathiyalagan
 * @author George Tzemenakis

 *
 */

#include<interrupts.hpp>
#include <algorithm> 

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    int current_time = 0;
    int context_save_time = 10;
    int isr_runtime = 40;



    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        if (activity == "CPU") {
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU Burst\n";
            current_time += duration_intr;
        } else if (activity == "SYSCALL" || activity == "END_IO") {
            int device_number = duration_intr;
            int total_time_left = delays[device_number];

            auto [boilerplate_text, new_time] = intr_boilerplate(current_time, device_number, context_save_time, vectors);
            execution += boilerplate_text;
            current_time = new_time;


            if (activity == "SYSCALL") {
            int run_isr_time = std::min(total_time_left, isr_runtime);
            execution += std::to_string(current_time) + ", " + std::to_string(run_isr_time) + ", SYSCALL: run the ISR (device driver)\n";
            current_time += run_isr_time;
            total_time_left -= run_isr_time;

            int transfer_data_time = std::min(total_time_left, isr_runtime);
            execution += std::to_string(current_time) + ", " + std::to_string(transfer_data_time) + ", transfer data from device to memory\n";
            current_time += transfer_data_time;
            total_time_left -= transfer_data_time;

            execution += std::to_string(current_time) + ", " + std::to_string(total_time_left) + ", check for errors\n";
            current_time += total_time_left;
            }
            else {
                int run_isr_time = std::min(total_time_left, isr_runtime);
                execution += std::to_string(current_time) + ", " + std::to_string(run_isr_time) + ", ENDIO: run the ISR (device driver)\n";
                current_time += run_isr_time;
                total_time_left -= run_isr_time;
                execution += std::to_string(current_time) + ", " + std::to_string(total_time_left) + ", check device status\n";
                current_time += total_time_left;

            }

            
        /************************************************************************/

    }

    
}
        input_file.close();

        write_output(execution);

        return 0;
}
