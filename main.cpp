#include "QuadraturePoints.hpp"
//#include "TimeDomain.hpp"
#include "Parameters.hpp"
#include "TypeTraits.hpp"
//#include "Results.hpp"
//#include "Dataset.hpp"
#include "TVModelBase.hpp"

#include <iostream>
#include <utility>

int main(){
    using T = TypeTraits;

    /*
    // Prove QuadraturePoints formula
    QuadraturePoints::Points9 points9;
    std::array<T::VariableType, 9>& nodes = points9.nodes;
    for(const auto& n: nodes)
        std::cout << n << std::endl;
    */
    
    /*
    // Prove Time class for errors
    std::cout << "Direct initialization" << std::endl;
    TimeDomainInfo::TimeDomain time("DataToolFile.txt");
    std::cout << time.get_time_begin() << std::endl;
    std::cout << time.get_n_intervals() << std::endl;
    time.print_v_intervals();

    std::cout << "Using a different initialization" << std::endl;
    TimeDomainInfo::TimeDomain time_alternative(TimeDomainInfo::TimeDomain("DataToolFile.txt"));
    time.print_v_intervals();
    */
    
    /*
    // Prova Parameters class and Results class for errors 
    T::VectorNumberType all_n_parameters{3,4,2,1};
    Params::Parameters params("DataToolFile.txt", 4, 10, 3, 4, all_n_parameters);
    T::VectorXdr & v_params = params.get_v_parameters();
    T::NumberType & n_params = params.get_n_parameters();
    T::VariableType opt_likelihoood = -10.0;
    std::cout << "Number of parameters: "<< n_params << std::endl;
    std::cout << v_params << std::endl;
    */

    /*
    ResultsMethod::Results results("Paik", n_params, v_params, opt_likelihoood);
    results.print_results();
    */

    /*
    // Prova Dataset class for errors
    DatasetInfoClass::DatasetInfo database("DataToolFile.txt", "DataIndividualsFile.txt");
    database.print_dataset();
    database.print_dataset_group();
    database.print_map_groups();
    database.print_dropout_intervals();
    database.print_individuals_group("EngC");
    */

    
    // Prova TVModelBase fo errors
    TVModel::ModelBase modelbase("DataToolFile.txt", "DataIndividualsFile.txt");
    modelbase.print_map_groups();
    modelbase.print_n_regressors();
    modelbase.print_n_intervals();

    

    return 0;
}

