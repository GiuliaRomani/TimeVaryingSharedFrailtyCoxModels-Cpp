#ifndef MODEL_BASE_HPP
#define MODEL_BASE_HPP

// Include header files
#include "Dataset.hpp"
#include "Results.hpp"
#include "ParallelComponents.hpp"

// Include 
#include <iostream>

// Class
namespace TVSFCM{
using T = TypeTraits;

/**
 * ModelBase class: abstract base class for any Time-Varying Shared Frailty Cox model. It define the basic data and function members that
 * all the three different models required. 
 * 
 * 
 * It is abstract since no instaces of this class are allowed. 
 * 
 * Some of the methods are virtual to use polymorphism through an object factory.
 * 
 * The methods are pure to permit each model to define its own implementation.
 * 
 * 
 * It is composed of the Results class for storing the result of a model application and 
 * it is publicly derived by the Dataset class and ParallelComponents,
 * for using the dataset, some useful variables of the temporal domain and the variables of the parallel computing.
*/

class ModelBase: public Dataset,
                 public ParallelComponents{
public:
    /** 
     * Default Constructor
    */
    ModelBase() = default;

    /**
     * Constructor that initializes the base Dataset class and define the dimension of the protected components
     * @param filename1 Name of the file containing both time and parameters related variables
     * @param filename2 Name of the file containing the dataset
    */
    ModelBase(const T::FileNameType& filename1, const T::FileNameType& filename2);

    /**
     * Virtual pure method for the computation of the model log-likelihood 
    */
    virtual void evaluate_loglikelihood() = 0;

    /**
     * Method for printing the results of the model application. It calls the print_results() method
     * of the Result class
    */
    void print_results() const noexcept;

    /**
     * Default destructor
    */
    virtual ~ ModelBase() = default;


protected:
    Results result;                                                         //! Results of the model application
                                              
    T::VariableType h_dd;                                                   //! Discretization step of the second derivative

    T::VectorXdr variance_frailty;                                          //! Vector for time-interval variance of the frailty
    T::VectorXdr sd_frailty;                                                //! Vector for time-interval sd of the frailty
    
    T::VectorXdr hessian_diag;                                              //! Diagonal of the Hessian matrix of the log-likelihood function
    T::VectorXdr se;                                                        //! Standard error of the parameters

    /** 
     * Virtual pure method for computing the model number of parameters
    */
    virtual void compute_n_parameters() = 0;

    /**
     * Virtual pure method that constructs the model log-likelihood 
    */
    virtual void build_loglikelihood() = 0;

    /**
     * Virtual pure method for constructing the log-likelihood in the parallel version
    */
    virtual void build_loglikelihood_parallel() = 0;

    /**
     * Virtual pure method that construct the centered finite difference scheme for the computation
     * of the second derivative of the log-likelihood. An accuracy of the secon order is chosen.
    */
    virtual void build_dd_loglikelihood() = 0;
    
    /**
     * Virtual pure method that initializes the diagonal of the hessian matrix and evaluate it in the 
     * optimal parameters vector. An implementation for each model is provided due to the different log-likelihood function.
     * @param v_parameters_ Optimal vector of parameters
    */
    virtual void compute_hessian_diagonal(T::VectorXdr& v_parameters_) = 0;

    /**
     * Virtual pure method that initializes the standard error of the optimal
     * vector of parameters
     * @param v_parameters_ Optimal vector of parameters
    */
    virtual void compute_se(T::VectorXdr& v_parameters_) = 0;
    
    /**
     * Virtual pure method for initializing and computing the standard deviation 
     * of the frailty variance
     * @param v_parameters_ Optimal vector of parameters
    */
    virtual void compute_sd_frailty(T::VectorXdr& v_parameters_) = 0;

    /**
     * Method for checking that the provided discretization step is positive, otherwise an exception is thrown.
     * @param h_dd_ Discretization step
    */
    void check_condition(T::VariableType h_dd_) const;

};

} // end namespace

#endif // MODEL_BASE_HPP

