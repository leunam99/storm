#include <unordered_map>
#include <string.h>
#include "stdint.h"
#include <iostream>
#include <fstream>
#include "eMDP.h"
#include <tuple>
#include <algorithm>
#include <algorithm>

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename StateType>
class eMDPDotGenerator {
        private: 
         bool include_action = true;
         bool include_samples = true;
         bool include_label = true;
         bool include_color = true;

         std::string color_obj(StateType color_ctr);
         std::string color_obj(std::string color);
         std::string add_labels(std::vector<std::string> label_vec);
         /*!
          * Write a transition (state1,state2) to outStream 
          * 
          * @param state1 
          * @param state2 
          * @param outStream 
          */
         void add_trans(StateType state1, StateType state2, std::ostream& outStream);

         /*!
          * Write a dotLabel for a state to outStream 
          * 
          * @param state the state for which the label should be printed 
          * @param label_vec the labels of the state 
          * @param color color for the state 
          * @param outStream 
          */
         void emdp_state_dotLabel(StateType state, std::vector<std::string> label_vec, std::string color, std::ostream& outStream);

         /*!
          * Write a dotLabel for a transition to outStream 
          * 
          * @param action the action for which the label should be printed 
          * @param samples the number of times this action has been sampled 
          * @param label_vec the labels of the action 
          * @param color_ctr color index for the transition 
          * @param outStream 
          */
         void emdp_trans_dotLabel(StateType action, StateType samples, std::vector<std::string> label_vec, StateType color_ctr, std::ostream& outStream);

         /*!
          * Recursively writes predecessors of a state to outStream 
          * 
          * @param emdp 
          * @param state 
          * @param depth the depth to which predecessors should be explored  
          * @param outStream 
          * @param visited 
          */
         void convert_pred(eMDP<StateType> emdp, StateType state, StateType depth, std::ostream& outStream, std::vector<std::tuple<StateType, StateType, StateType>> visited);

         /*!
          * Recursively writes successors of a state to outStream 
          * 
          * @param emdp 
          * @param state 
          * @param depth the depth to which successors should be explored  
          * @param outStream 
          * @param visited 
          */
         void convert_succ(eMDP<StateType> emdp, StateType state, StateType depth, std::ostream& outStream, std::vector<std::tuple<StateType, StateType, StateType>> visited);
        public: 
         /*!
          * Constructs new eMDP Dot Generator 
          * 
          */
         eMDPDotGenerator();

         /*!
          * Constructs new eMDP Dot Generator 
          * 
          * @param include_action if true include actions  
          * @param include_samples if true inlude samples 
          * @param include_label if true include labels
          * @param include_color if true color states and actions 
          */
         eMDPDotGenerator(bool include_action, bool include_samples, bool include_label, bool include_color);
         
         /*!
          * Writes the neighborhood of a state to outStream
          * 
          * @param state 
          * @param depth the depth to which neighbors should be explored 
          * @param outStream 
          */

         /*!
          * @brief 
          * 
          * @param state 
          * @param depth 
          * @param outStream 
          */
         void convert_neighborhood_eMDP(eMDP<StateType> emdp, StateType state, StateType depth, std::ostream& outStream);

         /*!
          * Writes the entire eMDP to outStream 
          * 
          * @param outStream 
          */
         void convert_eMDP(eMDP<StateType> emdp, std::ostream& outStream);

};

}
}
}