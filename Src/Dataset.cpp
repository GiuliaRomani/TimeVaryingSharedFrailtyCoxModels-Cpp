// Include header files
#include "Dataset.hpp"
#include "GetPot"
#include "MyException.hpp"

// Include 
#include <sstream>
#include <memory>

namespace TVSFCM{
    
// Constructor
Dataset::Dataset(const T::FileNameType& filename1_, const T::FileNameType& filename2_): 
    TimeDomain(filename1_) {

    // Initialize the number of groups to zero
    n_groups = 0;

    // Read the variables from input file
    read_from_file(filename2_);

    // Initialize the dropout_intervals variable
    initialize_dropout_intervals();

    // Initialize the e_time matrix
    initialize_e_time();
};

// Method for reading data variables from file
void Dataset::read_from_file(const T::FileNameType& filename2_){
    T::CheckType first_line = false;                    // If first line of the file is read, then it becomes true
    T::NumberType i = 0;                                // Number of row
    T::NumberType j = 0;                                // Number of column
    T::IntType n_individuals_, n_regressors_;           // Number of individuals and regressors of the dataset

    std::ifstream filename(filename2_.c_str());

    // Open the file, check its existence and read until its end
    while(!filename.fail() & !filename.eof()){
        std::string line;
        getline(filename, line);
        std::istringstream value_line(line);

        // Read the first line and check correctness of the read variables
        if(!first_line){
            value_line >> n_individuals_ >> n_regressors_;
            check_condition(n_individuals_, n_regressors_);
            first_line = true;

            dataset_group.resize(n_individuals);
            time_to_event.resize(n_individuals);
            dataset.resize(n_individuals, n_regressors);
        }
        else{ 
            // From the second line onward, save the group name inside the correspondent vector
            T::GroupNameType code_group;
            getline(value_line, code_group, ',');
            code_group = code_group.substr(1,4);
            dataset_group(i) = code_group;
            add_to_map_groups(code_group, i);

            // Read the regressors, one at the time, and convert them into a double from a string.
            std::string value_regressor;
            while(getline(value_line, value_regressor, ',')){
                T::VariableType value = std::stod(value_regressor.c_str());
                if( j < n_regressors)
                    dataset(i,j) = value;
                else if(j == n_regressors){     // The last element is not a regressor but the time-to-event, that must be non-negative
                    if(value < 0)
                        throw MyException("The provided time-to-event is negative.");
                    time_to_event(i) = value;               
                }   
                // Update the number of columns of the dataset
                j += 1;     
            }   
            // Reset j and update the row i
            j = 0;
            i += 1;
        }
    }
};

// Method for cheking that the future dimensions of the dataset are non-negative
void Dataset::check_condition(const T::IntType n_individuals_, const T::IntType n_regressors_){
    if(n_individuals_ < 0)
        throw MyException("Provided negative number of individuals.");
    else if(n_regressors_ < 0)
        throw MyException("Provided negative number of regressors.");
    
    // If they are correct, convert them from an integer to un unsigned integer
    n_individuals = static_cast<T::NumberType>(n_individuals_);
    n_regressors = static_cast<T::NumberType>(n_regressors_);
}

// Method for adding the dataset row index of an individual belonging to a group, to the map
void Dataset::add_to_map_groups(const T::GroupNameType& name_group, const T::IndexType& index_individual) noexcept{
    // Find the group name inside the map. 
    // If not present, add it, create the shared pointer and add the index to the shared pointer
    // Otherwise add only the index to the shared pointer.
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
void Dataset::initialize_dropout_intervals() noexcept{
    // Resize the matrix according to the right dimensions and fill it with null elements
    dropout_intervals.setZero(n_individuals, TimeDomain::n_intervals);

    // Fill the matrix according to in which interval the individual fails, if fails
    for(T::IndexType i = 0; i < n_individuals; ++i){
        for(T::IndexType k = 0; k < (TimeDomain::n_intervals); ++k){
            if((time_to_event(i) < TimeDomain::v_intervals[k+1]) & (time_to_event(i) >= TimeDomain::v_intervals[k]))
                dropout_intervals(i,k) = 1.;
        }
    } 
};

// Initialize the e_time matrix
void Dataset::initialize_e_time() noexcept{
    // Resize the matrix
    e_time.setZero(n_individuals, TimeDomain::n_intervals);

    // Fill the matrix using the method e_time_function(...)
    for(T::IndexType i = 0; i < n_individuals; ++i){
        T::TimeType time_individual = time_to_event(i);
        for(T::IndexType k = 0; k < (TimeDomain::n_intervals); ++k){
            T::TimeType v_k = TimeDomain::v_intervals[k];
            T::TimeType v_kk = TimeDomain::v_intervals[k+1];
            e_time(i,k) = e_time_function(time_individual, v_k, v_kk);
        }
    }
};

// Define the function to compute the e_time value in the matrix, using the definition reported in Appendix A
T::TimeType Dataset::e_time_function(const T::TimeType time_t, const T::TimeType v_k, const T::TimeType v_kk) const noexcept{
    T::TimeType result = 0;
    if(time_t < v_k)
        result = 0;
    else if((time_t >= v_k) & (time_t < v_kk))
        result = (time_t - v_k);
    else if(time_t >= v_kk)
        result = (v_kk - v_k);
    
    return result;
};

// From the map of groups, extract the shared pointer to the group vector
T::SharedPtrType Dataset::extract_individuals_group(const T::GroupNameType& name_group) const noexcept{
    // Find the group name inside the map.
    // If the group name exists, return the shared pointer. Otherwise a null pointer.
    T::MapType::const_iterator group_position = map_groups.find(name_group);
    if(group_position == map_groups.cend())
        return nullptr;
    else{
        return group_position->second;
    }
};

// Method for printing the number of individuals in each group
void Dataset::print_dimension_groups() const noexcept{
    auto it_begin = map_groups.cbegin();
    auto it_end = map_groups.cend();
    T::NumberType num_individuals = 0;
    T::GroupNameType name_group;

    // From the first element of the map, to the last
    for(; it_begin != it_end; ++it_begin){
        name_group = it_begin -> first;
        const auto& ptr_group = it_begin -> second;
        num_individuals = (*ptr_group).size();
        std::cout << "Group " << name_group << " has " << num_individuals << " individuals " << std::endl; 
    }
};

} // end namespace

