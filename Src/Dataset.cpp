// Include header files
#include "Dataset.hpp"
#include "GetPot"
#include "MyException.hpp"

// Include libraries
#include <sstream>
#include <memory>

namespace TVSFCM{
    
// Constructor
Dataset::Dataset(const T::FileNameType& filename1_, const T::FileNameType& filename2_): 
    TimeDomain(filename1_) {
    // Check the second input fle exists
    check_filename(filename2_);

    // Initialize the number of groups to zero
    n_groups = 0;

    // Initialize the rest of the class
    read_from_file(filename2_);

    // Initialize the dropout_intervals variable
    initialize_dropout_intervals();

    // Initialize the e_time matrix
    initialize_e_time();
};

// Method for reading from file
void Dataset::read_from_file(const T::FileNameType& filename2_){
    T::CheckType first_line = false;
    T::NumberType i = 0;                                // Number of row
    T::NumberType j = 0;                                // Number of column
    std::ifstream filename(filename2_.c_str());

    while(!filename.fail() & !filename.eof()){
        std::string line;
        getline(filename, line);
        std::istringstream value_line(line);

        if(!first_line){
            value_line >> n_individuals >> n_regressors;
            first_line = true;

            dataset_group.resize(n_individuals);
            time_to_event.resize(n_individuals);
            dataset.resize(n_individuals, n_regressors);
        }
        else{
            // Save the group inside the vector
            T::GroupNameType code_group;
            getline(value_line, code_group, ',');
            code_group = code_group.substr(1,4);
            dataset_group(i) = code_group;
            add_to_map_groups(code_group, i);

            // Read the regressors, one at the time,  and convert them into a double from a string
            std::string value_regressor;
            while(getline(value_line, value_regressor, ',')){
                double value = std::stod(value_regressor.c_str());
                if( j < n_regressors)
                    dataset(i,j) = value;
                else if(j == n_regressors)
                    time_to_event(i) = value;               // The last element is not a regressor but the time-to-event
                j += 1;
            }   
            // Reset j and update i
            j = 0;
            i += 1;
        }
    }
};

// Method for checking the filename is correct and exists
void Dataset::check_filename(const T::FileNameType& filename2_) const{
    std::ifstream check(filename2_);
    if(check.fail()){
        T::ExceptionType msg1 = "File ";
        T::ExceptionType msg2 = msg1.append((filename2_).c_str());
        T::ExceptionType msg3 = msg2.append(" does not exist.");
        throw MyException(msg3);
    }
};

// Method for adding individual code group name and index to the map
void Dataset::add_to_map_groups(const T::GroupNameType& name_group, const T::IndexType& index_individual){
    T::MapType::iterator group_position = map_groups.find(name_group);
    if(group_position == map_groups.end()){
        map_groups[name_group] = std::make_shared<T::VectorIndexType>();
        map_groups[name_group]->push_back(index_individual);
        n_groups += 1;
    }
    else{
        group_position->second->push_back(index_individual);
    }
};

// Initialize the dropout_intervals variable
void Dataset::initialize_dropout_intervals(){
    // Resize the matrix according to the right dimensions and fill it with null elements
    // dropout_intervals.resize(n_individuals, n_intervals);
    dropout_intervals.setZero(n_individuals, TimeDomain::n_intervals);

    // Fill the matrix according to the condition
    for(T::NumberType i = 0; i < n_individuals; ++i){
        for(T::NumberType k = 0; k < (TimeDomain::n_intervals); ++k){
            if((time_to_event(i) < TimeDomain::v_intervals(k+1)) & (time_to_event(i) >= TimeDomain::v_intervals(k)))
                dropout_intervals(i,k) = 1.;
        }
    } 
};

// Initialize the e_time matrix
void Dataset::initialize_e_time(){
    // Resize the matrix
    e_time.resize(n_individuals, TimeDomain::n_intervals);

    // Fill the matrix
    for(T::IndexType i = 0; i < n_individuals; ++i){
        T::VariableType time_individual = time_to_event(i);
        for(T::IndexType k = 0; k < (TimeDomain::n_intervals); ++k){
            T::VariableType v_k = TimeDomain::v_intervals(k);
            T::VariableType v_kk = TimeDomain::v_intervals(k+1);
            e_time(i,k) = e_time_function(time_individual, v_k, v_kk);
        }
    }
};

// Method for printing the number of individuals in each group
void Dataset::print_dimension_groups(){
    auto it_begin = map_groups.cbegin();
    auto it_end = map_groups.cend();
    T::NumberType num_individuals = 0;
    T::GroupNameType name_group;

    for(; it_begin != it_end; ++it_begin){
        name_group = it_begin -> first;
        const auto& ptr_group = it_begin -> second;
        num_individuals = (*ptr_group).size();

        std::cout << "Group " << name_group << " has " << num_individuals << " individuals " << std::endl; 
    }
}

// Define the function to compute the e_time value in the matrix
T::VariableType Dataset::e_time_function(T::VariableType time_t, T::VariableType v_k, T::VariableType v_kk){
    T::VariableType result = 0.;
    if(time_t < v_k)
        result = 0.;
    else if((time_t >= v_k) & (time_t < v_kk))
        result = (time_t - v_k);
    else if(time_t >= v_kk)
        result = (v_kk - v_k);
    
    return result;
};

// Extract the shared pointer to the name group in the map of groups
T::SharedPtrType Dataset::extract_individuals_group(const T::GroupNameType& name_group) const{
    T::MapType::const_iterator group_position = map_groups.find(name_group);
    if(group_position == map_groups.cend())
        return nullptr;
    else{
        return group_position->second;
    }
};


} // end namespace
