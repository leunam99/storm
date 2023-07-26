#pragma once
#include <unordered_map>
#include <string.h>
#include "stdint.h"
#include <iostream>
#include <fstream>
#include "EMdp.h"
#include <tuple>
#include <algorithm>

namespace storm {
namespace modelchecker {
namespace blackbox {

/*
TODO: 
- Coloring of states (for neib)
- Information on states samples 
*/

template<typename StateType>
class EMdpDotGenerator {
        private: 
         bool includeAction = true;
         bool includeSamples = true;
         bool includeLabel = true;
         bool includeColor = true;

         std::unordered_map<StateType, std::string> colorMap;

         std::string colorObj(StateType colorCtr);
         std::string colorObj(std::string color);
         std::string addLabels(std::vector<std::string> labelVec);
         /*!
          * Write a transition (state1,state2) to outStream 
          * 
          * @param state1 
          * @param state2 
          * @param outStream 
          */
         void addTrans(StateType state1, StateType state2, std::ostream& outStream);

         /*!
          * Write a dotLabel for a state to outStream 
          * 
          * @param state the state for which the label should be printed 
          * @param labelVec the labels of the state 
          * @param color color for the state 
          * @param outStream 
          */
         void addEMdpStateDotLabel(StateType state, std::vector<std::string> labelVec, std::string color, std::ostream& outStream);

         /*!
          * Write a dotLabel for a transition to outStream 
          * 
          * @param action the action for which the label should be printed 
          * @param samples the number of times this action has been sampled 
          * @param labelVec the labels of the action 
          * @param colorCtr color index for the transition 
          * @param outStream 
          */
         void addEMdpDotLabel(StateType action, StateType samples, std::vector<std::string> labelVec, StateType colorCtr, std::ostream& outStream);

         /*!
          * Recursively writes predecessors of a state to outStream 
          * 
          * @param EMdp 
          * @param state 
          * @param depth the depth to which predecessors should be explored  
          * @param outStream 
          * @param visited 
          */
         void convertPred(EMdp<StateType> EMdp, StateType state, StateType depth, std::ostream& outStream, std::vector<std::tuple<StateType, StateType, StateType>>* visited);

         /*!
          * Recursively writes successors of a state to outStream 
          * 
          * @param EMdp 
          * @param state 
          * @param depth the depth to which successors should be explored  
          * @param outStream 
          * @param visited 
          */
         void convertSucc(EMdp<StateType> EMdp, StateType state, StateType depth, std::ostream& outStream, std::vector<std::tuple<StateType, StateType, StateType>>* visited);
        public: 
         /*!
          * Constructs new EMdp Dot Generator 
          * 
          */
         EMdpDotGenerator();

         /*!
          * Constructs new EMdp Dot Generator 
          * 
          * @param includeAction if true include actions  
          * @param includeSamples if true inlude samples 
          * @param includeLabel if true include labels
          * @param includeColor if true color states and actions 
          */
         EMdpDotGenerator(bool incAction, bool incSamples, bool incLabel, bool incColor);
         
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
         void convertNeighborhood(EMdp<StateType> EMdp, StateType state, StateType depth, std::ostream& outStream);

         /*!
          * Writes the entire EMdp to outStream 
          * 
          * @param outStream 
          */
         void convert(EMdp<StateType> EMdp, std::ostream& outStream);

};

}
}
}